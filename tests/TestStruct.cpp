#include <gtest/gtest.h>

#include "ParserDriver.hpp"
#include "TestUtils.hpp"

namespace
{
const std::string_view TEST_DATA = R"(
    const float my_float_const = TRUE;

    struct array_str {
        float data[size];
    };

    struct my_str {
        long l;
        short x,y;
    };

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
        wchar long_var;

        boolean long_var;
        any long_var;
        octet long_var;

        Object long_var;

        string str;
        wstring wstr;
    };
)";

} // namespace

TEST(TestStruct, test)
{
    ASSERT_TRUE(utils::init("./test.data", TEST_DATA));

    std::ifstream stream;
    stream.open("./test.data", std::ios::in);
    ASSERT_TRUE(stream.good());

    ParserDriver p;
    EXPECT_TRUE(p.parse(stream));

    auto &scope = p.get_top_scope();
    EXPECT_EQ(4U, scope.size());

    utils::print_nodes(std::cout, &scope, 0);
}
