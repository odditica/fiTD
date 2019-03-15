#include <sstream>
#include <fstream>
#include <random>
#include <math.h>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "CGfx.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CGameGraphics.h"

CGame* CGame::mSingInst = 0;

CGame::CGame(int map, bool save = false):
mQuit(false), mGameState(EGAME_STATE::IDLE), mMoney(100), mMapWidth(0), mMapHeight(0), mExitX(0), mExitY(0), mSpawnX(0), mSpawnY(0),
mEnemySequenceIndex(0), mEnemiesKilled(0), mEnemiesFled(0), mScore(0), mKillGoal(0), mSpawnTimer(MAP_SPAWN_DELAY), mMapID(map), mSelectedTower(0) {
    mMapEntities.clear();
    mEnemySequence.clear();

    //Attempt to load a map (and optionally a save file)
    try {
        LoadMap(CDefinitions::GetMapDefinition(mMapID), save);        
        // Load a previously saved game
        if (save){
            LoadGame();
        }        
    } catch (const LoadException & m) {
        mQuit = true;        
        Destroy();
        throw m;
    }

    //Graphics setup
    CGameGraphics::Setup(this);
    
    //Tutorial
    CGameGraphics::ShowMessage("\\BWelcome!\\^B All important messages will be displayed here.");
    CGameGraphics::ShowMessage("Your goal in this game is to kill as many enemies as required.");
    CGameGraphics::ShowMessage("Use the \\BArrow Keys\\^B to move your cursor.");
    CGameGraphics::ShowMessage("To build a tower, press \\BW\\^B to enter \\Ubuild mode\\^U and choose the one you want using \\BQ\\^B and \\BE\\^B .");
    CGameGraphics::ShowMessage("While in \\Ubuild mode\\^U, find a good place for it and press \\BB\\^B again to build it. \\BSpace\\^B to cancel.");
    CGameGraphics::ShowMessage("You can also pause the game using \\BSpace\\^B and analyse enemies on the field by hovering the cursor over them.");    
}

void CGame::GameLoop() {
    // Shouldn't happen - precaution
    if (mSingInst == 0) return;
    
    // PROCESS INPUT
    ProcessInput();

    // Put everything in motion if we're not building/in a pause
    if (mGameState == EGAME_STATE::IDLE) {
        // Somehow make money automatically
        mMoney += .05;

        // ENTITY UPDATE
        bool enemiesGone = (mEnemySequenceIndex >= mEnemySequence.size());
        for (auto e : mMapEntities) {
            e->Update();
            if (enemiesGone) {
                if (typeid (*e) == typeid (CEnemy)) {
                    enemiesGone = false;
                }
            }
        }

        // Detect if the game should end
        if (enemiesGone) {
            CGameGraphics::ShowMessage("=================================");
            CGameGraphics::ShowMessage("");
            if (mEnemiesKilled >= mKillGoal) {
                CGameGraphics::ShowMessage("\\C6\\B\\bYou've won!");
                mGameState = EGAME_STATE::ENDGAME;
            } else {
                CGameGraphics::ShowMessage("\\C2\\B\\bYou've lost.");
                mGameState = EGAME_STATE::ENDGAME;
            }
            CGameGraphics::ShowMessage("Press escape to quit.");
        }

        // DESTROY ENTITIES THAT NEED TO BE DESTROYED
        DoEntityDestroy();

        mSpawnTimer--;
        if (mSpawnTimer == 0 && mEnemySequenceIndex < mEnemySequence.size()) {            
            auto enDef = CDefinitions::GetEnemyDefinition(mEnemySequence[mEnemySequenceIndex]);
            mMapEntities.push_back(std::make_shared<CEnemy>(CEnemy(this, enDef.mId, mSpawnX, mSpawnY, enDef.mSymbol, enDef.mColour, (float) enDef.mHealth, enDef.mSpeed, (CEnemy::E_WALK_STYLE)enDef.mWStyle, enDef.mAttack, enDef.mResist)));
            mEnemySequenceIndex++;
            mSpawnTimer = MAP_SPAWN_DELAY;
            if (mEnemySequenceIndex >= mEnemySequence.size()) {
                CGameGraphics::ShowMessage("\\C6\\BNo more enemies to spawn.");
            }
        }
    }

    // DRAW EVERYTHING
    CGameGraphics::Draw();
}

