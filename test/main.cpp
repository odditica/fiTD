#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "CDefinitions.h"
#include "CGfx.h"
#include "CGameGraphics.h"
#include "CMainMenu.h"
#include "CGame.h"

TEST_CASE( "Definition loading.", "[definition") {
    REQUIRE_NOTHROW(CDefinitions::LoadDefinitions());
}

TEST_CASE( "test", "[test]" ) {
    REQUIRE(1 == 1 );    
}