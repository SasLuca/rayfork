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
            rf_bool result = rf_str_match(str, rf_cstr("Hello"));
            REQUIRE(result);
        }
        else if (i == 1)
        {
            rf_bool result = rf_str_match(str, rf_cstr("World"));
            REQUIRE(result);
        }

        i++;
    }
}