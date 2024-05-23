using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


namespace Uneye
{
	public class Entity
	{
		protected Entity() { ID = 0; }

		internal Entity(ulong id)
		{
			ID = id;
		}
		public readonly ulong ID;

		/// <summary>
		/// Change the entity's translation.
		/// <example>
		/// For example:
		/// <code>
		/// Vector3 m_Translation = Translation;
		/// m_Translation *= m_Direction * m_Speed * m_DeltaTime;
		/// Translation = m_Translation;
		/// </code>
		/// </example>
		/// </summary>
		public Vector3 Translation
		{
			get
			{
				InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
				return result;
			}
			set
			{
				InternalCalls.TransformComponent_SetTranslation(ID, ref value);
			}
		}

		public bool HasComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(ID, componentType);
		}


		/// <typeparam name="T">ComponentType</typeparam>
		/// <returns>Component or null</returns>
		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return null;

			T component = new T() { Entity = this };
			return component;
		}

		/// <summary>
		/// Be careful when using this function inside an OnUpdate function
		/// </summary>
		/// <returns>Entity or null</returns>
		public Entity FindFirstEntityByName( string name)
		{
			ulong enttID = InternalCalls.Entity_FindFirstEntityByName(name);

			if(enttID == 0)
				return null;

			return new Entity(enttID);
		}

		public T As<T>() where T : Entity, new()
		{
			object instance = InternalCalls.GetScriptInstance(ID);
			return instance as T;
		}
	}
}
