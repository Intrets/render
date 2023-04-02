#pragma once

#include <span>
#include <vector>

namespace render
{
	template<class T>
	struct RenderInfoBase
	{
		std::vector<T> data;

		std::span<T> resizeAdditional(int i) {
			assert(i >= 0);
			auto current = this->data.size();
			this->data.resize(this->data.size() + i);

			return std::span(this->data.begin() + current, i);
		}

		void add(T&& t) {
			this->data.push_back(std::forward<T>(t));
		}

		void add(T const& t) {
			this->data.push_back(t);
		}

		size_t getSize() const {
			return this->data.size();
		}

		size_t getByteSize() const {
			return this->data.size() * sizeof(T);
		}

		T* getData() const {
			return this->data.data();
		}
	};

	template<class T>
	struct RenderInfoTemplate;
}