#include "CDefinitions.h"
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

void CDefinitions::AddTowerDefinition(int id, int price, int width, int height, int health, int radius, int frequency, int damage, int colour, char symbol) {
    mTowerDefinitions.insert(std::make_pair(id, TowerDefinition(id, price, width, height, health, radius, frequency, damage, colour, symbol)));
}

void CDefinitions::AddEnemyDefinition(int id, int health, int speed, int resist, int wStyle, int attack, int colour, char symbol) {
    mEnemyDefinitions.insert(std::make_pair(id, EnemyDefinition(id, health, speed, resist, wStyle, attack, colour, symbol)));
}

void CDefinitions::AddMapDefinition(int id, int killGoal, std::string sequence, std::vector<std::string> tiles) {
    mMapDefinitions.insert(std::make_pair(id, MapDefinition(id, killGoal, sequence, tiles)));
}

const std::unordered_map<int, CDefinitions::TowerDefinition> & CDefinitions::GetTowerDefinitions() {
    return mTowerDefinitions;
}

const CDefinitions::TowerDefinition & CDefinitions::GetTowerDefinitionByOrderIndex(int id) {
    int i = 0;

    for (auto it : mTowerDefinitions) {
        if (i == id) {
            return mTowerDefinitions.at(it.first);
        }
        ++i;
    }
    return (*mTowerDefinitions.begin()).second;
}

const std::unordered_map<int, CDefinitions::EnemyDefinition> & CDefinitions::GetEnemyDefinitions() {
    return mEnemyDefinitions;
}

const std::unordered_map<int, CDefinitions::MapDefinition> & CDefinitions::GetMapDefinitions() {
    return mMapDefinitions;
}

const CDefinitions::MapDefinition & CDefinitions::GetMapDefinition(int id) {
    return mMapDefinitions.at(id);
}

const CDefinitions::EnemyDefinition & CDefinitions::GetEnemyDefinition(int id) {
    return mEnemyDefinitions.at(id);
}

const CDefinitions::TowerDefinition & CDefinitions::GetTowerDefinition(int id) {
    return mTowerDefinitions.at(id);
}