void CGame::LoadGame() {
    std::string filename = std::to_string(mMapID) + ".sav";
    std::ifstream saveFile;
    saveFile.open(filename, std::ios_base::in);
    EGAME_LOAD_STATE loadState = EGAME_LOAD_STATE::NONE;
    
    // If this file can be loaded, proceed
    if (saveFile.good()) {
        
        //Temp attributes
        int tempId = -1, tempX = -1, tempY = -1, tempTime = -1, tempHealth = -1, tempDir = -1;

        std::string line;
        
        //Retrieve a line
        while (getline(saveFile, line)) {
            
            //Section end handling
            if (loadState != EGAME_LOAD_STATE::NONE) {                
                if (line[0] == '-') {
                    if (loadState == EGAME_LOAD_STATE::TOWER || loadState == EGAME_LOAD_STATE::ENEMY) {
                        if ((tempId < 0
                                || tempX < 0
                                || tempY < 0
                                || tempTime < 0
                                || tempHealth < 0) || (loadState == EGAME_LOAD_STATE::ENEMY && tempDir < 0)) {
                            saveFile.close();
                            throw LoadException("Missing entity attributes.");
                        }
                        
                        //Create entities
                        if (loadState == EGAME_LOAD_STATE::TOWER) {
                            
                            //Tower                            
                            auto tDef = CDefinitions::GetTowerDefinition(tempId);
                            CTower tower = CTower(this, tempId, tempX, tempY, tDef.mSymbol, tDef.mColour, tDef.mWidth, tDef.mHeight, tempHealth, tDef.mRadius, tDef.mFrequency, tDef.mDamage);
                            tower.mActionTimer = tempTime;
                            mMapEntities.push_back(std::make_shared<CTower>(tower));    
                            SetTileRegion(tempX, tempY, tempX + tDef.mWidth, tempY + tDef.mHeight, CTile::ETILE_TYPE::TOWER);
                            
                        } else if (loadState == EGAME_LOAD_STATE::ENEMY) {
                            
                            //Enemy                            
                            auto eDef = CDefinitions::GetEnemyDefinition(tempId);
                            CEnemy enemy = CEnemy(this, tempId, tempX, tempY, eDef.mSymbol, eDef.mColour, tempHealth, eDef.mSpeed, (CEnemy::E_WALK_STYLE)eDef.mWStyle, eDef.mAttack, eDef.mResist);
                            enemy.mActionTimer = tempTime;
                            enemy.mLastDir = (CEnemy::EDIR)tempDir;                                    
                            mMapEntities.push_back(std::make_shared<CEnemy>(enemy));
                            
                        }                                                
                    }
                    loadState = EGAME_LOAD_STATE::NONE;
                }
            }

            std::stringstream lineStream(line);
            std::string key;
            std::string strValue;
            float numValue;

            //Section handling
            switch (loadState) {
                
                //Section type recognition
                case EGAME_LOAD_STATE::NONE:
                    if (line.length() < 2) continue;
                    if (line[0] == '@') {

                        lineStream >> key;
                        if (lineStream.fail()) {
                            saveFile.close();
                            throw (LoadException("Invalid values."));
                        }

                        std::string sectionTag = key.substr(1, key.length() - 1);

                        if (sectionTag == "GAME") {
                            loadState = EGAME_LOAD_STATE::STATS;
                        } else
                            if (sectionTag == "TOWER") {
                            //Reset temporary tower attributes
                            tempId = -1, tempX = -1, tempY = -1, tempTime = -1, tempHealth = -1;

                            loadState = EGAME_LOAD_STATE::TOWER;
                        } else
                            if (sectionTag == "ENEMY") {
                            //Reset temporary tower attributes
                            tempId = -1, tempX = -1, tempY = -1, tempTime = -1, tempHealth = -1;

                            loadState = EGAME_LOAD_STATE::ENEMY;
                        } else {
                            saveFile.close();
                            throw (LoadException("Unknown tag: " + sectionTag));
                        }
                    }
                break;

                //Entity section
                case EGAME_LOAD_STATE::TOWER: case EGAME_LOAD_STATE::ENEMY:
                    lineStream >> key;
                    lineStream >> numValue;
                    if (lineStream.fail()) {
                        saveFile.close();
                        throw (LoadException("Invalid entity values."));
                    }
                    if (key == "X") {
                        tempX = numValue;
                    } else
                        if (key == "Y") {
                        tempY = numValue;
                    } else
                        if (key == "HP") {
                        tempHealth = numValue;
                    } else
                        if (key == "TIME") {
                        tempTime = numValue;
                    } else
                        if (key == "ID") {
                        tempId = numValue;
                    } else
                        if (key == "DIR" && loadState == EGAME_LOAD_STATE::ENEMY) {
                        tempDir = numValue;
                    }
                break;

                //Game statistics section
                case EGAME_LOAD_STATE::STATS:
                    lineStream >> key;
                    lineStream >> numValue;
                    if (lineStream.fail()) {
                        saveFile.close();
                        throw (LoadException("Invalid statistics values."));
                    }
                    if (key == "SCORE") {
                        mScore = (unsigned int) numValue;
                    } else
                        if (key == "MONEY") {
                        mMoney = numValue;
                    } else
                        if (key == "SPNTIME") {
                        mSpawnTimer = (unsigned int) numValue;
                    } else
                        if (key == "SEQINDEX") {
                        mEnemySequenceIndex = (unsigned int) numValue;
                    } else
                        if (key == "FLED") {
                        mEnemiesFled = (int) numValue;
                    } else
                        if (key == "KILLED") {
                        mEnemiesKilled = (int) numValue;
                    }
                break;
            }
        }
        
        //Make sure all enemies have a correctly calculated path
        if (!UpdateEnemyPaths()){
            throw (LoadException("Enemies can't get to the exit!"));
        }

    } else {
        throw (LoadException("Could not load save file."));
    }
    saveFile.close();
}

