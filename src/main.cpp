/*
 * fiTD by Jan Vorisek
 * 04/06/2018
 */

#include <ncurses.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <deque>

#include "CDefinitions.h"
#include "CGfx.h"
#include "CGameGraphics.h"
#include "CMainMenu.h"
#include "CGame.h"

static const int FRAME_DELAY = 30000;
static const int MIN_WIDTH = 110;
static const int MIN_HEIGHT = 35;

enum class EPROGRAM_STATE{
    STARTING,
    MENU,
    PLAYING
};

CGame* gamePtr;
EPROGRAM_STATE programState = EPROGRAM_STATE::STARTING;
bool programRunning = true;


/**
 * @brief Sets program state
 * @param state Program state
 */
void SetProgramState(EPROGRAM_STATE state){
    if (programState == state) return;
    if (programState == EPROGRAM_STATE::MENU){
            CMainMenu::Cleanup();
    }
   
    //Clean-up
    werase(stdscr);   
    erase();
    refresh();
  
    if (state == EPROGRAM_STATE::MENU){
        programState = EPROGRAM_STATE::MENU;        
        CMainMenu::Setup();
    }
    if (state == EPROGRAM_STATE::PLAYING){        
        programState = EPROGRAM_STATE::PLAYING;        
    }
    
}

/**
 * @brief Process actions of the validated menu item
 */
void ProcessMenuSelection(){    
    switch (CMainMenu::validatedItem){
        case CMainMenu::EMENU_ITEM::QUIT:
            //Exit the program
            programRunning = false;
        break;
        case CMainMenu::EMENU_ITEM::START:            
            //Start a normal game

            try {
                gamePtr = CGame::GetInstance(CMainMenu::selectedMap);  
            } catch  (const char* msg) {
                // Show an error message if something fails
                CGameGraphics::ThrowError(static_cast<std::string>(msg));
                break;
            }
            SetProgramState(EPROGRAM_STATE::PLAYING);                      
            
        break;
        case CMainMenu::EMENU_ITEM::LOAD:            
            //Start a game from a save file
            gamePtr = CGame::GetInstance(CMainMenu::selectedMap, true);  
            SetProgramState(EPROGRAM_STATE::PLAYING);                      
        break;
        default: break;
    }    
}

/**
 * @brief Handle terminal resizing
 */
void HandleResizing(){
    CGfx::winOldW = CGfx::winNewW;
    CGfx::winOldH = CGfx::winNewH;
    getmaxyx(stdscr, CGfx::winNewH, CGfx::winNewW);
    
    //Detect if the terminal window is too small
    while (CGfx::winNewH < MIN_HEIGHT|| CGfx::winNewW < MIN_WIDTH){            
        refresh();
        erase();
        
        //Inform the user        
        attron(A_BLINK);
        printw("Please resize the terminal!\n");
        attroff(A_BLINK);
        printw("Current size: %dx%d\nRequired size: %dx%d", CGfx::winNewW, CGfx::winNewH, MIN_WIDTH, MIN_HEIGHT);
        getmaxyx(stdscr, CGfx::winNewH, CGfx::winNewW);        
    }
    
    //Detect resizing and call the according methods based on game state
    if (CGfx::winNewW != CGfx::winOldW || CGfx::winNewH != CGfx::winOldH) {
        CGfx::winOldW = CGfx::winNewW;
        CGfx::winOldH = CGfx::winNewH;
        endwin();
        refresh();                        
        if (programState == EPROGRAM_STATE::MENU)    CMainMenu :: Maximise();
        if (programState == EPROGRAM_STATE::PLAYING) CGameGraphics::Maximise();
        CGfx::justResized = 1;
    }
}

int main() {         
    //Load definitions and maps
    try{
        CDefinitions::LoadDefinitions();
    }catch (const CDefinitions::LoadException & e){
        std::cout << "ERROR: " << e.mErrorMessage << std::endl;
        return 1;
    }           
    CMainMenu::mapCount = CDefinitions::GetMapDefinitions().size();
    
    //Setup graphics
    CGfx::CursesSetup(); 
    
    //Starting in the main menu
    SetProgramState(EPROGRAM_STATE::MENU);
    
    //MAIN PROGRAM LOOP
    while (programRunning) {        
        HandleResizing();
        if (programState == EPROGRAM_STATE::MENU){
            CMainMenu::MenuUpdate();        
            ProcessMenuSelection();
        }
        if (programState == EPROGRAM_STATE::PLAYING){                                    
            if (gamePtr->mQuit){
                CGame::Destroy();                
                gamePtr = 0;
                SetProgramState(EPROGRAM_STATE::MENU);
            }else{
                gamePtr->GameLoop();     
            }            
        }       
        //Frame delay
        usleep(FRAME_DELAY);        
    }

    //Clean-up and exit
    if (programState == EPROGRAM_STATE::MENU) CMainMenu::Cleanup();
                         
    endwin();
}