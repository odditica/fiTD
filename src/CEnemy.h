/*!
 * @class CEnemy
 * @brief A type of game entity representing an enemy.
 */

#ifndef CENEMY_H
#define CENEMY_H

#include "CGameEntity.h"
#include "CTile.h"
#include <stack>
#include <fstream>

class CEnemy : public CGameEntity {
public:

    // Field crossing strategy

    enum struct E_WALK_STYLE {
        STICK_TO_WALLS,
        A_STAR
    };

    // Last direction, used when sticking to walls to help better determine the next move

    enum class EDIR {
        R,
        RU,
        U,
        LU,
        L,
        LD,
        D,
        RD,
        NONE
    };

    /**
     * @brief Enemy constructor.
     * @param game Pointer to game instance
     * @param posX X posision
     * @param posY Y position
     * @param symbol Symbol used for rendering
     * @param colour Colour pair used for rendering
     * @param health Health
     * @param speed Speed
     * @param walkStyle Field crossing strategy
     */
    CEnemy(CGame* game, int specId, int posX, int posY, char symbol, int colour, float health, int speed, E_WALK_STYLE walkStyle, int attack, int resists);

    /**
     * @brief Main update function, called every frame
     */
    virtual void Update();

    /**
     * @brief Enemy rendering
     * @param win Window to draw into
     */
    virtual void Draw(WINDOW* win) const;

    /**
     * @brief Damage this enemy
     * @param hitPoints Hit points
     * @param showEffect Whether or not to show the effect on the screen (true by default)
     */
    virtual void Damage(float hitPoints, bool showEffect = true);

    /**
     * @brief Destroy this enemy
     */
    virtual void Destroy();
    std::stack<CTile*> mWalkPath; //Set of CTiles used as a path of nodes, only used when using pathfinding
    E_WALK_STYLE mWalkStyle;
    EDIR mLastDir; //Opposite direction of last movement (used when sticking to walls)    

private:
    /**
     * @brief Outputs a string representation of this enemy into a file stream
     * @param stream Output file stream
     */
    virtual void StringRepresentation(std::ofstream & stream) const;
    friend class CGameGraphics;
    friend class CTower;
    int mAttack, mResists;
    int mLastNode;
};

#endif /* CENEMY_H */

