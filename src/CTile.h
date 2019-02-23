/*!
 * @class CTile
 * @brief Represents a map tile. Also used as a node for pathfinding.
 */

#ifndef CTILE_H
#define CTILE_H

class CTile{
public:        
    enum class ETILE_TYPE{
        EXIT,
        VOID,
        WALL,
        GROUND,
        TOWER,                       
    };

    //Properties
    char mSymbol; 
    int mColour;
    ETILE_TYPE mType;     
    
    //Searching 
    float mFScore, mGScore;
    unsigned int mX, mY;
    CTile* mCameFrom;
    
    CTile(): mSymbol(' '), mColour(0), mType(ETILE_TYPE::VOID), mFScore(0), mGScore(0), mX(0), mY(0), mCameFrom(0){}    
};

#endif /* CTILE_H */

