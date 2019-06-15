#include <gtest/gtest.h>

#include "ParserDriver.hpp"
#include "TestUtils.hpp"

namespace
{
const std::string_view TEST_DATA = R"(
    interface t1
    {

        long my_operation( in value_t p1 )
            raises ( com::app::Exception, com::app::MyException );

    };
)";

} // namespace

TEST(TestInterface, test)
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
