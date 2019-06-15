#include <gtest/gtest.h>

#include "ParserDriver.hpp"
#include "TestUtils.hpp"

namespace
{
const std::string_view TEST_DATA = R"(
    typedef long long MyType, MyOtherType;

    typedef sequence<octet> data;

    struct my_str {
        long l;
        short x,y;
    };

    interface asdf : name::first, ::second, third
    {

        attribute int test_attr;

        readonly attribute int read_attr;

        attribute type_t multi_attr_1, multi_attr_2;
    };

    abstract interface abs_asdf;
    local interface loc_asdf;

    struct fields {
        short aaa ;
        long long_var;
        long long long_var;

        unsigned short aaa ;
        unsigned long long_var;
        unsigned long long long_var;

        float long_var;
        double long_var;
        long double long_var;

        char long_var;
        boolean long_var;
        any long_var;
        octet long_var;
    };

    interface asdf
    {
        struct my_str {};

        oneway void my_op();

        long double my_op(in long p1);

        some_type my_op(in long p1, out type_t out_param);
    };

    module mod {
        interface asdf {

            attribute long test_attr;

            readonly attribute long long read_attr;

            attribute type_t test_attr_1, aaa;
        };

        struct my_str {
            long double abc;
        };
    };
)";

} // namespace

TEST(TestFlex, test)
{
    ASSERT_TRUE(utils::init("./test.data", TEST_DATA));

    std::ifstream stream;
    stream.open("./test.data", std::ios::in);
    ASSERT_TRUE(stream.good());

    ParserDriver p;
    EXPECT_TRUE(p.parse(stream));

    auto &scope = p.get_top_scope();
    EXPECT_EQ(10U, scope.size());

    utils::print_nodes(std::cout, &scope, 0);
}
