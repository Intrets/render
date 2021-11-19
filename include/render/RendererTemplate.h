#pragma once

#include <vector>

#include <tepp/dest.h>

namespace INPUT
{
	struct SingleRenderInfo
	{
		float x;
		//float y;
		//float z;
		//int c;
	};
}

namespace TEMPLATES
{
	// template<class Single>
	template<class... Args>
	struct RenderInfo
	{
		using Single = INPUT::SingleRenderInfo;
		using RenderInfoMembers = te::get_members_t<Single>;

		std::vector<Single> data;

		void addSingleTest(Args&&... args) {

		}

		void test() {
			RenderInfoMembers a;
			rand();
		}
	};

	template<class Single>
	using RenderInfo2 = te::convert_to_t<te::type_function_t<RenderInfo>, te::get_members_t<Single>>;
}
