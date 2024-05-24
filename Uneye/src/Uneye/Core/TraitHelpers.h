#pragma once

namespace Uneye
{
	template<typename, typename T>
	struct has_OnComponentAdded_in_scene;

	// C - Component
	// Ret - Function
	// Args - Arguments
	template<typename C, typename Ret, typename... Args>
	struct has_OnComponentAdded_in_scene<C, Ret(Args...)>;
}