void CGame::SaveGame(std::string filename) const{
    //Save the game
    std::ofstream saveFile;
    saveFile.open(filename, std::ios_base::out);
    if (saveFile.good()) {
        
        //Stats
        saveFile << "@GAME" << std::endl;
        saveFile << "\tSCORE" << " " << mScore << std::endl;
        saveFile << "\tMONEY" << " " << mMoney << std::endl;
        saveFile << "\tSPNTIME" << " " << mSpawnTimer << std::endl;
        saveFile << "\tSEQINDEX" << " " << mEnemySequenceIndex << std::endl;
        saveFile << "\tFLED" << " " << mEnemiesFled << std::endl;
        saveFile << "\tKILLED" << " " << mEnemiesKilled << std::endl;
        saveFile << "-" << std::endl;
        
        //"Print" every entity into the file stream
        for (auto e : mMapEntities) {
            saveFile << *e << std::flush;
        }
        saveFile.close();
    }    
}

CTile::ETILE_TYPE CGame::TileTypeAtPosition(int x, int y) const {    
    if ((unsigned int)x > mMapWidth || (unsigned int)y > mMapHeight){
        return CTile::ETILE_TYPE::VOID;
    }
    return mMapTiles[y][x].mType;
}

bool CGame::TileWalkable(int x, int y) const {
    if ((unsigned int)x > mMapWidth || (unsigned int)y > mMapHeight){
        return false;
    }
    return (mMapTiles[y][x].mType == CTile::ETILE_TYPE::GROUND || mMapTiles[y][x].mType == CTile::ETILE_TYPE::EXIT);
}

