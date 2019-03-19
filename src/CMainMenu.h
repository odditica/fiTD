/*!
 * @class CMainMenu
 * @brief Main menu control and renderering.
 */

#ifndef HMAINMENU_H
#define HMAINMENU_H

#include "CGfx.h"
#include <fstream>
#include "config.h"
static const int MENU_ITEMS = 4;

class CMainMenu {
public:

    enum class EMENU_ITEM {
        MAP_SELECTION,
        START,
        LOAD,
        QUIT,
        NONE
    };

    static float fade;
    static bool endMenu;
    static std::string menuItemString[MENU_ITEMS];
    static WINDOW* winMenuTopBar;
    static WINDOW* winMenuBottomBar;
    static WINDOW* winMenuSpace;
    static EMENU_ITEM selectedItem;
    static EMENU_ITEM validatedItem;
    static unsigned int mapCount;
    static unsigned int selectedMap;
    static bool canLoad;

    /*
     * @brief Sets the label of a given menu item
     */
    static void SetMenuItemString(EMENU_ITEM item, const std::string & str) {
        menuItemString[(int) item] = str;
    }

    /*
     * @brief Retrieves the label of a given menu item.
     */
    static std::string GetMenuItemString(EMENU_ITEM item) {
        return menuItemString[(int) item];
    }

    /*
     * @brief Moves the item selector to the next menu item.
     */
    static void MenuNext() {
        selectedItem = (EMENU_ITEM) (((int) selectedItem + 1) % MENU_ITEMS);
        if (selectedItem == EMENU_ITEM::LOAD && !canLoad) {
            MenuNext();
        }
    }

    /*
     * @brief Moves the item selector to a previous menu item.
     */
    static void MenuPrevious() {
        selectedItem = (EMENU_ITEM) (((int) selectedItem - 1 + MENU_ITEMS) % MENU_ITEMS);
        if (selectedItem == EMENU_ITEM::LOAD && !canLoad) {
            MenuPrevious();
        }
    }

    /*
     * @brief Checks if the currently selected map has a save file
     */
    static void CheckLoad() {
        canLoad = false;
        std::string filename = std::to_string(selectedMap) + ".sav";
        std::ifstream saveFile;
        saveFile.open(filename, std::ios_base::in);
        if (saveFile.good()) {
            canLoad = true;
        }
        saveFile.close();
    }

    /*     
     * @brief Initial menu setup
     */
    static void Setup() {
        SetMenuItemString(EMENU_ITEM::START, "Start");
        SetMenuItemString(EMENU_ITEM::LOAD, "Load");
        SetMenuItemString(EMENU_ITEM::QUIT, "Quit");
        SetMenuItemString(EMENU_ITEM::MAP_SELECTION, "Map: 0");

        delwin(winMenuSpace);
        delwin(winMenuTopBar);
        delwin(winMenuBottomBar);
        winMenuTopBar = newwin(3, COLS, 0, 0);
        winMenuBottomBar = newwin(3, COLS, LINES - 3, 0);
        winMenuSpace = newwin(LINES - 6, COLS, 3, 0);
        selectedMap = 0;
        canLoad = false;
        nodelay(stdscr, true);
        nodelay(winMenuSpace, true);
        keypad(stdscr, true);
        keypad(winMenuSpace, true);
        CheckLoad();
        fade = 1;
        endMenu = false;
        selectedItem = EMENU_ITEM::MAP_SELECTION;
    }

    /*     
     * @brief Processes menu input. If an item has been selected using the enter key, marks it as such.
     */
    static void MenuProcessInput() {
        if (CGfx::justResized >= 0) {
            if (CGfx::justResized == 0) {
                keypad(winMenuSpace, true);
            } else {
                keypad(winMenuSpace, false);
            }
            CGfx::justResized--;
            return;
        }
        switch (wgetch(winMenuSpace)) {
            case KEY_DOWN:
                MenuNext();
                break;
            case KEY_UP:
                MenuPrevious();
                break;
            case KEY_LEFT:
                if (selectedItem == EMENU_ITEM::MAP_SELECTION) {
                    selectedMap = (selectedMap - 1 + mapCount) % mapCount;
                    SetMenuItemString(EMENU_ITEM::MAP_SELECTION, "Map: " + std::to_string(selectedMap));
                    CheckLoad();
                }
                break;
            case KEY_RIGHT:
                if (selectedItem == EMENU_ITEM::MAP_SELECTION) {
                    selectedMap = (selectedMap + 1 + mapCount) % mapCount;
                    SetMenuItemString(EMENU_ITEM::MAP_SELECTION, "Map: " + std::to_string(selectedMap));
                    CheckLoad();
                }
                break;

            case KEY_ENTER: case 13:
                validatedItem = selectedItem;
                break;
            case 27:
                validatedItem = EMENU_ITEM::QUIT;
                break;
        }
    }

