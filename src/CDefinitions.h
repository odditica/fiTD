/*!
 * @class CDefinitions
 * @brief Handles map and entity definitions loaded from the definitions file at the start of the game. It behaves statically and cannot be instantiated.
 */

#ifndef CDEFINITIONS_H
#define CDEFINITIONS_H

#include <unordered_map>
#include <vector>
#include <string>

class CDefinitions {
public:

    // Different classes used to hold different type of data

    /*!
     * @class TowerDefinition
     * @brief Stores tower definition data, used as a template when creating tower entities.
     */
    class TowerDefinition {
    private:
        friend class CGame;
        friend class CGameGraphics;
        int mId, mPrice, mWidth, mHeight, mHealth, mRadius, mFrequency, mDamage, mColour;
        char mSymbol;
    public:

        TowerDefinition(int id, int price, int width, int height, int health, int radius, int frequency, int damage, int colour, char symbol) :
        mId(id), mPrice(price), mWidth(width), mHeight(height), mHealth(health), mRadius(radius), mFrequency(frequency), mDamage(damage), mColour(colour), mSymbol(symbol) {
        }
    };

    /*!
     * @class EnemyDefinition
     * @brief Stores enemy definition data, used as a template when creating enemy entities.
     */
    class EnemyDefinition {
    private:
        friend class CGame;
        friend class CGameGraphics;
        int mId, mHealth, mSpeed, mResist, mWStyle, mAttack, mColour;
        char mSymbol;
    public:

        EnemyDefinition(int id, int health, int speed, int resist, int wStyle, int attack, int colour, char symbol) :
        mId(id), mHealth(health), mSpeed(speed), mResist(resist), mWStyle(wStyle), mAttack(attack), mColour(colour), mSymbol(symbol) {
        }
    };

    /*!
     * @class MapDefinition
     * @brief Stores map definition data, used as a template when generating maps.
     */
    class MapDefinition {
    private:
        friend class CGame;
        unsigned int mId, mKillGoal;
        std::string mSequence;
        std::vector<std::string> mTiles;
    public:

        MapDefinition(int id, int killGoal, const std::string & sequence, const std::vector<std::string> & tiles) :
        mId(id), mKillGoal(killGoal), mSequence(sequence), mTiles(tiles) {
        }
    };
private:
    CDefinitions();

    //Collections of definitions, definition id used as key
    static std::unordered_map<int, TowerDefinition> mTowerDefinitions;
    static std::unordered_map<int, EnemyDefinition> mEnemyDefinitions;
    static std::unordered_map<int, MapDefinition> mMapDefinitions;

    enum class EDEF_LOAD_STATE {
        NONE,
        MAP_DEFINITION,
        TOWER_DEFINITION,
        ENEMY_DEFINITION
    };

public:

    // Error handling

    /*!
     * @class LoadException
     * @brief Map/game loading exception. Stores error message.
     */
    class LoadException {
    public:
        std::string mErrorMessage;

        explicit LoadException(const std::string & message) : mErrorMessage(message) {
        };
    };

    /**
     * @brief Adds a tower definition.
     * @param id Definition ID
     * @param price Price
     * @param width Width
     * @param height Height
     * @param health Health
     * @param radius Attack radius
     * @param frequency Attack frequency
     * @param damage Attack power
     * @param colour Colour
     * @param symbol Symbol
     */
    static void AddTowerDefinition(int id, int price, int width, int height, int health, int radius, int frequency, int damage, int colour, char symbol);

    /**
     * @brief Adds an enemy definition
     * @param id Definiton ID
     * @param health Health
     * @param speed Speed (delay between updates)
     * @param resist Definition ID of tower whose attack this enemy resists (0 reserved for none)
     * @param wStyle Field-crossing strategy 
     * @param attack Attack power
     * @param colour Colour
     * @param symbol Symbol
     */
    static void AddEnemyDefinition(int id, int health, int speed, int resist, int wStyle, int attack, int colour, char symbol);

    /**
     * @brief Adds a map definition
     * @param id Definition ID
     * @param killGoal Target number of enemies to kill
     * @param sequence Enemy sequence
     * @param tiles Map tiles (rows)
     */
    static void AddMapDefinition(int id, int killGoal, std::string sequence, std::vector<std::string> tiles);

    /**
     * @brief Retrieves a tower definition based on its definition ID
     * @return Tower definition
     */
    static const std::unordered_map<int, TowerDefinition> & GetTowerDefinitions();

    /**
     * @brief Retrieves all enemy definitions
     * @return Enemy definitions
     */
    static const std::unordered_map<int, EnemyDefinition> & GetEnemyDefinitions();

    /**
     * @brief Retrieves all map definitions
     * @return Map definitions
     */
    static const std::unordered_map<int, MapDefinition> & GetMapDefinitions();

    /**
     * @brief Retrieves a map definition based on its definition ID
     * @param id Definition ID
     * @return Map definition
     */
    static const MapDefinition & GetMapDefinition(int id);

    /**
     * @brief Retrieves an enemy definition based on its definition ID
     * @param id Definition ID
     * @return Enemy definition
     */
    static const EnemyDefinition & GetEnemyDefinition(int id);

    /**
     * @brief Retrieves a tower definition based on its definition ID
     * @param id Definition ID
     * @return Tower definition
     */
    static const TowerDefinition & GetTowerDefinition(int id);

    /**
     * @brief Retrieves a tower definition based on order in the definition collection
     * @param id Index
     * @return Tower definition
     */
    static const CDefinitions::TowerDefinition & GetTowerDefinitionByOrderIndex(int id);

    /**
     * @brief Loads map/entity/tower definitions.
     */
    static void LoadDefinitions();
};

#endif /* HDEFINITIONS_H */

