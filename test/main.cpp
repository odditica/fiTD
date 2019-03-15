#define TEST_MODE
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <ncurses.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <deque>


#include "CDefinitions.h"
#include "CGfx.h"
#include "CGameGraphics.h"
#include "CMainMenu.h"
#include "CGame.h"

TEST_CASE("definition-loading") {

    SECTION("Loading") {
        REQUIRE_NOTHROW(CDefinitions::LoadDefinitions());
    }
    
    SECTION("Tower data check") {        
        auto tDef = CDefinitions::GetTowerDefinition(1);
        REQUIRE(tDef.mPrice == 600);
        REQUIRE(tDef.mSymbol == 'W');
        REQUIRE(tDef.mColour == 4);
        REQUIRE(tDef.mWidth == 4);
        REQUIRE(tDef.mHeight == 4);
        REQUIRE(tDef.mRadius == 5);
        REQUIRE(tDef.mFrequency == 25);
        REQUIRE(tDef.mDamage == 20);                            
    }
    
    SECTION("Enemy data check") {        
        auto eDef = CDefinitions::GetEnemyDefinition(0);
        REQUIRE(eDef.mSymbol == 'O');        
        REQUIRE(eDef.mColour == 0);
        REQUIRE(eDef.mHealth == 5);
        REQUIRE(eDef.mSpeed == 8);        
        REQUIRE(eDef.mWStyle == 1);
        REQUIRE(eDef.mResist == 0);        
        REQUIRE(eDef.mAttack == 10);                                        
    }

    SECTION("Map data check") {        
        auto mDef = CDefinitions::GetMapDefinition(0);
        REQUIRE(mDef.mId == 0);                                  
        REQUIRE(mDef.mKillGoal == 5);
        REQUIRE(!mDef.mSequence.std::string::compare("0,0,0,0,0"));             
        REQUIRE(!mDef.mTiles[0].std::string::compare(" #####"));
        REQUIRE(!mDef.mTiles[1].std::string::compare("##S.E##"));
        REQUIRE(!mDef.mTiles[2].std::string::compare(" #####"));                                                  
    }
    
}

TEST_CASE("game-initialisation") {
    CDefinitions::LoadDefinitions(); 
    CGfx::CursesSetup();         
    CGame* gamePtr;
    REQUIRE_NOTHROW(gamePtr = CGame::GetInstance(0, false));
    gamePtr->Destroy();
    endwin();
}

TEST_CASE("tile-property-check") {
    CDefinitions::LoadDefinitions(); 
    CGfx::CursesSetup();         
    CGame* gamePtr;
    gamePtr = CGame::GetInstance(0, false);

    SECTION("Walkable") { 
        REQUIRE(!gamePtr->TileWalkable(0, 0));
        REQUIRE(!gamePtr->TileWalkable(1, 0));
        REQUIRE(!gamePtr->TileWalkable(2, 1));
        REQUIRE(gamePtr->TileWalkable(3, 1));
        REQUIRE(gamePtr->TileWalkable(4, 1));
    }

    SECTION("Type") { 
        REQUIRE(gamePtr->TileTypeAtPosition(0, 1) == CTile::ETILE_TYPE::WALL);
        REQUIRE(gamePtr->TileTypeAtPosition(0, 0) == CTile::ETILE_TYPE::VOID);
        REQUIRE(gamePtr->TileTypeAtPosition(3, 1) == CTile::ETILE_TYPE::GROUND);
        REQUIRE(gamePtr->TileTypeAtPosition(4, 1) == CTile::ETILE_TYPE::EXIT);
    }
    
    gamePtr->Destroy();
    endwin();
}

TEST_CASE("save-load") {
    CDefinitions::LoadDefinitions(); 
    CGfx::CursesSetup();     
    CGame* gamePtr = CGame::GetInstance(0);
    gamePtr->GameLoop();
    REQUIRE_NOTHROW(gamePtr->Quit());
    gamePtr->Destroy();
    REQUIRE_NOTHROW(gamePtr = CGame::GetInstance(0, true));    
    REQUIRE_NOTHROW(gamePtr->Quit());
    gamePtr->Destroy();
    endwin();
}