void CGame::LoadMap(const CDefinitions::MapDefinition & map, bool save) {
    bool exitSet = false, spawnSet = false;
    unsigned int mapY = 0;

    for (int y = 0; y < MAP_MAX_HEIGHT; ++y) {
        for (int x = 0; x < MAP_MAX_WIDTH; ++x) {
            mMapTiles[y][x].mX = x;
            mMapTiles[y][x].mY = y;
        }
    }

    for (auto row : map.mTiles) {
        //Load row of tiles

        for (unsigned int mapX = 0; mapX < row.length(); ++mapX) {

            if (mapX > MAP_MAX_WIDTH - 1 || mapY > MAP_MAX_HEIGHT - 1) {
                throw (LoadException("Map is too big."));
            }

            switch (row[mapX]) {
                case '#':
                    mMapTiles[mapY][mapX].mType = CTile::ETILE_TYPE::WALL;
                    mMapTiles[mapY][mapX].mSymbol = ' ';
                    //mMapTiles[mapY][mapX].mColour = COLOR_PAIR(ECOLORS::);
                    //if ((mapX + mapY % 2) % 2 == 0){                        
                    mMapTiles[mapY][mapX].mColour = COLOR_PAIR(ECOLORS::BLACK_ON_WHITE);
                    //}
                    break;

                case '.':
                    mMapTiles[mapY][mapX].mType = CTile::ETILE_TYPE::GROUND;
                    mMapTiles[mapY][mapX].mSymbol = '.';
                    mMapTiles[mapY][mapX].mColour = COLOR_PAIR(ECOLORS::WHITE) | A_DIM;

                    break;

                case 'E':
                    if (exitSet) {
                        throw (LoadException("Duplicate exit point."));
                    }
                    mMapTiles[mapY][mapX].mType = CTile::ETILE_TYPE::EXIT;
                    mMapTiles[mapY][mapX].mSymbol = '@';
                    mMapTiles[mapY][mapX].mColour = COLOR_PAIR(ECOLORS::RED) | A_BLINK | A_BOLD;
                    exitSet = true;
                    mExitY = mapY;
                    mExitX = mapX;

                    break;

                case 'S':
                    if (spawnSet) {
                        throw (LoadException("Duplicate spawn point."));
                    }
                    mMapTiles[mapY][mapX].mType = CTile::ETILE_TYPE::VOID;
                    spawnSet = true;
                    mSpawnY = mapY;
                    mSpawnX = mapX;

                    break;
            }
            //Increase boundary
            if ((unsigned) mapX > mMapWidth) mMapWidth = mapX;
        }

        //Increase boundary      
        if (mapY > mMapHeight) mMapHeight = mapY;

        //Next row
        mapY++;
    }

    if (!spawnSet || !exitSet) {
        throw (LoadException("Missing spawn or exit."));
    }

    if (!FindPathToExit(mSpawnX, mSpawnY)) {
        throw (LoadException("Exit point can't be reached!"));
    }

    //Disassemble enemy sequence
    auto eDef = CDefinitions::GetEnemyDefinitions();
    int resId;
    std::string buff = "";

    for (unsigned int i = 0; i < map.mSequence.length(); ++i) {
        char ch = map.mSequence[i];
        if (ch != ',') {
            buff += ch;
        } else {
            if (buff != "") {
                try {
                    resId = stoi(buff);
                } catch (std::invalid_argument) {
                    throw (LoadException("Enemy sequence parsing error."));
                }
                mEnemySequence.push_back(resId);
            }
            buff = "";
        }
    }

    if (buff != "") {
        resId = stoi(buff);
        mEnemySequence.push_back(resId);
        buff = "";
    }

    // Shouldn't ever occur
    if (mEnemySequence.size() == 0) {
        throw (LoadException("Empty enemy sequence."));
    }

    // Check each ID against definitions
    for (auto x : mEnemySequence) {
        if (eDef.find(x) == eDef.end()) {
            throw (LoadException("Non-existent enemy ID found in sequence: " + std::to_string(x)));
        }
    }

    if (map.mKillGoal > mEnemySequence.size()) {
        throw (LoadException("Impossible kill target."));
    }

    mKillGoal = map.mKillGoal;
}

CGame* CGame::GetInstance(int map, bool save) {
    if (!mSingInst) {
        mSingInst = new CGame(map, save);
        return mSingInst;
    } else {
        return mSingInst;
    }
}

bool CGame::Destroy() {
    if (mSingInst) {
        delete mSingInst;
        mSingInst = 0;
        return true;
    }
    return false;
}

void CGame::DoEntityDestroy() {
    for (auto it = mMapEntities.begin(); it != mMapEntities.end();) {
        auto a = **it;
        if (a.mDestroyed) {
            (*it).reset();
            mMapEntities.erase(it);
        } else {
            ++it;
        }
    }
}

void CGame::Quit() {
    std::string filename = std::to_string(mMapID) + ".sav";
    if (mGameState != EGAME_STATE::ENDGAME) {
        SaveGame(filename);
    } else {
        //Delete existing save file
        std::remove(filename.c_str());
    }
    mQuit = true;
}

