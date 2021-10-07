#pragma once

#include <wglm/glm.hpp>

#include <string>
#include <concepts>
#include <bit>

#include "SRGBConversion.h"

namespace render
{
	struct Color
	{
		uint32_t c;
	};

	template<class T>
	constexpr T id(T t) { return t; };

	constexpr Color toLinear(Color other) {
		struct H
		{
			uint8_t a;
			uint8_t b;
			uint8_t g;
			uint8_t r;
		};

		H h = std::bit_cast<H>(other);

		h.r = ::toLinear(h.r);
		h.g = ::toLinear(h.g);
		h.b = ::toLinear(h.b);
		h.a = h.a;

		return std::bit_cast<Color>(h);
	}

	using converterFunctionType = Color(*)(Color);
	constexpr converterFunctionType converter = toLinear;

	static constexpr Color red = converter({ 0xFF0000FF });
	static constexpr Color green = converter({ 0xFF00FF00 });
	static constexpr Color blue = converter({ 0xFFFF0000 });
	static constexpr Color magenta = converter({ 0xFFFF00FF });
	static constexpr Color yellow = converter({ 0xFF00FFFF });
	static constexpr Color cyan = converter({ 0xFFFFFF00 });
	static constexpr Color white = converter({ 0xFFFFFFFF });
	static constexpr Color dlue = converter({ 0xFF261F00 });
	static constexpr Color nice_blue = converter({ 0xFFFFAC00 });
	static constexpr Color nice_green = converter({ 0xFF00CA1D });
	static constexpr Color nice_red = converter({ 0xFF2525E5 });
	static constexpr Color orange = converter({ 0xFF00B2FF });

	template<class T>
	Color uniqueColor(T val) {
		return { static_cast<uint32_t>(std::hash<T>{}(val)) | 0xFF000000 };
	}

	constexpr Color alpha(Color c, std::integral auto val) {
		assert(0 <= val && val < 256);

		return { c.c & (0x00FFFFFF | (static_cast<uint32_t>(val) << 24)) };
	}

	constexpr Color alpha(Color c, std::floating_point auto val) {
		assert(0.0 <= val && val < 1.0);

		return alpha(c, static_cast<int32_t>(255 * val));
	}
}
