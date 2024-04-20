#pragma once

#include "Event.h"

#include <sstream>



namespace Uneye
{
	class UNEYE_API KeyEvent : public Event
	{
		public:

			inline int GetKeyCode() const { return m_KeyCode; }

			EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

		protected:
			KeyEvent(int keyCode) : m_KeyCode(keyCode) {}

			int m_KeyCode;
	};

	class UNEYE_API KeyPressedEvent : public KeyEvent
	{
		public:
			KeyPressedEvent(int KeyCode, int RepeatCount)
			: KeyEvent(KeyCode), m_RepeatCount(RepeatCount) {};

			inline int GetRepeatCount() const { return m_RepeatCount; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyPressedEvent: " << m_KeyCode << " ("
					<< m_RepeatCount << " repeats)";

				return ss.str();
			}
			
			EVENT_CLASS_TYPE(KeyPressed)

		private:
			int m_RepeatCount;

	};


	class UNEYE_API KeyReleasedEvent : public KeyEvent
	{
		public:
			KeyReleasedEvent(int KeyCode)
				: KeyEvent(KeyCode) {};

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyReleasedEvent: " << m_KeyCode;
				return ss.str();
			}

			EVENT_CLASS_TYPE(KeyPressed)

	};

};