const std::vector<std::shared_ptr<CGameEntity>> &CGame::GetReadOnlyEntities() const {
    return mMapEntities;
}

bool CGame::FindPathToExit(unsigned int x, unsigned int y, CEnemy* enemy) {       
    //F-score comparator
    struct CompareFScoreLowest {
        int operator()(CTile* a, CTile* b) {
            return ((float) a->mFScore < (float) b->mFScore);
        }
    };

    //Reset all nodes
    for (unsigned int y = 0; y < mMapHeight; ++y) {
        for (unsigned int x = 0; x < mMapWidth; ++x) {
            mMapTiles[y][x].mCameFrom = 0;
            mMapTiles[y][x].mFScore = 0;
            mMapTiles[y][x].mGScore = 0;
        }
    }
    
    //Collections
    std::multiset<CTile*, CompareFScoreLowest> openSet;
    std::unordered_set<CTile*> closedSet;
    
    //Calculate F-score of starting node and insert it into the open set
    mMapTiles[y][x].mFScore = ManhattanDistance(mExitX, mExitY, x, y);   
    openSet.insert(&(mMapTiles[y][x]));

    //4 tile neighbours max, no need to do this with a dynamically resized collection
    CTile * neighbours[4];

    //Anything left in the open set?
    while (openSet.size() > 0) {
        
        //Retrieve node with lower F-score
        CTile* current = *(openSet.cbegin());
        openSet.erase(openSet.cbegin());
        closedSet.emplace(current);

        // Are we at the exit?
        if (current->mX == mExitX && current->mY == mExitY) {
            // If we're changing an enemy's path, reconstruct it and set it
            if (enemy) {
                if (enemy->mWalkStyle == CEnemy::E_WALK_STYLE::A_STAR) {
                    std::stack<CTile*> empty;
                    std::swap(enemy->mWalkPath, empty);
                    enemy->mWalkPath.push(current);
                    while (current->mCameFrom) {
                        current = current->mCameFrom;
                        enemy->mWalkPath.push(current);
                    }
                }
            }
            return true;
        }

        //Obstacle detection
        neighbours[0] = 0;
        neighbours[1] = 0;
        neighbours[2] = 0;
        neighbours[3] = 0;
        if (TileWalkable(current->mX + 1, current->mY)) neighbours[0] = &mMapTiles[current->mY ][current->mX + 1];
        if (TileWalkable(current->mX, current->mY - 1)) neighbours[1] = &mMapTiles[current->mY - 1][current->mX ];
        if (TileWalkable(current->mX - 1, current->mY)) neighbours[2] = &mMapTiles[current->mY ][current->mX - 1];
        if (TileWalkable(current->mX, current->mY + 1)) neighbours[3] = &mMapTiles[current->mY + 1][current->mX ];

        //Process each neighbour
        for (unsigned int i = 0; i < 4; ++i) {
            if (neighbours[i] != 0) {
                
                //Don't continue if we have already searched this node
                if (closedSet.find(neighbours[i]) != closedSet.end()) {
                    continue;
                }
                
                //Calculate new G-score
                float tGScore = current->mGScore + 1;
                bool willInsert = false;
                
                if (openSet.find(neighbours[i]) == openSet.end()) {
                    
                    //Newly discovered node
                    neighbours[i]->mGScore = tGScore;
                    willInsert = true;
                    
                } else {
                    
                    //If this node is better, re-insert it into the open set (due to ordering)
                    if (tGScore < neighbours[i]->mGScore) {
                        openSet.erase(neighbours[i]); //Re-insert to keep ordering
                        neighbours[i]->mGScore = tGScore;
                        willInsert = true;
                    }
                    
                }

                neighbours[i]->mCameFrom = current;

                // Adding a random value to F score to add some variation to the final path
                float rnd = (float) (std::rand() % 100 - 50) / 100;
                neighbours[i]->mFScore = neighbours[i]->mGScore + ManhattanDistance(mExitX, mExitY, neighbours[i]->mX, neighbours[i]->mY) + rnd;
                
                // Insert into the open set
                if (willInsert) openSet.insert(neighbours[i]);
            }
        }
    }

    // No solution
    return false;
}

