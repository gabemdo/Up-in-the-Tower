//
//  main.cpp
//  Up in the Tower
//
//  Created by Gabe on 10/28/20.
//

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

#include "menus.hpp"
#include "level.hpp"
#include "sprite.hpp"
#include "character.hpp"
#include "enemy.hpp"

//Declaration
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;


void initialize();
void close();
void runOpeningScreen(bool &quit);
void levelManager(bool& quit, int& level, Character*& playerCharacter);

int main() {
    
    int level;
    bool quit = false;
    
    //INITIALIZE GUI
    try {
        initialize();
    } catch (std::string exceptionMessage) {
        std::cout << exceptionMessage << std::endl;
    }
    
    if (!TEST_MODE) runOpeningScreen(quit);
    
    //Create Character
    Character* playerCharacter = NULL;
    Texture* detail = NULL;
    Texture* skinTexture = NULL;
    Texture* hairTexture = NULL;
    
    if (TEST_MODE) {
        skinTexture = new Texture(renderer, "Images/chr/PrinceS/M/skin5.png",
                                  50, 100, 10,  4);
        hairTexture = new Texture(renderer, "Images/chr/PrinceS/M/hair3.png",
                                  50, 100, 10,  4);
        detail = new Texture(renderer, "Images/chr/PrinceS/M/detail.png",
                             600, 1200, 6, 2);
        playerCharacter = new PrinceS(renderer, 5, 3, false, "TESTMODE",
                                      detail, skinTexture, hairTexture);
        level = TEST_LEVEL;
    } else {
        gameChoice(renderer, quit, level, playerCharacter,
                   detail, skinTexture, hairTexture);
    }
    
    //Main Game Play
    levelManager(quit, level, playerCharacter);

    //Free Memory and Close
    if (playerCharacter != NULL) delete playerCharacter;
    if (hairTexture != NULL) delete hairTexture;
    if (skinTexture != NULL) delete skinTexture;
    
    
     
      if (detail != NULL) delete detail;
    
    close();
    return 0;
}

void initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::string("SDL initialization failed \nError: ") + SDL_GetError();
    }
    
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        std::cout << "WARNING: Linear filtering failed. Defaulting to nearest pixel sampling." << std::endl;
    }
    
    window = SDL_CreateWindow(WINDOW_TITLE,
                              SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        throw std::string("Window creation failed. \nError: ") + SDL_GetError();
    }
    
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        throw std::string("Renderer failed. \nError: ") + SDL_GetError();
    }
    
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
    int flags = IMG_INIT_PNG;
    if (!(IMG_Init(flags) & flags)) {
        throw std::string("PNG processing failed. \nError") + SDL_GetError();
    }
}

void close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}

void runOpeningScreen(bool &quit) {
    Texture towerOpening = Texture(renderer, "Images/menus/towerOpening.png");
    Texture title = Texture(renderer, "Images/menus/title.png");
    
    SDL_Rect openingClip = {0, 1800, 800, 600};
    SDL_Rect openingFrame = {200, 100, 800, 600};
    
    SDL_Event event;
    
    bool play = true;
    int frame = -30; //0.5 seconds for VSYNC to catch up
    
    int repetitions = 5;
    int riseLength = 60 * (3 + repetitions);
    int height, offset;
    
    while (!quit && play) {
        play ^= pollQuit(&event, quit,true,SDLK_RETURN);
        
        SDL_RenderClear(renderer);
        
        //RISING UP THE TOWER
        if (0 <= frame && frame <= riseLength) {
            //smooth rise modeled by cos on [0,pi/2]
            height = int(5*riseLength*(1 + cos(M_PI*frame/riseLength)) + 0.5);
            //repetition logic
            //The tower has 4 blocks: the top, two repeaters, the bottom
            //This makes sure we start on the bottom and repeat correctly
            offset = height / 600 - 1;
            offset = (offset > 0) ? offset : 0;
            offset = (offset < repetitions) ? offset : repetitions;
            
            openingClip.y = height - 600 * offset;
        } else if (riseLength + 30 < frame && frame <= riseLength + 270) {
            // +30 corresponds to 0.5 second pause
            // Fade on Title for 2 seconds and hold for 2 seconds
            openingClip.y = 0;
            offset = Uint8((frame - riseLength - 30) * (255.f/240.f));
            offset = (offset > 255) ? 255 : offset;
            title.setOpacity(Uint8(offset));
            towerOpening.setOpacity(Uint8(255 - offset));
            title.render(&openingClip, &openingFrame);
        } else if (frame >= riseLength + 270) {
            play = false;
        }
        
        towerOpening.render(&openingClip, &openingFrame);
        
        SDL_RenderPresent(renderer);
        ++frame;
    }
}

void levelManager(bool& quit, int& level, Character*& playerCharacter) {
    if (!quit) {
        //GAME PLAY
        //Final Version: Levels Number 0-40
        //0 Is the win scene level for PrinceS
        //40 is the win scence level for Knight
        while (level != -1) {
            switch (level) {
                case 1:
                    level = gremlinLevel(renderer, playerCharacter);
                    break;
                case 2:
                    level = ogreLevel(renderer, playerCharacter);
                    break;
                case 3:
                    level = sentryLevel(renderer, playerCharacter);
                    break;
                case 4:
                    level = ghostLevel(renderer, playerCharacter);
                    break;
                case 5:
                    level = guardLevel(renderer, playerCharacter);
                    break;
                
                case 6:
                case 0: {
                    //WIN CUT SCENE NEEDED
                    std::cout << "YOU BEAT THE GAME!" << std::endl;
                    level = -1;
                    break;
                }
                default:
                    level = -1;
                    break;
            }
        
        }
    }
}

