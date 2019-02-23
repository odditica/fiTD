#include <string>
#include <iterator>
#include <algorithm>
#include <curses.h>
#include <ncurses.h>
#include <cstring>
#include "CGfx.h"

unsigned int CGfx::winOldH = LINES, CGfx::winOldW = COLS, CGfx::winNewW = COLS, CGfx::winNewH = LINES;
int CGfx::justResized = 0;

void CGfx::CursesSetup(){
    initscr();
    start_color();
    use_default_colors();    
    curs_set(0);        
    noecho();
    cbreak();        
    raw();
    nonl();
 
    // Init colour pairs.
    init_pair((short)ECOLORS::WHITE, COLOR_WHITE, -1);
    init_pair((short)ECOLORS::RED, COLOR_RED, -1);  
    init_pair((short)ECOLORS::BLUE, COLOR_BLUE, -1);  
    init_pair((short)ECOLORS::MAGENTA, COLOR_MAGENTA, -1);  
    init_pair((short)ECOLORS::GREEN, COLOR_GREEN, -1);  
    init_pair((short)ECOLORS::BLACK_ON_WHITE, -1, COLOR_WHITE); 
    init_pair((short)ECOLORS::FILLED_YELLOW, -1, COLOR_YELLOW); 
    init_pair((short)ECOLORS::YELLOW, COLOR_YELLOW, -1);  
    init_pair((short)ECOLORS::CYAN, COLOR_CYAN, -1);  
        
    winOldW = winNewW = COLS;
    winOldH = winNewH = LINES;  
}

int CGfx::GetMask(std::string maskStr) {
    int res;
    switch (maskStr[0]) {
        case 'b': res = A_BLINK;
            break;
        case 'B': res = A_BOLD;
            break;
        case 'D': res = A_DIM;
            break;
        case 'U': res = A_UNDERLINE;
            break;
        case 'S': res = A_STANDOUT;
            break;            
        case 'R': res = A_REVERSE;
            break;                        
        case 'C':
            res = COLOR_PAIR(maskStr[1] - '0');
            break;
        default: res = 0;
            break;
    }
    return res;
}

unsigned int CGfx::FormatString(chtype* & in, std::string & str, int n) {
    int attrs = 0;
    unsigned int off = 0;
    for (int i = 0; i < n; ++i) {
        in[i] = '\0';
    }
    for (int i = 0; i < n; ++i) {
        char chr = str[i];
        if (chr == '\0') break;
        if (chr != '\\') {
            chtype c = chr | attrs;

            in[off] = c;
            off++;
        } else {            
            if (str[i + 1] == '^') {
                //turning off
                std::string attrStr = "";
                attrStr += str[i + 2];
                attrStr += str[i + 3];
                attrs = attrs & ~(GetMask(attrStr));
                i++;
                if (attrStr[0] == 'C') i++;
            } else {
                std::string attrStr = "";
                attrStr += str[i + 1];
                attrStr += str[i + 2];
                attrs = attrs | GetMask(attrStr);
                if (attrStr[0] == 'C') i++;
            }
            i++;
        }
    }    
    return off;
}

void CGfx::PrintFormatted(WINDOW* win, int y, int x, std::string str){
    chtype chars[255];
    chtype* charPtr = &chars[0];         
    FormatString(charPtr, str, 255);   
    mvwaddchstr(win, y, x, charPtr);
}

void CGfx::PrintFormattedHCentered(WINDOW* win, int y, std::string str){
    chtype chars[255];
    chtype* charPtr = &chars[0];    
    auto len = FormatString(charPtr, str, 255);        
    mvwaddchstr(win, y, win->_maxx / 2 - len / 2 + 1, charPtr);
}

