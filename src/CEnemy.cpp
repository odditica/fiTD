#include <deque>
#include "CEnemy.h"
#include "CGame.h"
#include "CGameGraphics.h"
#include <random>
#include <fstream>

CEnemy::CEnemy(CGame* game, int specId, int posX, int posY, char symbol, int colour, float health, int speed, E_WALK_STYLE walkStyle, int attack, int resists) : CGameEntity(game, specId, posX, posY, speed, 1, 1, colour, symbol, health), mWalkStyle(walkStyle), mLastDir(EDIR::NONE), mAttack(attack), mResists(resists) {
    if (mWalkStyle == E_WALK_STYLE::A_STAR) {
        mParentGame->FindPathToExit(mX, mY, this);
    }
}

void CEnemy::Damage(float hitPoints, bool showEffect) {
    if (showEffect) mDamageEffectTimer = 10;
    mHealth -= hitPoints;
    if (mHealth <= 0) {
        mHealth = 0;
        Destroy();
        mParentGame->CountKill(mAttack * 20 + (mResists != 0) * 10 - mUpdateDelay);
    }
}

void CEnemy::Destroy() {
    mDestroyed = true;
}

void CEnemy::Draw(WINDOW* win) const {
    int effect = 0;
    if (mAttackEffectTimer > 0) {
        effect |= A_UNDERLINE;
    }
    if (mDamageEffectTimer <= 0) {
        effect |= A_BOLD;
    }
    mvwaddch(CGameGraphics::winMainField, 1 + mY, 1 + mX, mSymbol | COLOR_PAIR(mColour) | effect);
}

void CEnemy::StringRepresentation(std::ofstream & stream) const {
    stream << "@ENEMY" << std::endl;
    stream << "\tX " << mX << std::endl;
    stream << "\tY " << mY << std::endl;
    stream << "\tHP " << mHealth << std::endl;
    stream << "\tTIME " << mActionTimer << std::endl;
    stream << "\tID " << mSpecId << std::endl;
    stream << "\tDIR " << (int) mLastDir << std::endl;
    stream << "-" << std::endl;
}

