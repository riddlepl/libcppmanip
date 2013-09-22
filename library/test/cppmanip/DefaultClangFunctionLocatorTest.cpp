#include <cppmanip/DefaultClangFunctionLocator.hpp>
#include "ParsedFunction.hpp"
#include <cppmanip/SourceSelection.hpp>
#include <cppmanip/ExtractMethodError.hpp>
#include <gtest/gtest.h>
#include <memory>

namespace cppmanip
{

struct DefaultClangFunctionLocatorTest : testing::Test
{
    std::unique_ptr<test::ParsedFunction> func;
    void parse(const std::string& source)
    {
        func.reset(new test::ParsedFunction(source));
    }
    void assertFunctionContainsSelection(const std::string& name, unsigned from, unsigned to)
    {
        DefaultClangFunctionLocator locator(OffsetRange(from, to));
        ASSERT_EQ(name, locator.getFunction(func->getASTContext()).getNameAsString())
            << "[" << from << ", " << to << ")";
    }
    void assertFailsForSelection(unsigned from, unsigned to)
    {
        DefaultClangFunctionLocator locator(OffsetRange(from, to));
        ASSERT_THROW(locator.getFunction(func->getASTContext()), ExtractMethodError)
            << "[" << from << ", " << to << ")";
    }
};

TEST_F(DefaultClangFunctionLocatorTest, should_get_the_function_containing_given_selection)
{
    parse("void f()\n{\n  /*here*/\n}\n");
    ASSERT_NO_THROW(assertFunctionContainsSelection("f", 11, 19));
    ASSERT_NO_THROW(assertFunctionContainsSelection("f", 13, 13));
    ASSERT_NO_THROW(assertFunctionContainsSelection("f", 9, 10));
    ASSERT_NO_THROW(assertFunctionContainsSelection("f", 22, 23));
}

TEST_F(DefaultClangFunctionLocatorTest, should_fail_when_the_selection_is_not_overlapping_the_body)
{
    parse("void f()\n{\n /*...*/\n}\n");
    assertFailsForSelection(1, 9);
    assertFailsForSelection(21, 22);
}

TEST_F(DefaultClangFunctionLocatorTest, should_search_through_all_the_functions)
{
    parse("void f()\n{\n \n}\nvoid g()\n{\n /*here*/ \n}\nvoid h()\n{\n \n}\n");
    ASSERT_NO_THROW(assertFunctionContainsSelection("g", 27, 35));
}

TEST_F(DefaultClangFunctionLocatorTest, should_ignore_functions_without_bodies)
{
    parse("void f(); void g(); void h() { }");
    ASSERT_NO_THROW(assertFunctionContainsSelection("h", 30, 30));
}

}