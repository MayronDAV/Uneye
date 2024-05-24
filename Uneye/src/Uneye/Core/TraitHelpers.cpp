#include "uypch.h"
#include "TraitHelpers.h"
#include <type_traits>

namespace Uneye
{
	// Generic template for has_OnComponentAdded_in_scene
	template<typename, typename T>
	struct has_OnComponentAdded_in_scene
	{
		static constexpr bool value = false;
	};

	template<typename C, typename Ret, typename... Args>
	struct has_OnComponentAdded_in_scene<C, Ret(Args...)>
	{
		using type = decltype(check<C>(0));

		public:
			// type value
			// if std::true_type OnComponentAdded exists
			// if std::false_type OnComponentAdded  don't exists
			static constexpr bool value = type::value;

		private:
			// Checks if an OnComponentAdded function exists
			template<typename T>
			static constexpr auto check(T*) ->
				typename std::is_same<
				decltype(std::declval<T>().OnComponentAdded(std::declval<Args>()...)),
				Ret
				>::type;
			// If not exists return std::false_type
			template<typename>
			static constexpr std::false_type check(...) { return {}; }
	};

}
