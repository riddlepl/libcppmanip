#include <cppmanip/DefaultFunctionLocator.hpp>
#include "ParsedFunction.hpp"
#include <cppmanip/SourceSelection.hpp>
#include <cppmanip/ExtractMethodError.hpp>
#include <gtest/gtest.h>
#include <memory>

namespace cppmanip
{

struct DefaultFunctionLocatorTest : testing::Test
{
    std::unique_ptr<test::ParsedFunction> func;
    void parse(const std::string& source)
    {
        func.reset(new test::ParsedFunction(source));
    }
    void assertFunctionContainsSelection(const std::string& name, SourceLocation from, SourceLocation to)
    {
        DefaultFunctionLocator locator(LocationRange(from, to));
        ASSERT_EQ(name, locator.getFunction(func->getASTContext()).getNameAsString())
            << "[" << from.row << ", " << from.col << "; " << to.row << ", " << to.col << ")";
    }
    void assertFailsForSelection(SourceLocation from, SourceLocation to)
    {
        DefaultFunctionLocator locator(LocationRange(from, to));
        ASSERT_THROW(locator.getFunction(func->getASTContext()), ExtractMethodError)
            << "[" << from.row << ", " << from.col << "; " << to.row << ", " << to.col << ")";
    }
};

TEST_F(DefaultFunctionLocatorTest, should_get_the_function_containing_given_selection)
{
    parse("void f()\n{\n  /*here*/\n}\n");
    ASSERT_NO_THROW(assertFunctionContainsSelection("f", rowCol(2, 0), rowCol(2, 8)));
    ASSERT_NO_THROW(assertFunctionContainsSelection("f", rowCol(2, 2), rowCol(2, 2)));
    ASSERT_NO_THROW(assertFunctionContainsSelection("f", rowCol(1, 0), rowCol(1, 1)));
    ASSERT_NO_THROW(assertFunctionContainsSelection("f", rowCol(3, 0), rowCol(3, 1)));
}

TEST_F(DefaultFunctionLocatorTest, should_fail_when_the_selection_is_not_overlapping_the_body)
{
    parse("void f()\n{\n /*...*/\n}\n");
    assertFailsForSelection(rowCol(0, 1), rowCol(0, 9));
    assertFailsForSelection(rowCol(3, 1), rowCol(4, 0));
}

TEST_F(DefaultFunctionLocatorTest, should_search_through_all_the_functions)
{
    parse("void f()\n{\n \n}\nvoid g()\n{\n /*here*/ \n}\nvoid h()\n{\n \n}\n");
    ASSERT_NO_THROW(assertFunctionContainsSelection("g", rowCol(6, 1), rowCol(6, 14)));
}

TEST_F(DefaultFunctionLocatorTest, should_ignore_functions_without_bodies)
{
    parse("void f(); void g(); void h() { }");
    ASSERT_NO_THROW(assertFunctionContainsSelection("h", rowCol(0, 30), rowCol(0, 30)));
}

}