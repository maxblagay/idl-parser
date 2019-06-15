#include <gtest/gtest.h>

#include "ParserDriver.hpp"
#include "TestUtils.hpp"

namespace
{
const std::string_view TEST_DATA = R"(
    union MyUnion switch (long long)
    {
    case 1:
        FistType firstType;
    default:
    case 2:
    case 4:
        SecondType secondType;
    };
)";

} // namespace

TEST(TestUnion, test)
{
    ASSERT_TRUE(utils::init("./test.data", TEST_DATA));

    std::ifstream stream;
    stream.open("./test.data", std::ios::in);
    ASSERT_TRUE(stream.good());

    ParserDriver p;
    EXPECT_TRUE(p.parse(stream));

    auto &scope = p.get_top_scope();
    EXPECT_EQ(1U, scope.size());

    utils::print_nodes(std::cout, &scope, 0);
}
