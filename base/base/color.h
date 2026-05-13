#pragma once

#include <cstdint>

namespace Rc
{
	struct HSV
	{
		float h {0.0f};
		float s {0.0f};
		float v {0.0f};
		float a {1.0f};
	};

	struct RGBA8
	{
		uint8_t r {0};
		uint8_t g {0};
		uint8_t b {0};
		uint8_t a {255};
	};

	//// 0xRRGGBBAA
	//enum class UnormRGBA8 : uint32_t {};
	//
	//// 0xAABBGGRR
	//enum class ABGR8 : uint32_t {};
	//
	//// 0xBBGGRRAA
	//enum class BGRA8 : uint32_t {};
	//
	//// 0xAARRGGBB
	//enum class ARGB8 : uint32_t {};

	class Color
	{
	public:
		float r {0.0f};
		float g {0.0f};
		float b {0.0f};
		float a {1.0f};
		
		// From string...

		static constexpr Color Rgb8(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept
		{
			return {
				static_cast<float>(r) / 255.0f,
				static_cast<float>(g) / 255.0f,
				static_cast<float>(b) / 255.0f,
				static_cast<float>(a) / 255.0f
			};
		}

		// Make RGB color form HSV
		static Color Hsv(float h, float s, float v, float a) noexcept;

		static Color Hsv8(uint8_t h, uint8_t s, uint8_t v, uint8_t a) noexcept
		{
			return Color::Hsv(
				static_cast<float>(h) / 255.0f,
				static_cast<float>(s) / 255.0f,
				static_cast<float>(v) / 255.0f,
				static_cast<float>(a) / 255.0f
			);
		}

		
		float const* Data() const noexcept { return &r; }

		void Set(float r, float g, float b, float a) noexcept;

		operator HSV() const noexcept;
		
		// store to; store metody jsou rychlejsi nez cast metody, nevytvari se docasny objekt
		// void StoreUnorm( ColorUnorm& target ) const;

		Color& operator+=(Color const& rhs) noexcept
		{
			Set(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
			return *this;
		}

		Color operator+(Color const& rhs) const noexcept
		{
			return {r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a};
		}

		Color& operator-=(Color const& rhs) noexcept
		{
			Set(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
			return *this;
		}

		Color operator-(Color const& rhs) const noexcept
		{
			return {r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a};
		}

		Color operator*(float s) const noexcept
		{
			return {r * s, g * s, b * s, a * s};
		}

		Color operator*=(float s) noexcept
		{
			Set(r * s, g * s, b * s, a * s);
			return *this;
		}
	};

	inline Color operator*(float s, Color const& c) noexcept
	{
		return {c.r * s, c.g * s, c.b * s, c.a * s};
	}

} // Rc