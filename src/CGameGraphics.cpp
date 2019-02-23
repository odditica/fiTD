#include "CGameGraphics.h"
#include "CGfx.h"

#include <ncurses.h>
#include <iomanip>
#include <deque>
#include <sstream>

static const int SPEED_BASE = 30;
static const int SIDEBAR_WIDTH = 18;
static const int BOTTOM_BAR_HEIGHT = 10;
static const int MAX_LINE_LENGTH = 256;

WINDOW* CGameGraphics::winMainField;
WINDOW* CGameGraphics::winSidebar;
WINDOW* CGameGraphics::winBottomBar;
int CGameGraphics::cursorX, CGameGraphics::cursorY;
CGame* CGameGraphics::gamePtr;
bool CGameGraphics::messageDistinguish;
const unsigned int CGameGraphics::MESSAGE_LIMIT = 6; //Message limit
std::deque<std::string> CGameGraphics::messageLog;

void CGameGraphics::Setup(CGame* game) {        
    delwin(winMainField);
    delwin(winSidebar);
    delwin(winBottomBar);

    gamePtr = game;
    winMainField = newwin(LINES - BOTTOM_BAR_HEIGHT, COLS - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH);
    winSidebar = newwin(LINES - BOTTOM_BAR_HEIGHT, SIDEBAR_WIDTH, 0, 0);
    winBottomBar = newwin(BOTTOM_BAR_HEIGHT, COLS, LINES - BOTTOM_BAR_HEIGHT, 0);
    cursorX = gamePtr->mMapWidth / 2;
    cursorY = gamePtr->mMapHeight / 2;
    messageDistinguish = false;

    nodelay(stdscr, true);
    nodelay(winMainField, true);
    keypad(stdscr, true);
    keypad(winMainField, true);
}

void CGameGraphics::ShowMessage(std::string str) {    
    std::replace(str.begin(), str.end(), '\n', ' '); //Get rid of newlines    
    messageDistinguish = !messageDistinguish; //Arrow boldness toggle
    messageLog.emplace_front(str);
    if (messageLog.size() > MESSAGE_LIMIT) {
        messageLog.erase(messageLog.end());
    }
}

void CGameGraphics::ThrowError(const std::string & str) {
    delwin(CGameGraphics::winBottomBar);
    delwin(CGameGraphics::winMainField);
    delwin(CGameGraphics::winSidebar);
    endwin();
    erase();
    attron(COLOR_PAIR(ECOLORS::RED) | A_BOLD);
    mvwprintw(stdscr, 0, 0, ("ERROR: " + str).c_str());
    attroff(A_BOLD);
    mvwprintw(stdscr, 1, 0, "Press any key to quit.");
    attroff(COLOR_PAIR(ECOLORS::RED));
    refresh();

    //Wait for any key
    while (getch() == -1) {
    }
}

void CGameGraphics::Maximise() {
    wresize(winMainField, LINES - BOTTOM_BAR_HEIGHT, COLS - SIDEBAR_WIDTH);
    wresize(winSidebar, LINES - BOTTOM_BAR_HEIGHT, SIDEBAR_WIDTH);
    wresize(winBottomBar, BOTTOM_BAR_HEIGHT, COLS);
    mvwin(winSidebar, 0, 0);
    mvwin(winMainField, 0, SIDEBAR_WIDTH);
    mvwin(winBottomBar, LINES - BOTTOM_BAR_HEIGHT, 0);
}

void CGameGraphics::DrawSideStat(int y, const std::string & str, int num) {
    std::stringstream ss;
    ss << str << std::setw(SIDEBAR_WIDTH - str.length() - 2) << num << std::flush;
    mvwprintw(winSidebar, y, 1, ss.str().c_str());

    short c = (short) ECOLORS::WHITE;
    if (str == "$: ") {
        if (num < 50) {
            c = (short) ECOLORS::RED;
        } else {
            c = (short) ECOLORS::GREEN;
        }
    }
    mvwchgat(winSidebar, y, 8, SIDEBAR_WIDTH - 9, A_BOLD, c, 0);
}

void CGameGraphics::DrawSideStat(int y, const std::string & str, const std::string & val) {
    std::stringstream ss;
    ss << str << std::setw(SIDEBAR_WIDTH - str.length() - 2) << val << std::flush;
    mvwprintw(winSidebar, y, 1, ss.str().c_str());
    short c = (short) ECOLORS::WHITE;
    mvwchgat(winSidebar, y, 8, SIDEBAR_WIDTH - 9, A_BOLD, c, 0);
}

