/*!
 * @class CTower
 * @brief A type of game entity representing a tower.
 */

#ifndef CTOWER_H
#define CTOWER_H

#include "CGameEntity.h"
#include "CEnemy.h"
#include <fstream>

class CTower : public CGameEntity {
public:
    /**
     * 
     * @param game Pointer to game instance
     * @param specId ID Definition ID (used for saving)
     * @param posX X position
     * @param posY Y position
     * @param symbol Symbol used for rendering
     * @param colour Colour pair used for rendering
     * @param width Width
     * @param height Height
     * @param health Health
     * @param radius Attack radius
     * @param frequency Attack frequency
     * @param damage Attack damage
     */
    CTower(CGame* game, int specId, int posX, int posY, char symbol, int colour, int width, int height, float health, float radius, int attackFrequency, int damage);

    /**
     * @brief Tower update, called every frame
     */
    virtual void Update();

    /**
     * @brief Renders this tower
     * @param win Window to draw into
     */
    virtual void Draw(WINDOW* win) const;

    /**
     * @brief Damage this tower
     * @param hitPoints Hit points
     * @param showEffect Whether or not to show the effect on the screen (true by default)
     */
    virtual void Damage(float hitPoints, bool showEffect = true);

    /**
     * @brief Destroy this tower
     */
    virtual void Destroy();
private:
    /**
     * @brief Outputs a string representation of this tower into a file stream
     * @param stream Output file stream
     */
    virtual void StringRepresentation(std::ofstream & stream) const;
    friend class CGameGraphics;
    //Properties
    int mRadius, mDamage;
};

#endif /* CTOWER_H */

