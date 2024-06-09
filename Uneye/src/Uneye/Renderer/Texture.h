#pragma once

#include "Uneye/Core/Base.h"
#include "Uneye/Core/Buffer.h"

#include "Uneye/Asset/Asset.h"

#include <string>



namespace Uneye
{
	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
	};

	class Texture : public Asset
	{
		public:
			virtual ~Texture() = default;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;
			virtual uint32_t GetChannels() const = 0;
			virtual uint32_t GetRendererID() = 0;

			virtual uint64_t GetEstimatedSize() const = 0;

			virtual const TextureSpecification& GetSpecification() const = 0;


			virtual std::vector<unsigned char>& GetData() = 0;

			virtual void SetData(Buffer data) = 0;

			virtual void Bind(uint32_t slot = 0) = 0;
			virtual void Unbind() = 0;

			virtual bool operator== (const Texture& other) const = 0;
	};


	class Texture2D : public Texture
	{
		public:
			static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer data = Buffer());
			
			virtual void ChangeSize(uint32_t p_width, uint32_t p_height) = 0;

			static AssetType GetStaticType() { return AssetType::Texture2D; }
			virtual AssetType GetType() const { return GetStaticType(); }
	};

}
