/*!
 * @class CGameGraphics
 * @brief Handles rendering during gameplay. Behaves statically, cannot be instantiated.
 */

#ifndef CGAMEGRAPHICS_H
#define CGAMEGRAPHICS_H

#include <deque>
#include <string>
#include "CGameGraphics.h"
#include "CGame.h"

class CGameGraphics {
public:
    static WINDOW* winMainField;
    static WINDOW* winSidebar;
    static WINDOW* winBottomBar;
    static int cursorX, cursorY;
    static bool messageDistinguish;
    static const unsigned int MESSAGE_LIMIT; //Message limit
    static CGame* gamePtr; //Pointer to parent game controlling the rendering

    /**
     * @brief Initial setup
     * @param game Parent game instance
     */
    static void Setup(CGame* game);

    /**
     * @brief Display an error screen
     * @param str Error message
     */
    static void ThrowError(const std::string & str);

    /**
     * @brief Maximise all windows
     */
    static void Maximise();

    /**
     * @brief Draw a formatted sidebar statistic
     * @param y Y-position
     * @param str Label
     * @param num Numeric value
     */
    static void DrawSideStat(int y, const std::string & str, int num);

    /**
     * @brief Draw a formatted sidebar statistic
     * @param y Y-position
     * @param str Label
     * @param val Text value
     */
    static void DrawSideStat(int y, const std::string & str, const std::string & val);

    /**
     * @brief Render all game elements
     */
    static void Draw();
    static std::deque<std::string> messageLog;

    /**
     * @brief Show a formatted message
     * @param str Formatted string
     */
    static void ShowMessage(std::string str);
private:
    CGameGraphics();
};


#endif /* CGAMEGRAPHICS_H */

