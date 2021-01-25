#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "rayfork.h"

TEST_CASE( "rf_for_str_split", "[str]" )
{
    int i = 0;
    rf_for_str_split(str, rf_cstr("Hello World"), rf_cstr(" "))
    {
        if (i == 0)
        {
            bool result = rf_str_match(str, rf_cstr("Hello"));
            REQUIRE(result);
        }
        else if (i == 1)
        {
            bool result = rf_str_match(str, rf_cstr("World"));
            REQUIRE(result);
        }

        i++;
    }
}

TEST_CASE("rf_str_contains", "[str]")
{
    SECTION("\"Foo\" should contain \"o\"") {
        bool contains = rf_str_contains(rf_cstr("Foo"), rf_cstr("o"));
        REQUIRE(contains);
    }
    SECTION("\"Foo\" shouldn't contain \"a\"") {
        bool contains = rf_str_contains(rf_cstr("Foo"), rf_cstr("a"));
        REQUIRE(!contains);
    }
    SECTION("\"Foo\" should contain \"\"") {
        bool contains = rf_str_contains(rf_cstr("Foo"), rf_cstr(""));
        REQUIRE(contains);
    }
    SECTION("\"\" shouldn't contain \"a\"") {
        bool contains = rf_str_contains(rf_cstr(""), rf_cstr("a"));
        REQUIRE(!contains);
    }
    SECTION("\"\" should contain \"\"") {
        bool contains = rf_str_contains(rf_cstr(""), rf_cstr(""));
        REQUIRE(contains);
    }
}

TEST_CASE("rf_str_find_first", "[str]")
{
    SECTION("The first \"o\" should be on position 1 in \"Foo\"")
    {
        rf_int pos = rf_str_find_first(rf_cstr("Foo"), rf_cstr("o"));
        REQUIRE(pos == 1);
    }
    SECTION("\"a\" should be on an invalid position in \"Foo\"")
    {
        rf_int pos = rf_str_find_first(rf_cstr("Foo"), rf_cstr("a"));
        REQUIRE(pos == rf_invalid_index);
    }
    SECTION("The first \"\" should be on position 0 in \"Foo\"")
    {
        rf_int pos = rf_str_find_first(rf_cstr("Foo"), rf_cstr(""));
        REQUIRE(pos == 0);
    }
    SECTION("\"a\" should be on an invalid position in \"\"")
    {
        rf_int pos = rf_str_find_first(rf_cstr(""), rf_cstr("a"));
        REQUIRE(pos == rf_invalid_index);
    }
    SECTION("The first \"\" should be on position 0 in \"\"")
    {
        rf_int pos = rf_str_find_first(rf_cstr(""), rf_cstr(""));
        REQUIRE(pos == 0);
    }
    SECTION("The first \"bar\" should be on position 3 in \"Foobar\"")
    {
        rf_int pos = rf_str_find_first(rf_cstr("Foobar"), rf_cstr("bar"));
        REQUIRE(pos == 3);
    }
}

TEST_CASE("rf_str_find_last", "[str]")
{
    SECTION("The last \"o\" should be on position 2 in \"Foo\"")
    {
        rf_int pos = rf_str_find_last(rf_cstr("Foo"), rf_cstr("o"));
        REQUIRE(pos == 2);
    }
    SECTION("\"a\" should be on an invalid position in \"Foo\"")
    {
        rf_int pos = rf_str_find_last(rf_cstr("Foo"), rf_cstr("a"));
        REQUIRE(pos == rf_invalid_index);
    }
    SECTION("The last \"\" should be on position 3 in \"Foo\"")
    {
        rf_int pos = rf_str_find_last(rf_cstr("Foo"), rf_cstr(""));
        REQUIRE(pos == 3);
    }
    SECTION("\"a\" should be on an invalid position in \"\"")
    {
        rf_int pos = rf_str_find_last(rf_cstr(""), rf_cstr("a"));
        REQUIRE(pos == rf_invalid_index);
    }
    SECTION("The last \"\" should be on position 0 in \"\"")
    {
        rf_int pos = rf_str_find_last(rf_cstr(""), rf_cstr(""));
        REQUIRE(pos == rf_invalid_index);
    }
    SECTION("The last \"bar\" should be on position 6 in \"Foobarbar\"")
    {
        rf_int pos = rf_str_find_last(rf_cstr("Foobarbar"), rf_cstr("bar"));
        REQUIRE(pos == 6);
    }
}

TEST_CASE("rf_str_len", "[str]")
{
    SECTION("The length of \"Foo\" should be 3")
    {
        rf_int len = rf_str_len(rf_cstr("Foo"));
        REQUIRE(len == 3);
    }
    SECTION("The length of \"\" should be 0")
    {
        rf_int len = rf_str_len(rf_cstr(""));
        REQUIRE(len == 0);
    }
}

