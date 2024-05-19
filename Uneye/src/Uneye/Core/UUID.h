#pragma once

#include <xhash>

namespace Uneye
{
	class UUID
	{
		public:
			UUID();
			UUID(uint64_t uuid);
			UUID(const UUID&) = default;

			operator uint64_t() const { return m_UUID;  }
			
		private:
			uint64_t m_UUID;
	};

}


namespace std
{
	template<>
	struct hash<Uneye::UUID>
	{
		std::size_t operator()(const Uneye::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};
}
