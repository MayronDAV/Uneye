#pragma once

namespace Uneye
{

	class GraphicsContext
	{
		public:
			virtual void Init() = 0;
			virtual void SwapBuffers() = 0;

		private:
	};


}