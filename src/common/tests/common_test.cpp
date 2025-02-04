
#include "enumiterator.h"

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <fakeit.hpp>

#include <QtWidgets/QApplication>
#include <cstddef> // size_t, ptrdiff_t
#include <cstdint> // uint16_t
#include <iconlabel.h>
#include <iterator> // iterator_traits, begin, end
#include <limits> // numeric_limits
#include <type_traits> // is_same
#include <utility> // swap, next, advance
#include <vector> // vector

enum class Trivial { A, B, C };

enum class TrivialStart { A = 0, B, C, D, E };

enum class NonTrivial : std::uint16_t { A = 100, B = 101, C = 102 };

enum class NoPastTheEndUnsigned : std::size_t {
    A = std::numeric_limits<std::size_t>::min(),
    B = std::numeric_limits<std::size_t>::max()
};

enum class NoPastTheEndSigned : std::ptrdiff_t {
    A = std::numeric_limits<std::ptrdiff_t>::min(),
    B = std::numeric_limits<std::ptrdiff_t>::max()
};

TEST_CASE("EnumIterator singularity", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it;
    REQUIRE(it.end() == it);
    REQUIRE(it.begin() != it);
}

TEST_CASE("EnumIterator non-singular", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it{ Trivial::B };
    REQUIRE(it.end() != it);
    REQUIRE(it.begin() != it);
    REQUIRE(I{ Trivial::C } == ++it);
}

TEST_CASE("EnumIterator operator*", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it{ Trivial::A };
    REQUIRE(it.end() != it);

    REQUIRE(I{ Trivial::B } == ++it);
    REQUIRE(Trivial::B == *it);
    REQUIRE(I{ Trivial::C } == ++it);
    REQUIRE(Trivial::C == *it);

    const I it1{ Trivial::A };
    REQUIRE(it1.end() != it1);
    REQUIRE(Trivial::A == *it1);
    REQUIRE(it.begin() == it1);
}

TEST_CASE("EnumIterator operator->", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it{ Trivial::A };
    REQUIRE(it.end() != it);

    REQUIRE(I{ Trivial::B } == ++it);
    REQUIRE(Trivial::B == it.operator->());
    REQUIRE(I{ Trivial::C } == ++it);
    REQUIRE(Trivial::C == it.operator->());

    const I it1{ Trivial::A };
    REQUIRE(it1.end() != it1);
    REQUIRE(Trivial::A == it1.operator->());
    REQUIRE(it.begin() == it1);
}

TEST_CASE("EnumIterator operator++", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it1{ Trivial::A };
    REQUIRE(it1.end() != it1);
    REQUIRE(Trivial::B == *++it1);
    REQUIRE(Trivial::C == *++it1);
    REQUIRE(it1.end() == ++it1);
    REQUIRE(it1.end() == ++it1);

    I it2{ Trivial::A };
    REQUIRE(it2.end() != it2);
    REQUIRE(Trivial::B == *it2++);
    REQUIRE(Trivial::A == *it2);

    const I it3{ Trivial::A };
    REQUIRE(it3.end() != it3);
    REQUIRE(it3.begin() == it3);

    I it4;
    REQUIRE(it4.end() == it4);
    REQUIRE(it4.end() == ++it4);
}

TEST_CASE("EnumIterator swap", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it1{ Trivial::B };
    I it2{ Trivial::C };
    REQUIRE(Trivial::B == *it1);
    REQUIRE(Trivial::C == *it2);

    using std::swap;
    swap(it1, it2);

    REQUIRE(Trivial::C == *it1);
    REQUIRE(Trivial::B == *it2);
}

TEST_CASE("EnumIterator non-trivial", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<NonTrivial, NonTrivial::A, NonTrivial::C>;

    I it{ NonTrivial::A };
    REQUIRE(it.end() != it);
    REQUIRE(it.begin() == it);

    REQUIRE(NonTrivial::A == *it);
    REQUIRE(NonTrivial::B == *++it);
    REQUIRE(NonTrivial::C == *++it);
    REQUIRE(it.end() == ++it);
}

TEST_CASE("EnumIterator iterator_traits", "[common]")
{
    using T = std::iterator_traits<EnumIterator<NonTrivial, NonTrivial::A, NonTrivial::C>>;

    (void)std::is_same<void, T::difference_type>::value;
    (void)std::is_same<void, T::value_type>::value;
    (void)std::is_same<void, T::pointer>::value;
    (void)std::is_same<void, T::reference>::value;
    (void)std::is_same<void, T::iterator_category>::value;
}

TEST_CASE("EnumIterator next", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<NonTrivial, NonTrivial::A, NonTrivial::C>;

    I it{ NonTrivial::A };
    REQUIRE(I{ NonTrivial::B } == std::next(it));
    REQUIRE(NonTrivial::A == *it);

    std::advance(it, 2);
    REQUIRE(NonTrivial::C == *it);

    std::advance(it, 666);
    REQUIRE(it.end() == it);
}

TEST_CASE("EnumIterator range-based-for non-trivial", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<NonTrivial, NonTrivial::A, NonTrivial::C>;

    const std::vector<NonTrivial> a = { NonTrivial::A, NonTrivial::B, NonTrivial::C };
    std::vector<NonTrivial> b;

    for (auto&& i : I{ NonTrivial::A }) {
        b.emplace_back(i);
    }

    REQUIRE(a == b);
}

TEST_CASE("EnumIterator range-based-for trivial", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    const std::vector<Trivial> a = { Trivial::A, Trivial::B, Trivial::C };
    std::vector<Trivial> b;

    for (auto&& i : I{ Trivial::A }) {
        b.emplace_back(i);
    }

    REQUIRE(a == b);
}

TEST_CASE("EnumIterator range-based-for trivial-start", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<TrivialStart, TrivialStart::A, TrivialStart::E>;

    const std::vector<TrivialStart> a
        = { TrivialStart::A, TrivialStart::B, TrivialStart::C, TrivialStart::D, TrivialStart::E };
    std::vector<TrivialStart> b;

    for (auto&& i : I{ TrivialStart::A }) {
        b.emplace_back(i);
    }

    REQUIRE(a == b);
}

TEST_CASE("IconLable sizeHint", "[common]")
{
    IconLabel l("name", QColor(0, 0, 0), QColor(0, 0, 0));

    REQUIRE(l.minimumSizeHint() == QSize(140, 48));
}

// -- compile-time errors:
/*
TEST_CASE("EnumIterator invalid-value", "[common]")
{
    using I = EnumIterator<double, 0.0, 1.0>;

    I it;
}

TEST_CASE("EnumIterator invalid-param", "[common]")
{
    using I = EnumIterator<int, 0, 1>;

    I it;
}

TEST_CASE("EnumIterator invalid-range", "[common]")
{
    using I = EnumIterator<Trivial, Trivial::C, Trivial::A>;

    I it;
}

TEST_CASE("EnumIterator no-past-the-end", "[common]")
{
    using namespace fakeit;

    using I1 = EnumIterator<NoPastTheEndUnsigned
                          , NoPastTheEndUnsigned::A
                          , NoPastTheEndUnsigned::B>;

    I1 it1;

    using I2 = EnumIterator<NoPastTheEndSigned
                          , NoPastTheEndSigned::A
                          , NoPastTheEndSigned::B>;

    I2 it2;
}
*/
int main(int argc, char* argv[])
{
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
