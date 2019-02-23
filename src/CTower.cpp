#include <string>
#include <fstream>
#include "CTower.h"
#include "CGameGraphics.h"
#include "CGfx.h"

CTower::CTower (CGame* game, int specId, int posX, int posY, char symbol, int colour, int width, int height, float health, float radius, int frequency, int damage) :
    CGameEntity(game, specId, posX, posY, frequency, width, height, colour, symbol, health),
    mRadius(radius), mDamage(damage) {    
} 

void CTower::StringRepresentation(std::ofstream & stream) const{    
    stream << "@TOWER" << std::endl;
    stream << "\tX " << mX << std::endl;
    stream << "\tY " << mY << std::endl;
    stream << "\tHP " << mHealth << std::endl;
    stream << "\tTIME " << mActionTimer << std::endl;
    stream << "\tID " << mSpecId << std::endl;
    stream << "-" << std::endl;
}

void CTower::Draw(WINDOW* win) const {
    for (int y = 0; y < mHeight; ++y){
        for (int x = 0; x < mWidth; ++x){
            
            int effect = 0;                   
            if (mAttackEffectTimer > 0){
                effect |= A_UNDERLINE;             
            }
            if (mDamageEffectTimer <= 0){
                effect |= A_BOLD;
            }
            mvwaddch(CGameGraphics::winMainField, 1 + y + mY, 1 + x + mX, mSymbol | COLOR_PAIR(mColour) | effect);
            
        }
    }
}

void CTower::Damage(float hitPoints, bool showEffect){
    if (showEffect) mDamageEffectTimer = 10;
    mHealth -= hitPoints;
    if (mHealth <= 0){
        mHealth = 0;
        Destroy();
    }
}

void CTower::Destroy() {
    mDestroyed = true;
    mParentGame->TowerCleanup(*this);
}

void CTower::Update(){
    mActionTimer--;
    mAttackEffectTimer --;
    mDamageEffectTimer --;
    
    //Do entity actions if it's time
    if (mActionTimer <= 0) {
        
        //Find the nearest enemy and attack it
        CEnemy* attackedEnemy = 0;
        float minDist = 9999;
        
        for (auto e : mParentGame->GetReadOnlyEntities()){            
            if (typeid(*e) == typeid(CEnemy)){
                auto eP = std::dynamic_pointer_cast<CEnemy>(e);
                float dist = mParentGame->Distance(mX + (float)(mWidth - 1) / 2, mY + (float)(mHeight - 1) / 2, eP->mX, eP->mY);
                if (dist <= mRadius && dist <= minDist){
                    minDist = dist;
                    attackedEnemy = &*eP;                    
                }
            }
        }
        if (attackedEnemy != 0){            
            Damage(.1, false); //Take damage by existing
            mAttackEffectTimer = 10;
            
            // If the enemy being attacked resists all attacks of towers of this type, don't take damage
            if (attackedEnemy->mResists != mSpecId){
                attackedEnemy->Damage(mDamage);
            } else {
                CGameGraphics::ShowMessage("\\C2\\BEnemy resists attack!");
            }
                    
        }        
        mActionTimer = mUpdateDelay;
    }
}