void CGameGraphics::Draw() {
    
    //Erase
    werase(winBottomBar);
    werase(winSidebar);
    werase(winMainField);

    //Render tiles
    for (unsigned int y = 0; y <= gamePtr->mMapHeight; ++y) {
        for (unsigned int x = 0; x <= gamePtr->mMapWidth; ++x) {
            mvwaddch(winMainField, 1 + y, 1 + x, (char) (gamePtr->mMapTiles[y][x].mSymbol) | gamePtr->mMapTiles[y][x].mColour);
        }
    }

    //Render the spawn point
    mvwaddch(winMainField, 1 + gamePtr->mSpawnY, 1 + gamePtr->mSpawnX, '<' | A_BOLD | COLOR_PAIR(ECOLORS::RED));

    //Render all entities
    for (auto e : gamePtr->mMapEntities) {
        e->Draw(winMainField);
    }

    //Render main statistics
    wattron(winSidebar, A_BOLD | A_UNDERLINE);
    mvwprintw(winSidebar, 3, 1, "Enemies:");
    wattroff(winSidebar, A_BOLD | A_UNDERLINE);
    mvwprintw(winSidebar, 2, 1, "ENEMY:");
    CGameGraphics::DrawSideStat(1, "$: ", (int) gamePtr->mMoney);
    CGameGraphics::DrawSideStat(2, "Score: ", gamePtr->mScore);
    CGameGraphics::DrawSideStat(4, "Target: ", gamePtr->mKillGoal);
    CGameGraphics::DrawSideStat(5, "Killed: ", gamePtr->mEnemiesKilled);
    CGameGraphics::DrawSideStat(6, "Waiting: ", gamePtr->mEnemySequence.size() - gamePtr->mEnemySequenceIndex);
    mvwhline(winSidebar, 7, 1, 0, SIDEBAR_WIDTH - 2);

    // Render bottom part of the side panel based on game state
    switch (gamePtr->mGameState) {

        case EGAME_STATE::IDLE: case EGAME_STATE::PAUSE:
        {
            // Draw sidebar stats when the cursor is hover on an entity
            for (auto e : gamePtr->mMapEntities) {
                if (typeid (*e) == typeid (CEnemy)) {                    
                    //Enemy
                    auto eP = std::dynamic_pointer_cast<CEnemy>(e);
                    if (eP->mX == cursorX && eP->mY == cursorY) {
                        wattron(winSidebar, A_BOLD | A_UNDERLINE);
                        mvwprintw(winSidebar, 8, 1, "Enemy:");
                        wattroff(winSidebar, A_BOLD | A_UNDERLINE);
                        mvwaddch(winSidebar, 8, SIDEBAR_WIDTH - 2, eP->GetSymbol());
                        if (eP->mResists != 0) {
                            auto t = CDefinitions::GetTowerDefinition(eP->mResists);
                            DrawSideStat(12, "Resists: ", "");
                            mvwaddch(winSidebar, 12, SIDEBAR_WIDTH - 2, t.mSymbol | COLOR_PAIR(t.mColour) | A_BOLD);
                        }
                        DrawSideStat(9, "HP: ", (int) eP->mHealth);
                        DrawSideStat(10, "Attack: ", (int) eP->mAttack);
                        DrawSideStat(11, "Speed: ", (int) SPEED_BASE - eP->mUpdateDelay);
                    }
                    
                } else if (typeid (*e) == typeid (CTower)) {                    
                    //Tower
                    auto eP = std::dynamic_pointer_cast<CTower>(e);
                    if (eP->OverlapPoint(cursorX, cursorY)) {
                        wattron(winSidebar, A_BOLD | A_UNDERLINE);
                        mvwprintw(winSidebar, 8, 1, "Tower:");
                        wattroff(winSidebar, A_BOLD | A_UNDERLINE);
                        mvwaddch(winSidebar, 8, SIDEBAR_WIDTH - 2, eP->GetSymbol());
                        DrawSideStat(9, "HP: ", (int) eP->mHealth);
                        DrawSideStat(10, "Attack: ", (int) eP->mDamage);
                        DrawSideStat(11, "Radius: ", (int) eP->mRadius);
                        DrawSideStat(12, "Speed: ", (int) SPEED_BASE - eP->mUpdateDelay);
                        if (gamePtr->mGameState == EGAME_STATE::IDLE) {
                            std::stringstream ss;
                            ss << "\\BS\\^B to sell (\\U$" << gamePtr->TowerSellingPrice(*eP) << "\\^U)" << std::flush;
                            CGfx::PrintFormattedHCentered(winSidebar, 19, ss.str());
                        }
                    }
                }
            }

            if (gamePtr->mGameState == EGAME_STATE::IDLE) {
                CGfx::PrintFormattedHCentered(winSidebar, 17, "\\BW\\^B to build");
                CGfx::PrintFormattedHCentered(winSidebar, 18, "\\BSpace\\^B to pause");
            } else if (gamePtr->mGameState == EGAME_STATE::PAUSE) {
                CGfx::PrintFormattedHCentered(winSidebar, 17, "\\BGame paused.");
                CGfx::PrintFormattedHCentered(winSidebar, 18, "\\BSpace\\^B to unpause");
            }

            break;
        }
     
        case EGAME_STATE::PLACING_TOWER:
        {            
            //Render tower selection information
            
            wattron(winSidebar, A_BOLD | A_UNDERLINE);
            mvwprintw(winSidebar, 8, 1, "Available towers:");
            wattroff(winSidebar, A_BOLD | A_UNDERLINE);

            auto t = CDefinitions::GetTowerDefinitionByOrderIndex(gamePtr->mSelectedTower);

            mvwaddch(winSidebar, 9, SIDEBAR_WIDTH / 2, t.mSymbol | COLOR_PAIR(t.mColour) | A_BOLD);
            mvwaddch(winSidebar, 9, SIDEBAR_WIDTH / 2 - 2, '<' | COLOR_PAIR(ECOLORS::WHITE) | A_BOLD | A_BLINK);
            mvwaddch(winSidebar, 9, SIDEBAR_WIDTH / 2 + 2, '>' | COLOR_PAIR(ECOLORS::WHITE) | A_BOLD | A_BLINK);

            std::stringstream sstr;

            sstr << "$" << t.mPrice << std::flush;

            CGameGraphics::DrawSideStat(10, "Price: ", sstr.str());

            sstr.str("");
            sstr << t.mWidth << "x" << t.mHeight << std::flush;

            CGameGraphics::DrawSideStat(11, "Size: ", sstr.str());
            CGameGraphics::DrawSideStat(12, "HP: ", t.mHealth);
            CGameGraphics::DrawSideStat(13, "Attack: ", t.mDamage);
            CGameGraphics::DrawSideStat(14, "Radius: ", t.mRadius);
            CGameGraphics::DrawSideStat(15, "Speed: ", SPEED_BASE - t.mFrequency);

            CGfx::PrintFormattedHCentered(winSidebar, 17, "\\BQ\\^B/\\BE\\^B to navigate");
            CGfx::PrintFormattedHCentered(winSidebar, 18, "\\BW\\^B to buy");
            CGfx::PrintFormattedHCentered(winSidebar, 19, "\\BSpace\\^B to cancel");

            //DRAW CURSOR
            int xOffset = 0, yOffset = 0;
            if (t.mWidth % 2 != 0) xOffset = t.mWidth / 2;
            if (t.mHeight % 2 != 0) yOffset = t.mHeight / 2;

            for (int y = 0; y < t.mHeight; ++y) {
                for (int x = 0; x < t.mWidth; ++x) {
                    chtype c = mvwinch(winMainField, cursorY + 1 - yOffset + y, cursorX + 1 - xOffset + x) & A_CHARTEXT;
                    mvwaddch(winMainField, cursorY + 1 - yOffset + y, cursorX + 1 - xOffset + x, c | A_REVERSE);
                }
            }
            break;
        }
        default: break;
    }

    //Bottom panel info
    CGfx::PrintFormatted(winBottomBar, BOTTOM_BAR_HEIGHT - 2, 1, "\\BArrow Keys\\^B - Cursor movement | \\BEscape\\^B - Save and exit");
    mvwhline(winBottomBar, BOTTOM_BAR_HEIGHT - 3, 1, 0, COLS - 2);
    
    //Render the cursor
    chtype c = mvwinch(winMainField, cursorY + 1, cursorX + 1) & A_CHARTEXT;    
    mvwaddch(winMainField, cursorY + 1, cursorX + 1, c | A_REVERSE | A_BOLD);

    

    //Render messages
    int offset = 0;
    for (auto message : messageLog) {
        chtype chars[MAX_LINE_LENGTH];
        chtype* charPtr = &chars[0];
        CGfx::FormatString(charPtr, message, MAX_LINE_LENGTH);
        if ((offset + (int) messageDistinguish) % 2 == 0) wattron(winBottomBar, A_BOLD);
        mvwaddstr(winBottomBar, BOTTOM_BAR_HEIGHT - 4 - offset, 1, "> ");
        wattroff(winBottomBar, A_BOLD);
        mvwaddchstr(winBottomBar, BOTTOM_BAR_HEIGHT - 4 - offset, 3, charPtr);
        offset++;
    }

    //Render cursor position
    std::stringstream ss;
    ss << std::setw(4) << cursorX << std::setw(4) << cursorY << std::flush;
    mvwprintw(winMainField, LINES - BOTTOM_BAR_HEIGHT - 2, COLS - SIDEBAR_WIDTH - 9, ss.str().c_str());

    //Borders
    wborder(winBottomBar, 0, 0, 0, 0, 0, 0, 0, 0);
    wborder(winSidebar, 0, 0, 0, 0, 0, 0, 0, 0);
    wborder(winMainField, 0, 0, 0, 0, ACS_DIAMOND, ACS_DIAMOND, ACS_DIAMOND, ACS_DIAMOND);
    
    //Refresh
    wrefresh(winBottomBar);
    wrefresh(winSidebar);
    wrefresh(winMainField);
}

