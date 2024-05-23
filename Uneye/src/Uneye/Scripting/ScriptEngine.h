#pragma once

#include "Uneye/Scene/Scene.h"
#include "Uneye/Scene/Entity.h"

#include <filesystem>
#include <string>

#include <unordered_map>
#include <map>

#pragma region MonoStuff

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoType MonoType;
}

#pragma endregion

namespace Uneye
{
	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	namespace Utils
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

		static std::unordered_map<std::string, ScriptFieldType> s_StringToScriptFieldMap =
		{
			{ "None",   ScriptFieldType::None	  },
			{ "Float",  ScriptFieldType::Float	  },
			{ "Double", ScriptFieldType::Double   },
			{ "Bool",	ScriptFieldType::Bool     },
			{ "Char",	ScriptFieldType::Char	  },
			{ "Short",	ScriptFieldType::Short    },
			{ "Int",	ScriptFieldType::Int	  },
			{ "Long",	ScriptFieldType::Long     },
			{ "Byte",	ScriptFieldType::Byte	  },
			{ "UShort", ScriptFieldType::UShort   },
			{ "UInt",	ScriptFieldType::UInt	  },
			{ "ULong",  ScriptFieldType::ULong    },

			{ "Vector2", ScriptFieldType::Vector2 },
			{ "Vector3", ScriptFieldType::Vector3 },
			{ "Vector4", ScriptFieldType::Vector4 },

			{ "Entity", ScriptFieldType::Entity   },
		};
		
		static ScriptFieldType MonoTypeToScriptFieldType(MonoType* type);


		static std::string ConvertScriptFieldMapToString(const ScriptFieldType& type)
		{
			auto it = s_ScriptFieldTypeString.find(type);
			if (it == s_ScriptFieldTypeString.end())
				return "None";

			return it->second;
		}

		static ScriptFieldType ConvertStringToScriptFieldMap(const std::string& type)
		{
			auto it = s_StringToScriptFieldMap.find(type);
			if (it == s_StringToScriptFieldMap.end())
				return ScriptFieldType::None;

			return it->second;
		}

	}


	struct ScriptFieldInstance
	{
		public:
			ScriptField Field;

			ScriptFieldInstance()
			{
				memset(m_Buffer, 0, sizeof(m_Buffer));
			}

			template<typename T>
			T GetValue()
			{
				static_assert(sizeof(T) <= 16 , "Type too large!!!");

				return *(T*)m_Buffer;
			}

			template<typename T>
			void SetValue(T value)
			{
				static_assert(sizeof(T) <= 16, "Type too large!!!");

				memcpy(m_Buffer, &value, sizeof(T));
			}

		private:
			uint8_t  m_Buffer[16];

			friend class ScriptEngine;
			friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass
	{
		public:
			ScriptClass() = default;

			ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);

			MonoClass* GetClass();

			MonoObject* Instantiate();

			MonoMethod* GetMethod(const std::string& methodName, int param_count = 0);

			MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr, MonoObject** exc = nullptr);

			const std::map<std::string, ScriptField>& GetFields() { return m_Fields; }


		private:
			std::string m_Namespace = std::string();
			std::string m_Name = std::string();

			std::map<std::string, ScriptField> m_Fields;

			MonoClass* m_MonoClass = nullptr;

			friend class ScriptEngine;
	};

	class ScriptInstance
	{
		public:
			ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

			void InvokeOnCreate();
			void InvokeOnUpdate(float ts);

			Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

			template<typename T>
			T GetFieldValue(const std::string& name)
			{
				static_assert(sizeof(T) <= 16, "Type too large!!!");

				bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
				if (!success)
					return T();
				
				return *(T*)s_FieldValueBuffer;
			}

			template<typename T>
			void SetFieldValue(const std::string& name, T value)
			{
				static_assert(sizeof(T) <= 16, "Type too large!!!");

				SetFieldValueInternal(name, &value);
			}

			MonoObject* GetManageObject() { return m_Instance; }

		private:
			bool GetFieldValueInternal(const std::string& name, void* buffer);
			bool SetFieldValueInternal(const std::string& name, const void* value);

		private:

			Ref<ScriptClass> m_ScriptClass; 

			MonoObject* m_Instance = nullptr;
			MonoMethod* m_Constructor = nullptr;
			MonoMethod* m_OnCreateMethod = nullptr;
			MonoMethod* m_OnUpdateMethod = nullptr;

			inline static char s_FieldValueBuffer[16];

			friend class ScriptEngine;
			friend struct ScriptFieldInstance;
	};

	//class Scene;
	//class Entity;

	class ScriptEngine
	{
		public:
			static void Init();
			static void Shutdown();

			static void LoadAssembly(const std::filesystem::path& filepath);
			static void LoadAppAssembly(const std::filesystem::path& filepath);

			static void OnRuntimeStart(Scene* scene);
			static void OnRuntimeStop();

			static void LoadAssemblyClasses();


			static bool EntitySubClassExists(const std::string& fullClassName);

			static void OnCreateEntity(Entity entt);
			static void OnUpdateEntity(Entity entt, Timestep ts);

			static Scene* GetSceneContext();

			static Ref<ScriptInstance> GetEntityScriptInstance(UUID enttID);

			static Ref<ScriptClass> GetEntityClass(const std::string& name);
			static std::unordered_map<std::string, Ref<ScriptClass>> GetEntitySubClasses();
			static ScriptFieldMap& GetScriptFieldMap(Entity entt);
			
			static MonoImage* GetCoreAssemblyImage();

			static MonoObject* GetManageInstance(UUID enttID);

		private:
			static void InitMono();
			static void ShutdownMono();

			static MonoObject* InstantiateClass(MonoClass* monoClass);

			friend class ScriptClass;
			friend class ScriptGlue;
	};
}
