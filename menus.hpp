//
//  menus.hpp
//  Up in the Tower
//
//  Created by Gabe on 11/22/20.
//

#ifndef menus_hpp
#define menus_hpp

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "sprite.hpp"
#include "character.hpp"

#include <stdio.h>
#include <fstream>
#include <string>

//the return value if the optional key is pushed.
//Defaults to no key, but if a key is selected, it is assumed to be p for pause.
//quit is passed by reference
bool pollQuit(SDL_Event* event, bool& quit, bool withButton = false,
              SDL_Keycode key = SDLK_p);

bool selection(SDL_Event* event, bool& quit, int &selector, int max);

struct characterCustomization {
    int skin;
    int hair;
};

std::string nameChoice(SDL_Renderer* renderer, bool & quit, bool isPrinceS);

characterCustomization characterCustomizationMenu(SDL_Renderer* renderer,
                                                  bool &quit,
                                                  bool isPrinceS = true,
                                                  bool female = true);

bool typeChoice(SDL_Renderer* renderer, bool& quit);

bool genderChoice(SDL_Renderer* renderer, bool& quit, bool isPrinceS);

std::string baseFolder(bool isPrinceS);

std::string baseFolder(bool isPrinces, bool female);

std::string skinPath(bool isPrinceS, bool female, int skin);

std::string hairPath(bool female, int hair);

void makeCharacter(SDL_Renderer* renderer, int& level,
                   bool isPrinceS, bool female, int skin, int hair,
                   Character*& playerCharacter, Texture*& detail,
                   Texture*& skinTexture, Texture*& hairTexture, 
                   std::string name, int setLevel = 0, 
                   bool A = false, bool B = false,
                   bool C = false, bool D = false);

void newGame(SDL_Renderer* renderer, bool& quit, int& level,
             Character*& playerCharacter, Texture*& detail,
             Texture*& skinTexture, Texture*& hairTexture);

void loadGame(SDL_Renderer* renderer, bool& quit, int& level,
              Character*& playerCharacter, Texture*& detail,
              Texture*& skinTexture, Texture*& hairTexture);

void saveGame(SDL_Renderer* renderer, bool& quit,
              Character* playerCharacter);

void gameChoice(SDL_Renderer* renderer, bool& quit, int& level,
                Character*& playerCharacter, Texture*& detail,
                Texture*& skinTexture, Texture*& hairTexture);


unsigned int encode(unsigned int rawNumber);

unsigned int decode(unsigned int encodedNumber);

unsigned int makeSave(Character* playerCharacter);

unsigned int makeSave(bool isPrinceS, bool female, int skin, int hair,
                      int level, bool A, bool B, bool C, bool D);

void loadSave(SDL_Renderer* renderer, int& level, Character*& playerCharacter,
              Texture*& detail, Texture*& skinTexture, Texture*& hairTexture,
              unsigned int fileNumber, std::string name);

#endif /* menus_hpp */
