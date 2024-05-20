#include "uypch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"



namespace Uneye
{
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


	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;


	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();

		LoadAssembly("Resources/Scripts/Uneye-ScriptCore.dll");

		ScriptGlue::RegisterFunction();

		ScriptClass mainClass("Uneye", "Main");

		// Call method
		mainClass.CallMethod("PrintMessage");

		// Call method with one param
		int value = 33;
		void* param = &value;
		mainClass.CallMethod("PrintInt", 1, &param);

		// Call method with 2 param
		int value1 = 33;
		int value2 = 18;
		void* params[2] =
		{
			&value1,
			&value2
		};
		mainClass.CallMethod("PrintInts", 2, params);

		// Call method with string param
		MonoString* str = mono_string_new(s_Data->AppDomain, "Hello World from C++!!!");
		void* paramStr = str;
		mainClass.CallMethod("PrintCustomMessage", 1, &paramStr);

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

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* classInstance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(classInstance);

		return classInstance;
	}



	ScriptClass::ScriptClass( const std::string& classNamespace, const std::string& className, bool instantiate)
		: m_Namespace(classNamespace), m_Name(className), m_Instantiate(instantiate)
	{
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());

		if (m_Instantiate)
		{
			m_ClassObject = mono_object_new(s_Data->AppDomain, m_MonoClass);
			mono_runtime_object_init(m_ClassObject);
		}

	}

	MonoClass* ScriptClass::GetClass() { return m_MonoClass; }

	MonoObject* ScriptClass::GetInstance()
	{
		if (!m_Instantiate)
		{
			m_ClassObject = mono_object_new(s_Data->AppDomain, m_MonoClass);
			mono_runtime_object_init(m_ClassObject);
		}

		return m_ClassObject; 
	}

	MonoObject* ScriptClass::CallMethod(const std::string& methodName, int param_count, void** params, MonoObject** exc)
	{
		MonoMethod* method = mono_class_get_method_from_name(m_MonoClass, methodName.c_str(), param_count);
		return mono_runtime_invoke(method, m_ClassObject, params, exc);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& methodName, int param_count)
	{
		return mono_class_get_method_from_name(m_MonoClass, methodName.c_str(), param_count);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoMethod* method, void** params, MonoObject** exc)
	{
		return mono_runtime_invoke(method, m_ClassObject, params, exc);
	}
}
