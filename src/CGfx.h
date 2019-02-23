/*!
 * @class CGfx
 * @brief Provides some additional rendering functionality.It behaves statically and cannot be instantiated.
 */

#ifndef CGFX_H
#define CGFX_H

#include <algorithm>
#include <curses.h>
#include <ncurses.h>

// Color pair IDs
enum class ECOLORS : short {
    WHITE = 1,
    RED = 2,
    BLACK_ON_WHITE = 3,
    BLUE = 4,
    MAGENTA = 5,
    GREEN = 6,
    FILLED_YELLOW = 7,
    YELLOW = 8,
    CYAN = 9
};

class CGfx {
public:
    static unsigned int winOldH, winOldW, winNewW, winNewH; //Resizing    
    static int justResized; //Used to block input for a few frames after resizing the window

    /**
     * @brief Initial Curses setup.
     */
    static void CursesSetup();

    /**
     * @brief Translates an attribute symbol to a mask. 
     * @param maskStr String containing the attribute
     * @return mask 
     */
    static int GetMask(std::string maskStr);

    /**
     * @brief Translates a formatted string into a chtype*
     * @param in Reference to output chtype*
     * @param str Input string
     * @param n Number of characters to process
     * @return Final length of the string
     */
    static unsigned int FormatString(chtype* & in, std::string & str, int n);

    /**
     * @brief Prints a decorated string
     * @param win Window to draw into
     * @param y Y-position
     * @param x X-position
     * @param str Input string
     */
    static void PrintFormatted(WINDOW* win, int y, int x, std::string str);

    /**
     * @brief Prints a decorated string horizontally centered in a given window
     * @param win Window to draw into
     * @param y Y-position 
     * @param str Input string
     */
    static void PrintFormattedHCentered(WINDOW* win, int y, std::string str);
private:
    CGfx();
};

#endif /* CGFX_H */

