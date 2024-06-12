#pragma once

namespace Uneye
{
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4,
		Mat3, Mat4,
		UInt32, UInt64,
		Int, Int2, Int3, Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case Uneye::ShaderDataType::None:		return 0;

			case Uneye::ShaderDataType::Float:		return 4;
			case Uneye::ShaderDataType::Float2:		return 4 * 2;
			case Uneye::ShaderDataType::Float3:		return 4 * 3;
			case Uneye::ShaderDataType::Float4:		return 4 * 4;

			case Uneye::ShaderDataType::Mat3:		return 4 * 3 * 3;
			case Uneye::ShaderDataType::Mat4:		return 4 * 4 * 4;

			case Uneye::ShaderDataType::UInt32:		return 4;
			case Uneye::ShaderDataType::UInt64:		return 4 * 2;

			case Uneye::ShaderDataType::Int:		return 4;
			case Uneye::ShaderDataType::Int2:		return 4 * 2;
			case Uneye::ShaderDataType::Int3:		return 4 * 3;
			case Uneye::ShaderDataType::Int4:		return 4 * 4;

			case Uneye::ShaderDataType::Bool:		return 1;
		}

		UNEYE_CORE_ASSERT(true, "Unknown ShaderDataType!");
		return 0;
	}


	struct BufferElement
	{
		std::string Name = " ";
		ShaderDataType Type = ShaderDataType::None;
		uint32_t Size = 0;
		uint32_t Offset = 0;
		bool Normalized = false;

		BufferElement() {}
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		:Name(name), Type(type), Size(ShaderDataTypeSize(type)),
			Offset(0), Normalized(normalized) 
		{}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case Uneye::ShaderDataType::None:		return 0;
				case Uneye::ShaderDataType::Float:		return 1;
				case Uneye::ShaderDataType::Float2:		return 2;
				case Uneye::ShaderDataType::Float3:		return 3;
				case Uneye::ShaderDataType::Float4:		return 4;
				case Uneye::ShaderDataType::Mat3:		return 3 * 3;
				case Uneye::ShaderDataType::Mat4:		return 4 * 4;
				case Uneye::ShaderDataType::UInt32:		return 1;
				case Uneye::ShaderDataType::UInt64:		return 1;
				case Uneye::ShaderDataType::Int:		return 1;
				case Uneye::ShaderDataType::Int2:		return 2;
				case Uneye::ShaderDataType::Int3:		return 3;
				case Uneye::ShaderDataType::Int4:		return 4;
				case Uneye::ShaderDataType::Bool:		return 1;
			}

			UNEYE_CORE_ASSERT(true, "Unknown ShaderDataType!");
			return 0;
		}
	};


	class BufferLayout
	{
		public:
			BufferLayout() {}
			BufferLayout(const std::initializer_list<BufferElement>& element)
				: m_Elements(element)
			{
				CalculateOffsetAndStride();
			}
			inline uint32_t GetStride() const { return m_Stride; }
			inline const std::vector<BufferElement>& GetElements() const { return m_Elements;  }

			std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
			std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
			std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
			std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

		private:
			void CalculateOffsetAndStride()
			{
				uint32_t offset = 0;
				m_Stride = 0;
				for (auto& element : m_Elements)
				{
					element.Offset = offset;
					offset += element.Size;
					m_Stride += element.Size;
				}
			}

		private:
			std::vector<BufferElement> m_Elements;
			uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
		public:
			virtual ~VertexBuffer() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual const BufferLayout& GetLayout() const = 0;
			virtual void SetLayout(const BufferLayout& layout) = 0;

			virtual void SetData(const void* data, uint32_t size) = 0;

			static Ref<VertexBuffer> Create(uint32_t size);
			static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
		public:
			virtual ~IndexBuffer() = default;

			virtual uint32_t GetCount() const = 0;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	};
}