    /*
     * @brief Maximises all windows.
     */
    static void Maximise() {
        mvwin(winMenuBottomBar, LINES - 3, 0);
        wresize(winMenuSpace, LINES - 6, COLS);
        wresize(winMenuTopBar, 3, COLS);
        wresize(winMenuBottomBar, 3, COLS);
    }

    /*
     * @brief Main update function, executes every frame.
     */
    static void MenuUpdate() {
        validatedItem = EMENU_ITEM::NONE;
        werase(winMenuTopBar);
        werase(winMenuBottomBar);
        werase(winMenuSpace);

        //Transition
        fade *= .75;
        if (fade <= .005) fade = 0;
        std::stringstream titleStream;
        titleStream <<  "\\Ufi\\^U\\BTD\\^B by Jan Vorisek | Version \\B" << PROJECT_VER_MAJOR << "." << PROJECT_VER_MINOR << "." << PROJECT_VER_PATCH << "\\^B | Built on \\B" << PROJECT_BUILD_DATE << "\\^B";
        CGfx::PrintFormatted(winMenuTopBar, 1, 2 + COLS * fade, titleStream.str());
        CGfx::PrintFormatted(winMenuBottomBar, 1, 2, "\\BArrow Keys\\^B - Navigation / Map selection | \\BEnter\\^B - Select");

        // Render menu items
        for (int i = 0; i < MENU_ITEMS; ++i) {
            std::string menuStr = menuItemString[i];
            if ((int) selectedItem == i) {
                wattron(winMenuSpace, A_UNDERLINE | A_BOLD);
                if ((EMENU_ITEM) i == EMENU_ITEM::MAP_SELECTION) {
                    menuStr = "< " + menuStr + " >";
                }
            }

            int itemLen = menuStr.length();
            int itX = COLS / 2 - itemLen / 2;

            if ((EMENU_ITEM) i == EMENU_ITEM::LOAD && !canLoad) {
                wattron(winMenuSpace, A_DIM);
            }
            mvwaddstr(winMenuSpace, 5 + i, itX - (fade * (1 + i * .4)) * (itX + itemLen), menuStr.c_str());
            wattroff(winMenuSpace, A_UNDERLINE | A_BOLD);
            wattroff(winMenuSpace, A_DIM);
        }

        wborder(winMenuTopBar, 0, 0, 0, 0, 0, 0, 0, 0);
        wborder(winMenuBottomBar, 0, 0, 0, 0, 0, 0, 0, 0);
        wborder(winMenuSpace, 0, 0, 0, 0, ACS_DIAMOND, ACS_DIAMOND, ACS_DIAMOND, ACS_DIAMOND);

        MenuProcessInput();

        wrefresh(winMenuSpace);
        wrefresh(winMenuTopBar);
        wrefresh(winMenuBottomBar);
    }

    /*
     * @brief Destroys existing windows.
     */
    static void Cleanup() {
        delwin(CMainMenu::winMenuSpace);
        delwin(CMainMenu::winMenuTopBar);
        delwin(CMainMenu::winMenuBottomBar);
    }

private:
    CMainMenu();
};

float CMainMenu::fade;
bool CMainMenu::endMenu;
WINDOW* CMainMenu::winMenuTopBar;
WINDOW* CMainMenu::winMenuBottomBar;
WINDOW* CMainMenu::winMenuSpace;
std::string CMainMenu::menuItemString[MENU_ITEMS];
CMainMenu::EMENU_ITEM CMainMenu::selectedItem;
CMainMenu::EMENU_ITEM CMainMenu::validatedItem;
unsigned int CMainMenu::mapCount;
unsigned int CMainMenu::selectedMap;
bool CMainMenu::canLoad;
#endif /* HMAINMENU_H */