void CEnemy::Update() {
    mActionTimer--;
    mAttackEffectTimer--;
    mDamageEffectTimer--;

    //Only perform actions when it's time
    if (mActionTimer <= 0) {

        //Exit detection
        if (mParentGame->TileTypeAtPosition(mX, mY) == CTile::ETILE_TYPE::EXIT) {
            mDestroyed = true;
            CGameGraphics::ShowMessage("\\C5\\BAn enemy has left the scene!");
            return;
        }

        //Field crossing strategy - sticking to walls        
        if (mWalkStyle == E_WALK_STYLE::STICK_TO_WALLS) {
            if (mParentGame->TileTypeAtPosition(mX + 1, mY) == CTile::ETILE_TYPE::VOID) {
                mLastDir = EDIR::R;
            }

            bool R = (mParentGame->TileWalkable(mX + 1, mY)) && mLastDir != EDIR::R;
            bool RU = (mParentGame->TileWalkable(mX + 1, mY - 1));
            bool U = (mParentGame->TileWalkable(mX, mY - 1)) && mLastDir != EDIR::U;
            bool LU = (mParentGame->TileWalkable(mX - 1, mY - 1));
            bool L = (mParentGame->TileWalkable(mX - 1, mY)) && mLastDir != EDIR::L;
            bool LD = (mParentGame->TileWalkable(mX - 1, mY + 1));
            bool D = (mParentGame->TileWalkable(mX, mY + 1)) && mLastDir != EDIR::D;
            bool RD = (mParentGame->TileWalkable(mX + 1, mY + 1));

            bool continueMoving = true;
            //Sometimes nudge in a different direction
            if (std::rand() % 8 == 7) {
                switch (std::rand() % 4) {
                    case 0:
                        if (L) {
                            mX--;
                            mLastDir = EDIR::R;
                            continueMoving = false;
                        }
                        break;
                    case 1:
                        if (R) {
                            mX++;
                            mLastDir = EDIR::L;
                            continueMoving = false;
                        }
                        break;
                    case 2:
                        if (U) {
                            mY--;
                            mLastDir = EDIR::D;
                            continueMoving = false;
                        }
                        break;
                    case 3:
                        if (D) {
                            mY++;
                            mLastDir = EDIR::U;
                            continueMoving = false;
                        }
                        break;
                }
            }
            if (continueMoving) {

                if (L && R && !D && !U && !RD && !LD && mLastDir == EDIR::D) {
                    mX++;
                    mLastDir = EDIR::L;
                } else
                    if (!L && D && !LD && !U) {
                    mY++;
                    mLastDir = EDIR::U;
                } else
                    if (!L && U && !R && mLastDir == EDIR::D) {
                    mY--;
                    mLastDir = EDIR::D;
                } else
                    if (L && !U && !R) {
                    mX--;
                    mLastDir = EDIR::R;
                } else
                    if (U && !R && !D) {
                    mY--;
                    mLastDir = EDIR::D;
                } else
                    if (D && !L && !U) {
                    mY++;
                    mLastDir = EDIR::U;
                } else
                    if (R && !D && !L) {
                    mX++;
                    mLastDir = EDIR::L;
                } else
                    if (!R && U && !RU) {
                    mY--;
                    mLastDir = EDIR::D;
                } else
                    if (L && !U && !LU) {
                    mX--;
                    mLastDir = EDIR::R;
                } else
                    if (D && !LD && !L) {
                    mY++;
                    mLastDir = EDIR::U;
                } else
                    if (R && !RD && !D) {
                    mX++;
                    mLastDir = EDIR::L;
                } else
                    if (!R && !U && !L && D) {
                    mY++;
                    mLastDir = EDIR::U;
                } else
                    if (!L && !U && !R && !D && mLastDir == EDIR::U && (mParentGame->TileWalkable(mX, mY - 1))) {
                    mY--;
                    mLastDir = EDIR::D;
                } else
                    if (!L && !U && !R && !D && mLastDir == EDIR::L && (mParentGame->TileWalkable(mX - 1, mY))) {
                    mX--;
                    mLastDir = EDIR::R;
                } else
                    if (!L && !U && !R && !D && mLastDir == EDIR::R && (mParentGame->TileWalkable(mX + 1, mY))) {
                    mX++;
                    mLastDir = EDIR::L;
                } else
                    if (!L && !R && !U && !D && mLastDir == EDIR::D && (mParentGame->TileWalkable(mX, mY + 1))) {
                    mY++;
                    mLastDir = EDIR::U;
                } else
                    if (L && R && U && !D && mLastDir == EDIR::D) {
                    mY--;
                    mLastDir = EDIR::D;
                } else
                    if (L && R && !U && D && mLastDir == EDIR::U) {
                    mY++;
                    mLastDir = EDIR::U;
                } else
                    if (L && !R && U && D && mLastDir == EDIR::R) {
                    mX--;
                    mLastDir = EDIR::R;
                } else
                    if (!L && R && U && D && mLastDir == EDIR::L) {
                    mX++;
                    mLastDir = EDIR::L;
                } else {
                    switch (std::rand() % 4) {
                        case 0:
                            if (L) {
                                mX--;
                                mLastDir = EDIR::R;
                            }
                            break;
                        case 1:
                            if (R) {
                                mX++;
                                mLastDir = EDIR::L;
                            }
                            break;
                        case 2:
                            if (U) {
                                mY--;
                                mLastDir = EDIR::D;
                            }
                            break;
                        case 3:
                            if (D) {
                                mY++;
                                mLastDir = EDIR::U;
                            }
                            break;
                    }
                }
            }
        } else if (mWalkStyle == E_WALK_STYLE::A_STAR) {
            //Field crossing strategy - A* with variation
            //Retrieve a node from the stack and go to it
            if (!mWalkPath.empty()) {
                CTile* nextTile = mWalkPath.top();
                mX = nextTile->mX;
                mY = nextTile->mY;
                mWalkPath.pop();
            }
        }

        // Attack surrounding towers
        for (auto e : mParentGame->GetReadOnlyEntities()) {
            if (typeid (*e) == typeid (CTower)) {
                auto eP = std::dynamic_pointer_cast<CTower>(e);
                if (eP->OverlapRect(mX - 1, mY - 1, 3, 3)) {
                    eP->Damage(mAttack);
                }
            }
        }
        mActionTimer = mUpdateDelay;
    }
}