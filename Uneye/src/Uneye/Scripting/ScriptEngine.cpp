#include "uypch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"



namespace Uneye
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{ "System.Single",  ScriptFieldType::Float   },
		{ "System.Double",  ScriptFieldType::Double  },
		{ "System.Boolean", ScriptFieldType::Bool    },
		{ "System.Char",	ScriptFieldType::Char    },
		{ "System.Int16",	ScriptFieldType::Short   },
		{ "System.Int32",	ScriptFieldType::Int     },
		{ "System.Int64",	ScriptFieldType::Long    },
		{ "System.Byte",	ScriptFieldType::Byte    },
		{ "System.UInt16",	ScriptFieldType::UShort  },
		{ "System.UInt32",	ScriptFieldType::UInt    },
		{ "System.UInt64",	ScriptFieldType::ULong   },

		{ "Uneye.Vector2",	ScriptFieldType::Vector2 },
		{ "Uneye.Vector3",	ScriptFieldType::Vector3 },
		{ "Uneye.Vector4",	ScriptFieldType::Vector4 },

		{ "Uneye.Entity",	ScriptFieldType::Entity	 },
	};

	static std::unordered_map<ScriptFieldType, const char*> s_ScriptFieldTypeString =
	{
		{ ScriptFieldType::None,	"None"    },
		{ ScriptFieldType::Float,   "Float"   },
		{ ScriptFieldType::Double,  "Double"  },
		{ ScriptFieldType::Bool,    "Bool"	  },
		{ ScriptFieldType::Char,    "Char"	  },
		{ ScriptFieldType::Short,   "Short"	  },
		{ ScriptFieldType::Int,     "Int"	  },
		{ ScriptFieldType::Long,    "Long"	  },
		{ ScriptFieldType::Byte,    "Byte"	  },
		{ ScriptFieldType::UShort,  "UShort"  },
		{ ScriptFieldType::UInt,    "UInt"	  },
		{ ScriptFieldType::ULong,   "ULong"	  },

		{ ScriptFieldType::Vector2, "Vector2" },
		{ ScriptFieldType::Vector3, "Vector3" },
		{ ScriptFieldType::Vector4, "Vector4" },

		{ ScriptFieldType::Entity,  "Entity"  },
	};

	namespace Utils
	{
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			UNEYE_CORE_ASSERT(!stream, "");

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			UNEYE_CORE_ASSERT(size == 0, "");

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath.string(), &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				UNEYE_CORE_CRITICAL(errorMessage);
				return nullptr;
			}

			std::string pathStr = assemblyPath.string();

			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		static void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				printf("%s.%s\n", nameSpace, name);
			}
		}

		static ScriptFieldType MonoTypeToScriptFieldType(MonoType* type)
		{
			const std::string& typeName = mono_type_get_name(type);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
				return ScriptFieldType::None;

			return it->second;
		}

		static const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
		{
			auto it = s_ScriptFieldTypeString.find(fieldType);
			if (it == s_ScriptFieldTypeString.end())
				return "Invalid";

			return it->second;
		}

	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntitySubClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;

		// Runtime
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	#pragma region ScriptEngine

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();

		LoadAssembly("Resources/Scripts/Uneye-ScriptCore.dll");
		LoadAppAssembly("SandboxProject/Assets/Scripts/Binaries/Sandbox.dll");

		LoadAssemblyClasses();

		Utils::PrintAssemblyTypes(s_Data->AppAssembly);

		ScriptGlue::RegisterComponents();
		ScriptGlue::RegisterFunction();

		s_Data->EntityClass = ScriptClass("Uneye", "Entity", true);

		MonoObject* instance = s_Data->EntityClass.Instantiate();

		//UNEYE_CORE_ASSERT(true, "");
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("UneyeJITRuntime");
		
		UNEYE_CORE_ASSERT(rootDomain == nullptr, "");

		s_Data->RootDomain = rootDomain;
	}

	void ScriptEngine::ShutdownMono()
	{
		// WHY, WHY MONO
		// maybe come back to this

		//mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		//mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}


	void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppDomain = mono_domain_create_appdomain("UneyeScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Move this maybe
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		//Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		// Move this maybe
		s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath);
		//Utils::PrintAssemblyTypes(s_Data->AppAssembly);
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);

	}


	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;

		s_Data->EntityInstances.clear();
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID enttID)
	{
		auto it = s_Data->EntityInstances.find(enttID);
		if (it == s_Data->EntityInstances.end())
			return nullptr;

		return it->second;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntitySubClasses()
	{
		return s_Data->EntitySubClasses;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntitySubClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Uneye", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, className);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity)
				continue;
			
			Ref<ScriptClass>  scriptClass = CreateRef<ScriptClass>(nameSpace, className);
			s_Data->EntitySubClasses[fullName] = scriptClass;


			int fieldlNum = mono_class_num_fields(monoClass);
			UNEYE_CORE_INFO("{0} has {1} fields: ", className, fieldlNum);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldname = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);

				//UNEYE_CORE_WARN("	{0} flags = {1}", fieldname, flags);

				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* type = mono_field_get_type(field);
					//const char* typeName = mono_type_get_name(type);

					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);

					//UNEYE_CORE_WARN("	{} ({})", fieldname, Utils::ScriptFieldTypeToString(fieldType));
				
					scriptClass->m_Fields[fieldname] = {fieldType, fieldname, field};
				}
			}
		}

	}

	bool ScriptEngine::EntitySubClassExists(const std::string& fullClassName)
	{
		return s_Data->EntitySubClasses.find(fullClassName) != s_Data->EntitySubClasses.end();
	}

	void ScriptEngine::OnCreateEntity(Entity entt)
	{
		const auto& sc = entt.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntitySubClassExists(sc.Name))
		{
			UUID enttID = entt.GetUUID();
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntitySubClasses[sc.Name], entt);
			s_Data->EntityInstances[enttID] = instance;

			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entt, Timestep ts)
	{
		UUID entityUUID = entt.GetUUID();
		if (s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end())
		{
			Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
			instance->InvokeOnUpdate((float)ts);
		}
		else
		{
			UNEYE_CORE_ERROR("Could not find ScriptInstance for entity {}", entityUUID);
		}
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* classInstance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(classInstance);

		return classInstance;
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	#pragma endregion


	#pragma region ScriptClass

	ScriptClass::ScriptClass( const std::string& classNamespace, const std::string& className, bool isCore)
		: m_Namespace(classNamespace), m_Name(className)
	{
		m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());

	}

	MonoClass* ScriptClass::GetClass() { return m_MonoClass; }

	MonoObject* ScriptClass::Instantiate()
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, m_MonoClass);
		mono_runtime_object_init(instance);

		return instance;
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& methodName, int param_count)
	{
		return mono_class_get_method_from_name(m_MonoClass, methodName.c_str(), param_count);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params, MonoObject** exc)
	{
		return mono_runtime_invoke(method, instance, params, exc);
	}

	#pragma endregion


	#pragma region ScriptInstance

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		:m_ScriptClass(scriptClass)
	{
		m_Instance		 = m_ScriptClass->Instantiate();

		m_Constructor    = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = m_ScriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = m_ScriptClass->GetMethod("OnUpdate", 1);

		// Call the entt constructor
		{
			UUID enttID = entity.GetUUID();
			void* param = &enttID;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod != nullptr)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_OnUpdateMethod != nullptr)
		{
			void* param = &ts;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
		}
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		ScriptField field = it->second;
		mono_field_get_value(m_Instance, field.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		ScriptField field = it->second;
		mono_field_set_value(m_Instance, field.ClassField, (void*)value);
		return true;
	}

	#pragma endregion
}
