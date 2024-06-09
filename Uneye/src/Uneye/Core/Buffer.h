#pragma once

#include <stdint.h>
#include <cstring>

namespace Uneye
{
	struct Buffer
	{
		unsigned char* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;

		Buffer(uint64_t size)
		{
			Allocate(size);
		}

		Buffer(const void* data, uint64_t size)
			: Data((unsigned char*)data), Size(size)
		{
		}

		Buffer(const Buffer&) = default;

		static Buffer Copy(Buffer other)
		{
			Buffer result(other.Size);
			memcpy(result.Data, other.Data, other.Size);
			return result;
		}

		void Allocate(uint64_t size)
		{
			Release();

			Data = (unsigned char*)malloc(size);
			Size = size;
		}

		void Release()
		{
			free(Data);
			Data = nullptr;
			Size = 0;
		}

		template<typename T>
		T* As()
		{
			return (T*)Data;
		}

		operator bool() const
		{
			return (bool)Data;
		}

	};

	struct ScopedBuffer
	{
			ScopedBuffer(Buffer buffer)
				: m_Buffer(buffer)
			{
			}

			ScopedBuffer(uint64_t size)
				: m_Buffer(size)
			{
			}

			~ScopedBuffer()
			{
				m_Buffer.Release();
			}

			unsigned char* Data() { return m_Buffer.Data; }
			uint64_t Size() { return m_Buffer.Size; }

			template<typename T>
			T* As()
			{
				return m_Buffer.As<T>();
			}

			operator bool() const { return m_Buffer; }
		private:
			Buffer m_Buffer;
	};


}
