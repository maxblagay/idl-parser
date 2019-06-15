#include <gtest/gtest.h>

#include "ParserDriver.hpp"
#include "TestUtils.hpp"

namespace
{
const std::string_view TEST_DATA = R"(
    const float my_float_const = TRUE % 12;

    const long my_float_const = TRUE + a / ( c % d) + ( ~d );
)";

} // namespace

TEST(TestExpr, test)
{
    ASSERT_TRUE(utils::init("./test.data", TEST_DATA));

    std::ifstream stream;
    stream.open("./test.data", std::ios::in);
    ASSERT_TRUE(stream.good());

    ParserDriver p;
    EXPECT_TRUE(p.parse(stream));

    auto &scope = p.get_top_scope();
    EXPECT_EQ(2U, scope.size());

    utils::print_nodes(std::cout, &scope, 0);
}
