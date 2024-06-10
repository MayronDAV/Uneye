#pragma once

namespace Uneye
{
	class ErrorWindow
	{
		public:
			static void Show(const std::string& message, const std::string& title = "Error");
	};
}
