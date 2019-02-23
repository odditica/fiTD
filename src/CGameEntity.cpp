#include "CGame.h"
#include "CGameEntity.h"
#include <sstream>

CGameEntity::CGameEntity(CGame* gameInstance, int specId, int posX, int posY, int updateDelay, int width, int height, int colour, char symbol, float health): mParentGame(gameInstance), mSpecId(specId), mX(posX), mY(posY), mUpdateDelay(updateDelay), mWidth(width), mHeight(height), mColour(colour), mSymbol(symbol), mHealth(health), mActionTimer(updateDelay), mAttackEffectTimer(0), mDamageEffectTimer(0), mDestroyed(false){}

std::ofstream & operator << (std::ofstream & stream, CGameEntity & entity){
    entity.StringRepresentation(stream);    
    return stream;
}

bool CGameEntity::OverlapPoint(int x, int y) const{
    return (x >= mX && x <= mX + mWidth - 1 && y >= mY && y <= mY + mHeight - 1);
}

bool CGameEntity::OverlapRect(int x, int y, int width, int height) const{
    return (x + width - 1 >= mX && y + height - 1 >= mY && x <= mX + mWidth - 1 && y <= mY + mHeight - 1);         
}

chtype CGameEntity::GetSymbol() const{
    return mSymbol | COLOR_PAIR(mColour) | A_BOLD;
}