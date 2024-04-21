#pragma once

#include "Uneye/Core.h"
#include "Uneye/Events/Event.h"


namespace Uneye
{
	class UNEYE_API Layer
	{
		public:
			Layer(const std::string& name = "Layer");
			virtual ~Layer() = default;

			virtual void OnAttach() {}
			virtual void OnDetach() {}
			virtual void OnUpdate() {}
			virtual void OnEvent(Event& event) {}

			const std::string& GetName() const { return m_DebugName; }

		protected:
			std::string m_DebugName;
	};
};