bool CGame::UpdateEnemyPaths() {
    for (auto ent : mMapEntities) {
        if (typeid (*ent) == typeid (CEnemy)) {
            auto eP = std::dynamic_pointer_cast<CEnemy>(ent);
            if (!FindPathToExit(eP->mX, eP->mY, &(*eP))) {
                return false;
            }
        }
    }
    return true;
}

int CGame::TowerSellingPrice(const CTower& tower) const {
    auto tDef = CDefinitions::GetTowerDefinition(tower.mSpecId);
    return (int) (tDef.mPrice * ((float) (tower.mHealth / tDef.mHealth)));
}

void CGame::SetTileRegion(int x0, int y0, int x1, int y1, CTile::ETILE_TYPE type){    
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            mMapTiles[y][x].mType = type;
        }
    }
}

void CGame::ProcessInput() {      
    //If needed, block all input for a few frames after resizing
    if (CGfx::justResized >= 0){        
        if (CGfx::justResized == 0){
            keypad(CGameGraphics::winMainField, true);
        } else {
            keypad(CGameGraphics::winMainField, false);            
        }
        CGfx::justResized --;
        return;
    }
    
    switch (wgetch(CGameGraphics::winMainField)) {
        case KEY_DOWN:
            CGameGraphics::cursorY++;
            break;
        case KEY_UP:
            CGameGraphics::cursorY--;
            break;
        case KEY_LEFT:
            CGameGraphics::cursorX--;
            break;
        case KEY_RIGHT:
            CGameGraphics::cursorX++;
            break;
        case KEY_SLEFT:
            CGameGraphics::cursorX -= 2;
            break;
        case KEY_SRIGHT:
            CGameGraphics::cursorX += 2;
            break;

        case ' ':
            // Pausing
            if (mGameState == EGAME_STATE::IDLE) {
                mGameState = EGAME_STATE::PAUSE;
            } else if (mGameState == EGAME_STATE::PAUSE) {
                mGameState = EGAME_STATE::IDLE;
            } else if (mGameState == EGAME_STATE::PLACING_TOWER) {
                mGameState = EGAME_STATE::IDLE;
            }
            break;

        case 'w': case 'W':
            // Building
            if (mGameState == EGAME_STATE::IDLE) {
                mGameState = EGAME_STATE::PLACING_TOWER;
            } else {
                if (mGameState == EGAME_STATE::PLACING_TOWER) {
                    
                    //Buy & build tower
                    auto t = CDefinitions::GetTowerDefinitionByOrderIndex(mSelectedTower);

                    if (mMoney < t.mPrice) {
                        CGameGraphics::ShowMessage("\\B\\C2You can't afford this tower!");
                        break;
                    }

                    int xOffset = 0, yOffset = 0;
                    if (t.mWidth % 2 != 0) xOffset = t.mWidth / 2;
                    if (t.mHeight % 2 != 0) yOffset = t.mHeight / 2;
                    int x0 = CGameGraphics::cursorX - xOffset;
                    int x1 = CGameGraphics::cursorX - xOffset + t.mWidth;
                    int y0 = CGameGraphics::cursorY - yOffset;
                    int y1 = CGameGraphics::cursorY - yOffset + t.mHeight;
                    bool spaceFree = true;
                                        
                    //Check if all tiles in the given region are ground
                    for (int y = y0; y < y1; ++y) {
                        for (int x = x0; x < x1; ++x) {                            
                            if (mMapTiles[y][x].mType != CTile::ETILE_TYPE::GROUND) {
                                spaceFree = false;
                                break;
                            }
                        }
                    }
                                                             
                    // Search for potential overlaps with entities
                    if (spaceFree) {
                        for (auto ent : mMapEntities) {
                            if (ent->OverlapRect(x0, y0, t.mWidth, t.mHeight)) {
                                spaceFree = false;
                                break;
                            }
                        }
                    }
                    
                    if (!spaceFree) {
                        CGameGraphics::ShowMessage("\\B\\C2You can't build there!");
                        break;
                    }
                    
                    //Set the type of tiles in the tower region to tower, if we can't build there, this will be reverted
                    SetTileRegion(x0, y0, x1, y1, CTile::ETILE_TYPE::TOWER);

                    //Check clear path from start to finish 
                    if (spaceFree) {
                        if (!FindPathToExit(mSpawnX, mSpawnY)) {
                            spaceFree = false;
                        }
                    }

                    //Make sure all enemies can still get to the exit, update their paths if they can.
                    if (spaceFree) {
                        spaceFree = UpdateEnemyPaths();
                    }

                    if (!spaceFree) {
                        //Undo changes
                        for (int y = y0; y < y1; ++y) {
                            for (int x = x0; x < x1; ++x) {
                                mMapTiles[y][x].mType = CTile::ETILE_TYPE::GROUND;
                            }
                        }
                        SetTileRegion(x0, y0, x1, y1, CTile::ETILE_TYPE::GROUND);
                        CGameGraphics::ShowMessage("\\B\\C2You can't block enemies' path!");
                        break;
                    }

                    //Buy and create tower
                    mMapEntities.push_back(std::make_shared<CTower>(CTower(this, t.mId, x0, y0, t.mSymbol, t.mColour, t.mWidth, t.mHeight, t.mHealth, t.mRadius, t.mFrequency, t.mDamage)));
                    mMoney -= t.mPrice;
                    CGameGraphics::ShowMessage("\\B\\C6Tower built.");
                    mGameState = EGAME_STATE::IDLE;
                }
            }
            break;

        case 's': case 'S':
            //Sell tower
            if (mGameState == EGAME_STATE::IDLE) {
                //Determine which tower the cursor is hovered over
                for (auto e : mMapEntities) {
                    if (typeid (*e) == typeid (CTower)) {
                        auto eP = std::dynamic_pointer_cast<CTower>(e);
                        if (eP->OverlapPoint(CGameGraphics::cursorX, CGameGraphics::cursorY)) {
                            //Sell it
                            mMoney += TowerSellingPrice(*eP);
                            eP->Destroy();
                        }
                    }
                }
            }
            break;

        case 'Q': case 'q':
            //Previous tower
            if (mGameState == EGAME_STATE::PLACING_TOWER) {
                int len = CDefinitions::GetTowerDefinitions().size();
                mSelectedTower = (((mSelectedTower - 1) % len) + len) % len;
            }
            break;

        case 'E': case 'e':
            //Next tower
            if (mGameState == EGAME_STATE::PLACING_TOWER) {
                int len = CDefinitions::GetTowerDefinitions().size();
                mSelectedTower = (((mSelectedTower + 1) % len) + len) % len;
            }
            break;

        case 27:         
            //Escape detection
            Quit();
            return;
            break;
    }

    // Lock cursor position
    CGameGraphics::cursorX = (int) fmin(fmax(0, CGameGraphics::cursorX), mMapWidth);
    CGameGraphics::cursorY = (int) fmin(fmax(0, CGameGraphics::cursorY), mMapHeight);
}