TEST_CASE("rf_str_match", "[str]")
{
    SECTION("\"Foo\" should match \"Foo\"")
    {
        bool match = rf_str_match(rf_cstr("Foo"), rf_cstr("Foo"));
        REQUIRE(match);
    }
    SECTION("\"\" should match \"\"")
    {
        bool match = rf_str_match(rf_cstr(""), rf_cstr(""));
        REQUIRE(match);
    }
    SECTION("\"Foo\" should not match \"\"")
    {
        bool match = rf_str_match(rf_cstr("Foo"), rf_cstr(""));
        REQUIRE(!match);
    }
    SECTION("\"Foo\" should not match \"foo\"")
    {
        bool match = rf_str_match(rf_cstr("Foo"), rf_cstr("foo"));
        REQUIRE(!match);
    }
}

TEST_CASE("rf_strbuf_append", "[strbuf]")
{
    SECTION("Appending \"Foo\" to an empty strbuf should become \"Foo\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf, rf_cstr("Foo"));
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foo")));
    }
    SECTION("Appending \"\" to an empty strbuf should become \"\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf, rf_cstr(""));
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("")));
    }
    SECTION("Appending \"Foo\" to \"\" should become \"Foo\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf, rf_cstr(""));
        rf_strbuf_append(&strbuf, rf_cstr("Foo"));
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foo")));
    }
    SECTION("Appending \"bar\" to \"Foo\" should become \"Foobar\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf, rf_cstr("Foo"));
        rf_strbuf_append(&strbuf, rf_cstr("bar"));
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foobar")));
    }
}

TEST_CASE("rf_strbuf_clone_ex", "[strbuf]")
{
    SECTION("Cloning \"Foo\" should stay \"Foo\"")
    {
        rf_strbuf strbuf_initial = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf_initial, rf_cstr("Foo"));
        rf_strbuf strbuf = rf_strbuf_clone_ex(strbuf_initial, rf_default_allocator);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foo")));
    }
    SECTION("Cloning an empty strbuf should be \"\"")
    {
        rf_strbuf strbuf_initial = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf strbuf = rf_strbuf_clone_ex(strbuf_initial, rf_default_allocator);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("")));
    }
}

TEST_CASE("rf_strbuf_insert_b", "[strbuf]")
{
    SECTION("Inserting \"Foo\" on position 0 in an empty string should become \"Foo\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_insert_b(&strbuf, rf_cstr("Foo"), 0);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foo")));
    }
    SECTION("Inserting \"bar\" on position 3 in \"Foo\" should become \"Foobar\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf, rf_cstr("Foo"));
        rf_strbuf_insert_b(&strbuf, rf_cstr("bar"), 3);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foobar")));
    }
    SECTION("Inserting \"\" on position 0 in an empty string should stay \"\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_insert_b(&strbuf, rf_cstr(""), 0);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("")));
    }
    // @Note(bg): This test crashes the test runner.
    /*
    SECTION("Inserting \"Foo\" on position 1 in an empty string should become \"Foo\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, RF_DEFAULT_ALLOCATOR);
        rf_strbuf_insert_b(&strbuf, rf_cstr("Foo"), 1);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foo")));
    }
    // @Note(bg): This test causes the other tests to not run.
    SECTION("Inserting \"bar\" on position 4 in \"Foo\" should become \"Foobar\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, RF_DEFAULT_ALLOCATOR);
        rf_strbuf_append(&strbuf, rf_cstr("Foo"));
        rf_strbuf_insert_b(&strbuf, rf_cstr("bar"), 4);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foobar")));
    }
    */
    SECTION("Inserting \"bar\" on position 2 in \"Foo\" should become \"Fobaro\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf, rf_cstr("Foo"));
        rf_strbuf_insert_b(&strbuf, rf_cstr("bar"), 2);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Fobaro")));
    }
    SECTION("Inserting \"\" on position 2 in \"Foo\" should stay \"Foo\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf, rf_cstr("Foo"));
        rf_strbuf_insert_b(&strbuf, rf_cstr(""), 2);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Foo")));
    }
    SECTION("Inserting \"bar\" on position -1 in \"Foo\" should become \"Fobaro\"")
    {
        rf_strbuf strbuf = rf_strbuf_make_ex(1024, rf_default_allocator);
        rf_strbuf_append(&strbuf, rf_cstr("Foo"));
        rf_strbuf_insert_b(&strbuf, rf_cstr("bar"), -1);
        REQUIRE(rf_str_match(rf_strbuf_to_str(strbuf), rf_cstr("Fobaro")));
    }
}
