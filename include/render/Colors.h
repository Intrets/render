// render - A C++ OpenGL library
// Copyright (C) 2021  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <wglm/glm.hpp>

#include <string>
#include <concepts>
#include <bit>
#include <array>

#include "SRGBConversion.h"

namespace render
{
	struct Color
	{
		uint32_t c;
	};

	template<class T>
	constexpr T id(T t) {
		return t;
	};

	constexpr Color toLinear(Color other) {
		struct H
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
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

	constexpr Color rgb(uint32_t r, uint32_t g, uint32_t b) {
		return { r | (g << 8) | (b << 16) | 0xFF000000 };
	}

	constexpr Color crgb(uint32_t r, uint32_t g, uint32_t b) {
		return converter(rgb(r, g, b));
	}

	constexpr Color rgba(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
		return { r | (g << 8) | (b << 16) | (a << 24) };
	}

	constexpr Color crgba(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
		return converter(rgba(r, g, b, a));
	}

	static constexpr Color red = converter({ 0xFF0000FF });
	static constexpr Color green = converter({ 0xFF00FF00 });
	static constexpr Color blue = converter({ 0xFFFF0000 });
	static constexpr Color magenta = converter({ 0xFFFF00FF });
	static constexpr Color yellow = converter({ 0xFF00FFFF });
	static constexpr Color cyan = converter({ 0xFFFFFF00 });
	static constexpr Color white = converter({ 0xFFFFFFFF });
	static constexpr Color dlue = converter({ 0xFF261F00 });
	static constexpr Color nice_blue = converter({ 0xFFFFAC00 });
	static constexpr Color nice_darkblue = converter({ 0xFFFF6900 });
	static constexpr Color nice_green = converter({ 0xFF00CA1D });
	static constexpr Color nice_red = converter({ 0xFF2525E5 });
	static constexpr Color orange = converter({ 0xFF00B2FF });
	static constexpr Color light_gray = converter({ 0xFF404040 });
	static constexpr Color gray = converter({ 0xFF808080 });
	static constexpr Color dark_gray = converter({ 0xFFBFBFBF });
	static constexpr Color gray_red = converter({ 0xFF404080 });
	static constexpr Color gray_green = converter({ 0xFF408040 });
	static constexpr Color gray_blue = converter({ 0xFF804040 });

	static constexpr Color gray_translucent = converter(rgba(0, 0, 0, 100));


	static constexpr std::array<Color, 5> nice_colors = {
		nice_blue,
		nice_darkblue,
		nice_green,
		nice_red,
		orange,
	};

	static constexpr std::array<Color, 30> reds_and_purples = {
		converter(rgb(205,  92,  92)),
		converter(rgb(178,  34,  34)),
		converter(rgb(165,  42,  42)),
		converter(rgb(233, 150, 122)),
		converter(rgb(250, 128, 114)),
		converter(rgb(255, 160, 122)),
		converter(rgb(255, 127,  80)),
		converter(rgb(240, 128, 128)),
		converter(rgb(255,  99,  71)),
		converter(rgb(255,  69,   0)),
		converter(rgb(255,   0,   0)),
		converter(rgb(255, 105, 180)),
		converter(rgb(255,  20, 147)),
		converter(rgb(255, 192, 203)),
		converter(rgb(255, 182, 193)),
		converter(rgb(219, 112, 147)),
		converter(rgb(176,  48,  96)),
		converter(rgb(199,  21, 133)),
		converter(rgb(208,  32, 144)),
		converter(rgb(255,   0, 255)),
		converter(rgb(238, 130, 238)),
		converter(rgb(221, 160, 221)),
		converter(rgb(218, 112, 214)),
		converter(rgb(186,  85, 211)),
		converter(rgb(153,  50, 204)),
		converter(rgb(148,   0, 211)),
		converter(rgb(138,  43, 226)),
		converter(rgb(160,  32, 240)),
		converter(rgb(147, 112, 219)),
		converter(rgb(216, 191, 216)),
	};


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