void CGame::CountKill(int score) {
    int worth = score / 2.5;
    mScore += score;
    mMoney += worth;
    mEnemiesKilled++;
    std::stringstream ss;
    ss << "\\B\\C8Enemy killed for \\U" << score << "\\^U points and \\U$" << worth << "\\^U!" << std::flush;
    CGameGraphics::ShowMessage(ss.str());
    if (mEnemiesKilled == mKillGoal) {
        CGameGraphics::ShowMessage("\\B\\C8\\b!!\\^b Goal cleared! \\b!!\\^b");
    }
}

void CGame::TowerCleanup(const CTower & tower) {
    for (int y = 0; y < tower.mHeight; ++y) {
        for (int x = 0; x < tower.mWidth; ++x) {
            if (TileTypeAtPosition(x + tower.mX, y + tower.mY) == CTile::ETILE_TYPE::TOWER) {
                mMapTiles[y + tower.mY][x + tower.mX].mType = CTile::ETILE_TYPE::GROUND;
                mMapTiles[y + tower.mY][x + tower.mX].mSymbol = 'O';

            }
        }
    }
}

float CGame::ManhattanDistance(int x0, int y0, int x1, int y1) const {
    return abs(x0 - x1) + abs(y0 - y1);
}

float CGame::Distance(int x0, int y0, int x1, int y1) const {
    return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}
