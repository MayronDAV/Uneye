#include "uypch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"
#include "Uneye/Core/Buffer.h"
#include "Uneye/Core/FileSystem.h"

#include "Uneye/Core/Application.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"

#include "filewatch/FileWatch.h"



namespace Uneye
{
	namespace Utils
	{

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false)
		{
			uint32_t fileSize = 0;
			ScopedBuffer fileData = FileSystem::ReadFileBinary(assemblyPath);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData.As<char>(), (uint32_t)fileData.Size(), 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				UNEYE_CORE_CRITICAL(errorMessage);
				return nullptr;
			}

			if (loadPDB)
			{
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");

				if (std::filesystem::exists(pdbPath))
				{
					uint32_t pdbFileSize = 0;
					ScopedBuffer pdbFileData = FileSystem::ReadFileBinary(pdbPath);
					mono_debug_open_image_from_memory(image, pdbFileData.As<const mono_byte>(), (int)pdbFileData.Size());
					UNEYE_CORE_INFO("Loaded PDB {}", pdbPath);
				}
			}

			std::string pathStr = assemblyPath.string();

			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathStr.c_str(), &status, 0);
			mono_image_close(image);

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

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* type)
		{
			const std::string& typeName = mono_type_get_name(type);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
				return ScriptFieldType::None;

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

		std::filesystem::path CoreAssemblyFilepath;
		std::filesystem::path AppAssemblyFilepath;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntitySubClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;

		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

		Scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;

		bool EnableDebugging = true;

		// Runtime
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

#pragma region ScriptEngine

	void ScriptEngine::Init()
	{
		UNEYE_PROFILE_FUNCTION();

		s_Data = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunction();

		LoadAssembly("Resources/Scripts/Uneye-ScriptCore.dll");
		LoadAppAssembly("SandboxProject/Assets/Scripts/Binaries/Sandbox.dll");

		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		s_Data->EntityClass = ScriptClass("Uneye", "Entity", true);
		//Utils::PrintAssemblyTypes(s_Data->AppAssembly);


		MonoObject* instance = s_Data->EntityClass.Instantiate();

		//UNEYE_CORE_ASSERT(true);
	}

	void ScriptEngine::Shutdown()
	{

		ShutdownMono();
		delete s_Data;
	}

	void ScriptEngine::InitMono()
	{
		UNEYE_PROFILE_FUNCTION();

		mono_set_assemblies_path("mono/lib");

		if (s_Data->EnableDebugging)
		{
			const char* argv[2] = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};

			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}


		MonoDomain* rootDomain = mono_jit_init("UneyeJITRuntime");

		UNEYE_CORE_ASSERT(rootDomain == nullptr);

		s_Data->RootDomain = rootDomain;

		if (s_Data->EnableDebugging)
			mono_debug_domain_create(s_Data->RootDomain);

		mono_thread_set_main(mono_thread_current());
	}

	void ScriptEngine::ShutdownMono()
	{
		UNEYE_PROFILE_FUNCTION();

		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}


	void ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		s_Data->CoreAssemblyFilepath = filepath;

		s_Data->AppDomain = mono_domain_create_appdomain("UneyeScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Move this maybe
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		//Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		if (!s_Data->AssemblyReloadPending && change_type == filewatch::Event::modified)
		{
			s_Data->AssemblyReloadPending = true;

			Application::Get().SubmitToMainThread([]() 
			{
				s_Data->AppAssemblyFileWatcher.reset();
				ScriptEngine::ReloadAssembly(); 
			});
		}

		switch (change_type)
		{
			case filewatch::Event::added:
				UNEYE_TRACE("path: {0} - type: added", path); break;
			case filewatch::Event::modified:
				UNEYE_TRACE("path: {0} - type: modified", path); break;
			case filewatch::Event::removed:
				UNEYE_TRACE("path: {0} - type: removed", path); break;
			case filewatch::Event::renamed_new:
				UNEYE_TRACE("path: {0} - type: renamed_new", path); break;
			case filewatch::Event::renamed_old:
				UNEYE_TRACE("path: {0} - type: renamed_old", path); break;
		}
	}
	
	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppAssemblyFilepath = filepath;

		s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath, s_Data->EnableDebugging);
		//Utils::PrintAssemblyTypes(s_Data->AppAssembly);
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);

		s_Data->AppAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(
			filepath.string(),
			OnAppAssemblyFileSystemEvent
		);

		s_Data->AssemblyReloadPending = false;
	}

	void ScriptEngine::ReloadAssembly()
	{
		UNEYE_PROFILE_FUNCTION();

		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);
		//mono_domain_free(s_Data->AppDomain, true);

		LoadAssembly(s_Data->CoreAssemblyFilepath);
		LoadAppAssembly(s_Data->AppAssemblyFilepath);

		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		s_Data->EntityClass = ScriptClass("Uneye", "Entity", true);
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

	Ref<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		auto it = s_Data->EntitySubClasses.find(name);
		if (it == s_Data->EntitySubClasses.end())
			return nullptr;

		return it->second;
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entt)
	{
		UNEYE_CORE_ASSERT(!entt);

		UUID entityID = entt.GetUUID();
		return s_Data->EntityScriptFields[entityID];
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		UNEYE_PROFILE_FUNCTION();

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

					scriptClass->m_Fields[fieldname] = { fieldType, fieldname, field };
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
		UNEYE_PROFILE_FUNCTION();

		const auto& sc = entt.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntitySubClassExists(sc.Name))
		{
			UUID enttID = entt.GetUUID();
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntitySubClasses[sc.Name], entt);
			s_Data->EntityInstances[enttID] = instance;

			// Copy field values
			if (s_Data->EntityScriptFields.find(enttID) != s_Data->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields.at(enttID);
				for (const auto& [name, fieldInstance] : fieldMap)
					instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
			}


			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entt, Timestep ts)
	{
		UNEYE_PROFILE_FUNCTION();

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

	MonoObject* ScriptEngine::GetManageInstance(UUID enttID)
	{
		auto it = s_Data->EntityInstances.find(enttID);
		if (it == s_Data->EntityInstances.end())
			return nullptr;

		return it->second->GetManageObject();
	}

#pragma endregion


#pragma region ScriptClass

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
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
		m_Instance = m_ScriptClass->Instantiate();

		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
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
