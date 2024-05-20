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

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void Entity_GetTranslation(ulong entityID, out Vector3 translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void Entity_SetTranslation(ulong entityID, ref Vector3 translation);

		#endregion

		#region Events

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool Input_IsKeyDown(KeyCode keycode);

		#endregion
	}
}
