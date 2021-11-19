#include <iostream>

#include "render/RendererTemplate.h"

struct Foo { int a; };
struct Bar { std::string s; };
struct Baz { std::vector<int> ok; };

struct Test
{
	Foo foo;
	Bar bar;
	Baz baz;

	//Test(Foo, Bar, Baz) {};
};

template<class... Args>
struct A
{

};

int main() {
	TEMPLATES::RenderInfo().test();
	//TEMPLATES::RenderInfo<Foo, Bar, Baz>().addSingleTest(Foo(), Bar(), Baz());
	//TEMPLATES::RenderInfo2<Test>().addSingleTest(Foo(), Bar(), Baz());

	//te::replicate_t<10, int> bbb;

	//constexpr bool b1 = te::has_members<Test, te::WildCard>;
	//constexpr bool b2 = te::has_members<Test, te::WildCard, te::WildCard>;
	//constexpr bool b3 = te::has_members<Test, te::WildCard, te::WildCard, te::WildCard>;

	constexpr bool b0 = std::is_trivially_constructible_v<Test, te::WildCard, te::WildCard, te::WildCard>;
	constexpr bool b1 = std::is_constructible_v<Test, te::WildCard, te::WildCard, te::WildCard, te::WildCard>;

	//auto w4 = Test{ te::WildCard(), te::WildCard(), te::WildCard(), te::WildCard() };
	//auto w3 = Test{ te::WildCard(), te::WildCard(), te::WildCard() };
	//auto w2 = Test{ te::WildCard(), te::WildCard() };
	//auto w1 = Test{ te::WildCard() };

	auto a = Test(te::WildCard());

	te::has_n_members_f(te::Type<Test>, te::Value<3>);

	constexpr auto test = te::Type_function<std::is_constructible>;

	constexpr auto losing = std::is_constructible_v<Test, te::WildCard, te::WildCard, te::WildCard>;

	constexpr auto ok = Getvalue(test(te::Type<Test>, te::Type<te::WildCard>));



	constexpr bool b3 = te::has_n_members_v<Test, 0>;
	constexpr bool b4 = te::has_n_members_v<Test, 1>;
	constexpr bool b5 = te::has_n_members_v<Test, 2>;
	constexpr bool b6 = te::has_n_members_v<Test, 3>;
	constexpr bool b7 = te::has_n_members_v<Test, 4>;



	te::Dest<Test>::type bbb;

	rand();

	return 0;
}