void CDefinitions::LoadDefinitions() {
    EDEF_LOAD_STATE loadState = EDEF_LOAD_STATE::NONE;
    bool towersSet = false;
    bool enemiesSet = false;
    bool mapsSet = false;

    std::vector<std::string> mapTiles;
    std::string mapSequence = "";

    std::ifstream defFile;
    defFile.open("data/definitions.cnf", std::ios_base::in);

    if (!defFile.good()) {
        defFile.close();
        throw LoadException("Game data file could not be opened.");
    } else {

        //Tower template
        int tId = -1, tPrice = -1, tWidth = -1, tHeight = -1, tColour = -1, tHealth = -1, tRadius = -1, tFrequency = -1, tDamage = -1;
        char tSymbol = '\0';
        //
        //Enemy template
        int eId = -1, eColour = -1, eSpeed = -1, eHealth = -1, eWStyle = -1, eResist = -1, eAttack = -1;
        char eSymbol = '\0';
        //
        //Map template
        int mapId = -1, mapKillGoal = -1;

        //Read line by line
        std::string line;
        while (getline(defFile, line)) {
            
            //Remove newline
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

            //Section end handling
            if (loadState != EDEF_LOAD_STATE::NONE) {
                if (line[0] == '-') {
                    if (loadState == EDEF_LOAD_STATE::TOWER_DEFINITION) {
                        //Insert tower definition     
                        towersSet = true;
                        if (tId < 0
                                || tPrice < 0
                                || tWidth < 0
                                || tHeight < 0
                                || tRadius < 0
                                || tFrequency < 0
                                || tDamage < 0
                                || tColour < 0
                                || tSymbol == '\0') {
                            defFile.close();
                            throw LoadException("Missing tower attributes.");
                        }
                        AddTowerDefinition(tId, tPrice, tWidth, tHeight, tHealth, tRadius, tFrequency, tDamage, tColour, tSymbol);
                    } else
                        if (loadState == EDEF_LOAD_STATE::ENEMY_DEFINITION) {
                        enemiesSet = true;
                        if (eId < 0
                                || eAttack < 0
                                || eColour < 0
                                || eHealth < 0
                                || eResist < 0
                                || eWStyle < 0
                                || eSpeed < 0
                                || eSymbol == '\0') {
                            defFile.close();
                            throw LoadException("Missing enemy attributes.");
                        }
                        AddEnemyDefinition(eId, eHealth, eSpeed, eResist, eWStyle, eAttack, eColour, eSymbol);
                    } else
                        if (loadState == EDEF_LOAD_STATE::MAP_DEFINITION) {
                        mapsSet = true;
                        if (mapId < 0
                                || mapKillGoal < 0
                                || mapSequence == ""
                                || mapTiles.size() == 0) {
                            defFile.close();
                            throw LoadException("Missing map attributes.");
                        }
                        AddMapDefinition(mapId, mapKillGoal, mapSequence, mapTiles);
                    }

                    loadState = EDEF_LOAD_STATE::NONE;
                    continue;
                }
            }

            std::stringstream lineStream(line);
            std::string key;
            std::string strValue;
            int intValue;

            //Key-value extractions from sections
            switch (loadState) {

                    //Section identifier handling
                case EDEF_LOAD_STATE::NONE:
                    if (line.length() < 2) continue;
                    if (line[0] == '@') {

                        lineStream >> key;
                        if (lineStream.fail()) {
                            defFile.close();
                            throw (LoadException("Invalid values."));
                        }

                        std::string sectionTag = key.substr(1, key.length() - 1);

                        if (sectionTag == "MAP") {
                            mapId = -1, mapKillGoal = -1;
                            mapTiles.clear();
                            mapSequence = "";
                            loadState = EDEF_LOAD_STATE::MAP_DEFINITION;
                        } else
                            if (sectionTag == "TOWER") {
                            //Reset temporary tower attributes
                            tId = -1, tPrice = -1, tWidth = -1, tHeight = -1, tColour = -1, tHealth = -1, tRadius = -1, tFrequency = -1, tDamage = -1;
                            tSymbol = '\0';
                            loadState = EDEF_LOAD_STATE::TOWER_DEFINITION;
                        } else
                            if (sectionTag == "ENEMY") {
                            //Reset temporary enemy attributes
                            eId = -1, eColour = -1, eSpeed = -1, eHealth = -1, eWStyle = -1, eResist = -1, eAttack = -1;
                            eSymbol = '\0';
                            loadState = EDEF_LOAD_STATE::ENEMY_DEFINITION;
                        } else {
                            defFile.close();
                            throw (LoadException("Unknown tag: " + sectionTag));
                        }
                    }
                    break;

                    //Tower definition section
                case EDEF_LOAD_STATE::TOWER_DEFINITION:
                    lineStream >> key;

                    if (key != "SYMBOL") {
                        lineStream >> intValue;
                    } else {
                        lineStream >> strValue;
                    }
                    if (lineStream.fail()) {
                        defFile.close();
                        throw (LoadException("Invalid tower values."));
                    }

                    if (key == "ID") {
                        tId = intValue;
                    } else
                        if (key == "PRICE") {
                        tPrice = intValue;
                    } else
                        if (key == "WIDTH") {
                        tWidth = intValue;
                    } else
                        if (key == "HEIGHT") {
                        tHeight = intValue;
                    } else
                        if (key == "COLOUR") {
                        tColour = intValue;
                    } else
                        if (key == "HEALTH") {
                        tHealth = intValue;
                    } else
                        if (key == "RADIUS") {
                        tRadius = intValue;
                    } else
                        if (key == "ADELAY") {
                        tFrequency = intValue;
                    } else
                        if (key == "DAMAGE") {
                        tDamage = intValue;
                    } else
                        if (key == "SYMBOL") {
                        tSymbol = strValue[0];
                    } else {
                        defFile.close();
                        throw (LoadException("Missing/invalid value for tower key " + key));
                    }

                    break;

                    //Enemy definition section
                case EDEF_LOAD_STATE::ENEMY_DEFINITION:
                    lineStream >> key;

                    if (key != "SYMBOL") {
                        lineStream >> intValue;
                    } else {
                        lineStream >> strValue;
                    }
                    if (lineStream.fail()) {
                        defFile.close();
                        throw (LoadException("Missing/invalid value for enemy key " + key));
                    }

                    if (key == "ID") {
                        eId = intValue;
                    } else
                        if (key == "HEALTH") {
                        eHealth = intValue;
                    } else
                        if (key == "WDELAY") {
                        eSpeed = intValue;
                    } else
                        if (key == "RESIST") {
                        eResist = intValue;
                    } else
                        if (key == "WSTYLE") {
                        eWStyle = intValue;
                    } else
                        if (key == "COLOUR") {
                        eColour = intValue;
                    } else
                        if (key == "SYMBOL") {
                        eSymbol = strValue[0];
                    } else
                        if (key == "ATTACK") {
                        eAttack = intValue;
                    } else {
                        defFile.close();
                        throw (LoadException("Unknown enemy attribute: " + key));
                    }
                    break;

                    //Map definition section
                case EDEF_LOAD_STATE::MAP_DEFINITION:
                    lineStream >> key;
                    if (key != "TILES" && key != "ENEMYSEQ") {
                        lineStream >> intValue;
                    } else {
                        if (key == "ENEMYSEQ") {
                            lineStream >> strValue;
                        }
                    }

                    if (key != "TILES" && lineStream.fail()) {
                        defFile.close();
                        throw (LoadException("Invalid map values."));
                    }

                    if (key == "ID") {
                        mapId = intValue;
                    } else
                        if (key == "ENEMYSEQ") {
                        mapSequence = strValue;
                    } else
                        if (key == "KILLGOAL") {
                        mapKillGoal = intValue;
                    } else
                        if (key == "TILES") {
                        while (true) {
                            getline(defFile, line);
                            if (line[0] == '-') {
                                break;
                            }
                            if (defFile.eof()) {
                                defFile.close();
                                throw (LoadException("Unexpected end of file."));
                            }

                            //Remove newline
                            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
                            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
                            
                            mapTiles.push_back(line);
                        }
                    }
                    break;
            }
        }
    }

    // Parameter check
    if (!towersSet || !enemiesSet || !mapsSet) {
        defFile.close();
        throw (LoadException("Game data file is missing some sections."));
    }

    defFile.close();
}

std::unordered_map<int, CDefinitions::TowerDefinition> CDefinitions::mTowerDefinitions;
std::unordered_map<int, CDefinitions::EnemyDefinition> CDefinitions::mEnemyDefinitions;
std::unordered_map<int, CDefinitions::MapDefinition> CDefinitions::mMapDefinitions;