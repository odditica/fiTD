/*!
 * @class CGameEntity
 * @brief Represents a polymorphic game entity.
 */

#ifndef CGAMEENTITY_H
#define CGAMEENTITY_H

#include "curses.h"
#include <fstream>

// FORWARD DECLARATION
class CGame;

struct CGameEntity {
    friend class CGameGraphics;
    friend class CGame;

    ~CGameEntity() {
    };

    /**
     * @brief Entity constructor
     * @param gameInstance Parent game instance
     * @param specId Definition ID
     * @param posX X-position
     * @param posY Y-position
     * @param updateDelay Delay between actions
     * @param width Width
     * @param height Height 
     * @param colour Colour
     * @param symbol Symbol
     * @param health Health
     */
    CGameEntity(CGame* gameInstance, int specId, int posX, int posY, int updateDelay, int width, int height, int colour, char symbol, float health);

    /**
     * @brief Virtual draw method called on every entity every frame.
     * @param win Window to draw into
     */
    virtual void Draw(WINDOW* win) const {
    };

    /**
     * @brief Virtual update method called on every entity every frame.
     */
    virtual void Update() {
    };

    /**
     * @brief Virtual damage method called on an entity when they take damage.
     * @param hitPoints Hit points
     * @param showEffect Show effect on screen
     */
    virtual void Damage(float hitPoints, bool showEffect = true) {
    };

    /**
     * @bried Virtual destroy method that destroys its entity
     */
    virtual void Destroy() {
    };

    /**
     * @brief Retrieves the colored symbol of this entity
     * @return Symbol
     */
    chtype GetSymbol() const;

    //Collision detection

    /**
     * @brief Detects if a given point overlaps this entity
     * @param x X-position
     * @param y Y-position
     * @return True if an overlap is found
     */
    bool OverlapPoint(int x, int y) const;

    /**
     * @brief Detects if a given region overlaps this entity (AABB)
     * @param x X-position
     * @param y Y-position
     * @param width Width
     * @param height Height
     * @return True if an overlap is found
     */
    bool OverlapRect(int x, int y, int width, int height) const;

protected:
    friend std::ofstream & operator<<(std::ofstream & stream, CGameEntity & entity);

    virtual void StringRepresentation(std::ofstream & stream) const {
    };
    CGame* mParentGame; //Parent game instance
    int mSpecId, mX, mY, mUpdateDelay, mWidth, mHeight, mColour;
    char mSymbol;
    float mHealth;
    int mActionTimer, mAttackEffectTimer, mDamageEffectTimer;
    bool mDestroyed; //Functions as a flag for CGame    
};

/**
 * @brief Outputs current state information into a file stream 
 * @param stream File stream
 * @param entity Entity
 * @return File stream
 */
std::ofstream & operator<<(std::ofstream & stream, CGameEntity & entity); //Used when exporting data for saving

#endif /* CGAMEENTITY_H */

