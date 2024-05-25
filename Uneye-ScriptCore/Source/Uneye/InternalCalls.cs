using System;
using System.Runtime.CompilerServices;


namespace Uneye
{
	public static class InternalCalls
	{
		#region LogCalls

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void NativeLog(string message);

		#endregion

		#region EntityCalls

		#region Entity

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static ulong Entity_FindFirstEntityByName( string name);

		[ MethodImpl(MethodImplOptions.InternalCall) ]
		internal extern static object GetScriptInstance( ulong entityID );

		#endregion

		#region TransformComponent

		[ MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);

		#endregion

		#region Rigidbody2DComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetTransform( ulong entityID, ref Vector2 impulse, float angle);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_GetLinearVelocity( ulong entityID, out Vector2 linearVelocity );

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static Rigidbody2DComponent.BodyType Rigidbody2DComponent_GetType( ulong entityID );

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void Rigidbody2DComponent_SetType( ulong entityID, Rigidbody2DComponent.BodyType type );

		#endregion

		#endregion

		#region Events

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyDown(KeyCode keycode);

		#endregion
	}
}
