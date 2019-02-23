/*!
 * @class CGame
 * @brief Handles gameplay. Singleton.
 */

#ifndef CGAME_H
#define CGAME_H

#include <ncurses.h>
#include <memory>   
#include <vector>
#include <string>
#include <queue>

#include "CGameEntity.h"
#include "CEnemy.h"
#include "CTower.h"
#include "CTile.h"
#include "CDefinitions.h"

//Different game states
enum struct EGAME_STATE {
    IDLE, //Not doing anything, enemies moving, money slowly increasing
    PLACING_TOWER, //Picking a position to build our tower, enemies frozen
    ENDGAME,
    PAUSE
};

class CGame {
public:
    CGame(const CGame& orig) = delete; // Disallow copying
    CGame & operator=(const CGame &) = delete; // 
    ~CGame() = default;

    // Lookup
    CEnemy & NearestEnemy(int x, int y) const;
    CTower & NearestTower(int x, int y) const;

    /**
     * @brief If it exists, retrieves tile type at a given position, otherwise returs "void"
     * @param x X-position
     * @param y Y-position
     * @return  Tile type
     */
    CTile::ETILE_TYPE TileTypeAtPosition(int x, int y) const;

    /**
     * @brief Returns whether or not a tile at a given position can be walked on
     * @param x X-position
     * @param y Y-position
     * @return True if this tile can be walked on
     */
    bool TileWalkable(int x, int y) const;

    /**
     * @brief Calculates distance between two points.
     * @param x0
     * @param y0
     * @param x1
     * @param y1
     * @return Distance.
     */
    float Distance(int x0, int y0, int x1, int y1) const;

    /**
     * @brief Main game loop, called every frame from main.
     */
    void GameLoop(); //Called every frame, updates, then redraws everything

    /**
     * @brief Counts a kill and rewards the player
     * @param score Score to add
     */
    void CountKill(int score);

    /**
     * @brief Sets all tiles in a region where a given tower resides to ground
     * @param tower Tower
     */
    void TowerCleanup(const CTower &);

    /**
     * @brief Returns a pointer to an instance of CGame, if it doesn't exist, it will be created.
     * @param map Map id
     * @param save Whether or not we should look for a save file and load it
     * @return 
     */
    static CGame* GetInstance(int map, bool save = false);

    /**
     * @brief If a game instance exists, destroy it
     * @return If an instance has been destroyed
     */
    static bool Destroy();
    bool mQuit;

    /**
     * @brief Returns a read-only reference to entities vector.
     * @return Const reference to vector.
     */
    const std::vector<std::shared_ptr<CGameEntity>> &GetReadOnlyEntities() const;

    /**
     * @brief A* pathfinding from a given position to the exit with added variation
     * @param x Start x-position
     * @param y Start y-position
     * @param enemy Enemy whose path will be changed
     * @return If a path has been found
     */
    bool FindPathToExit(unsigned int x, unsigned int y, CEnemy* enemy = 0);

private:
    static const int MAP_SPAWN_DELAY = 50;
    static const int MAP_MAX_HEIGHT = 23;
    static const int MAP_MAX_WIDTH = 90;

    // Given the class hierarchy, this is appropriate
    friend class CTile;
    friend class CGameGraphics;

    static CGame* mSingInst; // Pointer to the one-and-only instance of the class

    /**
     * @brief Game constructor, called only by GetInstance
     * @param map Map definition ID
     * @param save Load a saved game?
     */
    CGame(int map, bool save);

    /**
     * @brief Save the game
     * @param filename Filename
     */
    void SaveGame(std::string filename) const;

    /**
     * @brief Loads a saved game
     */
    void LoadGame();

    /**
     * @brief Takes a map definition object and turns it into a playable map
     * @param map MapDefinition object
     * @param save Whether or not we should look for a save file and load it
     */
    void LoadMap(const CDefinitions::MapDefinition & map, bool save);

    /**
     * @brief Quits the current game, if the player hasn't lost/won yet, we save it
     */
    void Quit();

    /**
     * @brief Game input processing.
     */
    void ProcessInput();

    /**
     * @brief Destroys all entities marked to be destroyed
     */
    void DoEntityDestroy();

    /**
     * @brief Sets all tiles in a given region to a given type
     * @param x0 Start x-position
     * @param y0 Start y-position
     * @param x1 End x-position
     * @param y1 End y-position
     * @param type Tile type
     */
    void SetTileRegion(int x0, int y0, int x1, int y1, CTile::ETILE_TYPE type);

    /**
     * @brief Calculates Manhattan distance between two points.
     * @param x0
     * @param y0
     * @param x1
     * @param y1
     * @return Distance.
     */
    float ManhattanDistance(int x0, int y0, int x1, int y1) const; //used in pathfinding    

    /**
     * @brief Calculates the selling price of a tower
     * @param tower
     * @return Price
     */
    int TowerSellingPrice(const CTower & tower) const;

    /**
     * @brief Attempts to update enemy paths.
     * @return True if all paths were set, false if one or more paths couldn't be found
     */
    bool UpdateEnemyPaths();

    // Loading and error handling, used for map and save file loading

    class LoadException {
    public:
        std::string mErrorMessage;

        explicit LoadException(const std::string & message) : mErrorMessage(message) {
        };
    };

    enum class EMAP_LOAD_STATE {
        NONE,
        MAP_ATTRIBUTES,
        MAP_TILES,
        TOWER_DEFINITION,
        ENEMY_DEFINITION
    };

    enum class EGAME_LOAD_STATE {
        NONE,
        ENEMY,
        TOWER,
        STATS
    };
    //

    CTile mMapTiles [MAP_MAX_HEIGHT][MAP_MAX_WIDTH]; //(back layer of tiles, used for collision detection, building etc.)    
    std::vector<std::shared_ptr<CGameEntity>> mMapEntities; //front layer of entities

    EGAME_STATE mGameState;

    float mMoney;
    unsigned int mMapWidth, mMapHeight, mExitX, mExitY, mSpawnX, mSpawnY;
    unsigned int mEnemySequenceIndex, mEnemiesKilled, mEnemiesFled, mScore, mKillGoal, mSpawnTimer, mMapID; //Stats               
    int mSelectedTower;
    std::vector<int> mEnemySequence; //Stores enemy sequence (spawn order)
};

#endif /* CGAME_H */

