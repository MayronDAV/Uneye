#include "uypch.h"
#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"



namespace Uneye
{
	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	namespace Utils
	{
		char* ReadBytes(const std::string& filepath, uint32_t* outSize)
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

		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				UNEYE_CORE_CRITICAL(errorMessage);
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
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

		MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			MonoClass* klass = mono_class_from_name(image, namespaceName, className);

			UNEYE_CORE_ASSERT(klass == nullptr, "");

			return klass;
		}

		MonoObject* InstantiateClass(const char* namespaceName, const char* className)
		{
			// Get a reference to the class we want to instantiate
			MonoClass* testingClass = GetClassInAssembly(s_Data->CoreAssembly, namespaceName, className);

			// Allocate an instance of our class
			MonoObject* classInstance = mono_object_new(s_Data->AppDomain, testingClass);

			UNEYE_CORE_ASSERT(classInstance == nullptr, "");

			// Call the parameterless (default) constructor
			mono_runtime_object_init(classInstance);

			return classInstance;
		}
	}


	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();
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

		s_Data->AppDomain = mono_domain_create_appdomain("UneyeScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Move this maybe
		s_Data->CoreAssembly = Utils::LoadCSharpAssembly("Resources/Scripts/Uneye-ScriptCore.dll");

		Utils::PrintAssemblyTypes(s_Data->CoreAssembly);

		MonoClass* monoClass = Utils::GetClassInAssembly(s_Data->CoreAssembly, "Uneye", "Main");

		// 1 - create an object ( and call constructor)
		MonoObject* classInstance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(classInstance);

		// 2 - call function
		MonoMethod* printMessageFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
		mono_runtime_invoke(printMessageFunc, classInstance, nullptr, nullptr);

		// 1 - call function with param
		{
			MonoMethod* printIntFunc = mono_class_get_method_from_name(monoClass, "PrintInt", 1);

			int value = 33;
			void* param = &value;

			mono_runtime_invoke(printIntFunc, classInstance, &param, nullptr);
		}
		
		{
			MonoMethod* printIntsFunc = mono_class_get_method_from_name(monoClass, "PrintInts", 2);

			int value1 = 33;
			int value2 = 18;

			void* params[2] =
			{
				&value1,
				&value2
			};

			mono_runtime_invoke(printIntsFunc, classInstance, params, nullptr);
		}

		{

			MonoMethod* printCustomMessageFunc = mono_class_get_method_from_name(monoClass, "PrintCustomMessage", 1);
			
			MonoString* str = mono_string_new(s_Data->AppDomain, "Hello World from C++!!!");

			void* param = str;
			
			mono_runtime_invoke(printCustomMessageFunc, classInstance, &param, nullptr);
		}
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
}
