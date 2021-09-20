#pragma once

#include <wglm/glm.hpp>
#include <string>
#include <concepts>

namespace render
{
	struct Color
	{
		uint32_t c;
	};

	static constexpr Color red = { 0xFF0000FF };
	static constexpr Color green = { 0xFF00FF00 };
	static constexpr Color blue = { 0xFFFF0000 };
	static constexpr Color magenta = { 0xFFFF00FF };
	static constexpr Color yellow = { 0xF00FFFFF };
	static constexpr Color cyan = { 0xFFFFFF00 };
	static constexpr Color white = { 0xFFFFFFFF };

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
