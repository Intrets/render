#pragma once

#include <span>
#include <vector>

#include <cassert>

namespace render
{
	template<class T>
	struct BatchInfo
	{
		size_t start{};
		size_t end{};
		T batchData{};

		bool empty() const;
	};

	template<class T>
	struct BatchInfos
	{
		std::vector<BatchInfo<T>> data{};

		void newBatch(T const& batchData, size_t start);
		void endBatch(size_t end);
	};

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

		void append(RenderInfoBase<T> const& other) {
			this->data.append_range(other.data);
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

		T* getData() const {
			return this->data.data();
		}
	};

	template<class T>
	struct RenderInfoTemplate;

	template<class T>
	inline bool BatchInfo<T>::empty() const {
		return this->end == this->start;
	}

	template<class T>
	inline void BatchInfos<T>::newBatch(T const& batchData, size_t start) {
		if (!this->data.empty() && this->data.back().batchData == batchData) {
			return;
		}
		else {
			this->data.push_back({ .start = start, .end = start, .batchData = batchData });
		}
	}
	template<class T>
	inline void BatchInfos<T>::endBatch(size_t end) {
		assert(!this->data.empty());

		this->data.back().end = end;
		if (this->data.back().empty()) {
			this->data.pop_back();
		}
	}
}