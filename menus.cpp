//
//  menus.cpp
//  Up in the Tower
//
//  Created by Gabe on 11/22/20.
//

#include "menus.hpp"

bool pollQuit(SDL_Event* event, bool& quit, bool withPause, SDL_Keycode key) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) quit = true;

        if (withPause && event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == key) {
                return true;
            }
        }
    }
    return false;
}

bool selection(SDL_Event* event, bool& quit, int &selector, int max) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            quit = true;
        } else if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.sym) {
                case SDLK_UP:
                case SDLK_LEFT:
                    if (selector > 0) --selector;
                    break;
                case SDLK_DOWN:
                case SDLK_RIGHT:
                    if (selector < max) ++selector;
                    break;
                case SDLK_RETURN:
                case SDLK_SPACE:
                    return true;
                default:
                    break;
            }
        }
    }
    return false;
}

std::string nameChoice(SDL_Renderer* renderer, bool& quit, bool isPrinceS) {
   /*   THIS NEEDS TO BE IMPLEMENTED WITH TTF PACKAGE
    is selector = 0;
    std::string name;
    bool finished = false;

    std::string folder = baseFolder(isPrinceS);
    Texture nameMenu = Texture(renderer, folder = "menus/name.png");

    SDL_Event event;
    while (!quit && !finished) {
        while (SDL_PollEvent(event)) {
            if (event->type == SDL_QUIT) {
                quit = true;
            } else if (event->type == SDL_KEYDOWN) {
                //HANDLE TYPING
                //MAX characters: 12
                //if enter, finished = true;
            }
        }

        SDL_RendererClear(renderer);

        nameMenu.renderer(ScreenPoint());
        //RENDER NAME
    }
   */
   return "NEWNAME";
}

characterCustomization characterCustomizationMenu(SDL_Renderer* renderer,
                                                  bool &quit, bool isPrinceS,
                                                  bool female) {
    int selector = 0;
    characterCustomization settings;
    
    std::string folder = baseFolder(isPrinceS);
    Texture characterMenu = Texture(renderer, folder + "menus/customization.png",
                                    1200, 800, 1, 2);
    Texture selectorIcon = Texture(renderer, folder + "menus/customizationSelector.png",
                                     250, 125, 6, 2);

    folder += (female ? "F/" : "M/");
    Texture preview = Texture(renderer, folder + "detail.png",
                              600, 1200, 6, 2);
    
    SDL_Event event;

    bool onHair = false;
    
    while (!quit) {
        //SCAN QUIT PHASE
        if (selection(&event, quit, selector,5)) {
            if ((0 < selector && selector <= 5) && !onHair) {
                settings.skin = selector;
                selector = 0;
                onHair = true;
            } else if (selector == 0 && onHair) {
                selector = settings.skin;
                onHair = false;
            } else if (0 < selector) {
                settings.hair = selector;
                break;
            }
        }
        
        //RENDER PHASE
        SDL_RenderClear(renderer);
        
        //Background Render
        characterMenu.render(ScreenPoint(), onHair ? 1 : 0);
        
        //Selector Pointer Render
        selectorIcon.render(ScreenPoint(onHair ? 950 : 0, 25 + 125 * selector),
                            selector + (onHair ? 6 : 0));
        //Section Render
        if (onHair) {
            selectorIcon.render(ScreenPoint(0, 25 + 125 * settings.skin),
                                settings.skin);
            preview.render(ScreenPoint(375, 250), settings.skin, 0.75);
            preview.render(ScreenPoint(375, 250), 6 + selector, 0.75);
        } else {
            preview.render(ScreenPoint(375, 250), selector, 0.75);
        }
        SDL_RenderPresent(renderer);
    }
    return settings;
}


bool typeChoice(SDL_Renderer* renderer, bool& quit) {
    Texture typeMenu = Texture(renderer, "Images/chr/typeMenu.png");
    Texture selectorIcon = Texture(renderer, "Images/chr/typeSelector.png",
                                   400, 600, 2, 1);
    
    SDL_Event event;
    int selector = 0;
    
    while (!quit) {
        if (selection(&event, quit, selector, 1)) break;
    
        //RENDER PHASE
        SDL_RenderClear(renderer);

        typeMenu.render(ScreenPoint());
        selectorIcon.render(ScreenPoint(100 + 600*selector, 200), selector);
    
        SDL_RenderPresent(renderer);
    }
    return selector == 0; //0/left is PrinceS, 1/right is Knight
}

bool genderChoice(SDL_Renderer* renderer, bool& quit, bool isPrinceS) {
    
    std::string folder = baseFolder(isPrinceS);
    
    Texture genderMenu = Texture(renderer, folder + "menus/gender.png");
    Texture selectorIcon = Texture(renderer, folder + "menus/genderSelector.png",
                                   400, 600, 2, 1);

    SDL_Event event;
    int selector = 0;
    
    while (!quit) {
        if (selection(&event, quit, selector, 1)) break;
    
        //RENDER PHASE
        SDL_RenderClear(renderer);
        
        //Background Render
        genderMenu.render(ScreenPoint());
        selectorIcon.render(ScreenPoint(100 + 600*selector, 200), selector);
    
        SDL_RenderPresent(renderer);
    }
    return selector == 0; //0/left is female, 1/right is male
}

std::string baseFolder(bool isPrinceS) {
    std::string folder = "Images/chr/";
    folder += (isPrinceS ? "PrinceS/" : "Knight/");
    return folder;
}

std::string baseFolder(bool isPrinceS, bool female) {
    std::string folder = "Images/chr/";
    folder += (isPrinceS ? "PrinceS/" : "Knight/");
    folder += (female ? "F/" : "M/");
    
    return folder;
}

std::string skinPath(bool isPrinceS, bool female, int skin) {
    std::string folder;
    if (isPrinceS) {
        folder = baseFolder(isPrinceS, female);
    } else {
        folder = baseFolder(isPrinceS);
    }
    return folder + "skin" + std::to_string(skin) + ".png";
}

std::string hairPath(//bool isPrinceS,
                     bool female, int hair) {
    std::string folder = baseFolder(true, female);
    return folder + "hair" + std::to_string(hair) + ".png";
}

void makeCharacter(SDL_Renderer* renderer, int& level,
                   bool isPrinceS, bool female, int skin, int hair,
                   Character*& playerCharacter, Texture*& detail,
                   Texture*& skinTexture, Texture*& hairTexture, 
                   std::string name, int setLevel, 
                   bool A, bool B, bool C, bool D) {
    
    if (detail != NULL) {
        detail = new Texture(renderer, baseFolder(isPrinceS, female) +
                             "detail.png", 600, 1200, 6, 2);
    }
    
    skinTexture = new Texture(renderer,
                              skinPath(isPrinceS, female, skin),
                              50, 100, 10, 4);
    
    if (isPrinceS) {
        //Currently the Knight only has hair in cut scenes.
        hairTexture = new Texture(renderer,
                                  hairPath(female, hair),
                                  50, 100, 10, 4);
        
        playerCharacter = new PrinceS(renderer, skin, hair, female, name,
                                      detail, skinTexture, hairTexture);
        if (setLevel == 0) {
            level = maxLevel;
        } else {
            level = setLevel;
        }
    } else {
        playerCharacter = new Knight(renderer, skin, hair, female, name,
                                     detail, skinTexture);
        if (setLevel == 0) {
            level = 1;
        } else {
            level = setLevel;
        }
    }
}

void newGame(SDL_Renderer* renderer, bool& quit, int& level,
             Character*& playerCharacter, Texture*& detail,
             Texture*& skinTexture, Texture*& hairTexture) {
    
    bool isPrinceS = typeChoice(renderer, quit);
    if (quit) return;
    
    bool female = genderChoice(renderer, quit, isPrinceS);
    if (quit) return;
    
    detail = new Texture(renderer, baseFolder(isPrinceS, female) + "detail.png",
                         600, 1200, 6, 2);
    
    characterCustomization settings;
    settings = characterCustomizationMenu(renderer, quit, isPrinceS, female);
    if (quit) return;

    std::string name = nameChoice(renderer, quit, isPrinceS);
    if (quit) return;
    
    makeCharacter(renderer, level, isPrinceS, female,
                  settings.skin, settings.hair, playerCharacter,
                  detail, skinTexture, hairTexture, name);
}

void loadGame(SDL_Renderer* renderer, bool& quit, int& level,
              Character*& playerCharacter, Texture*& detail,
              Texture*& skinTexture, Texture*& hairTexture) {
    Texture loadMenu = Texture(renderer, "Images/menus/load.png");
    Texture selectorIcon = Texture(renderer, "Images/menus/selectors.png", 200, 100, 9, 1);

    std::string names[3];
    int fileCodes[3];

    std::ifstream saveFiles("Logs/save.dat");
    int clip, i = 0;
    while ((saveFiles >> names[i] >> fileCodes[i]) && (i < 3)) ++i;

    SDL_Event event;
    int selector = 0;
    while (!quit) {
        if (selection(&event,quit,selector,2)) break;
    
        //RENDER PHASE
        SDL_RenderClear(renderer);
        
        loadMenu.render(ScreenPoint());
        clip = (names[selector] == "" ? 5 : 2);
        selectorIcon.render(ScreenPoint(800, 150 + 200 * selector), clip);
        //render file names

        SDL_RenderPresent(renderer);
    }

    if (names[selector] == "") {
        newGame(renderer, quit, level, playerCharacter, detail, 
                skinTexture, hairTexture);
    } else {
        loadSave(renderer, level, playerCharacter, detail, 
                 skinTexture, hairTexture, fileCodes[selector], names[selector]);
    }
}

void saveGame(SDL_Renderer* renderer, bool& quit,
              Character* playerCharacter) {
    bool isPrinceS = playerCharacter->isPrinceS();
    std::string folder = baseFolder(isPrinceS);
    Texture saveMenu = Texture(renderer, folder + "menus/save.png");
    Texture selectorIcon = Texture(renderer, "Images/menus/selectors.png", 200, 100, 9, 1);
    
    SDL_Event event;
    int selector = 0;
    
    //open file
    std::string names[3];
    int fileCodes[3];
    std::ifstream saveFiles("Logs/save.dat");
    int clip, i = 0;
    while ((saveFiles >> names[i] >> fileCodes[i]) && (i < 3)) ++i;
    saveFiles.close();
     
    while (!quit) {
        if (selection(&event,quit,selector,3)) break;
        
        //RENDER PHASE
        SDL_RenderClear(renderer);
        
        saveMenu.render(ScreenPoint());
        clip = ((names[selector] == "") ? 3 : 0);
        if (selector == 3) clip = 6;
        clip += (isPrinceS ? 0 : 1);
        selectorIcon.render(ScreenPoint(800, 50 + 200*selector),clip);
        //renderNames;
        
        SDL_RenderPresent(renderer);
    }
    
    if (quit || selector == 3) return;
    
    //write file
    names[selector] = playerCharacter->name();
    fileCodes[selector] = makeSave(playerCharacter);
    
    std::ofstream writeFiles("Logs/save.dat");
    for (int i = 0; i < 3; ++i) {
        writeFiles << names[i] << fileCodes[i];
    }
    writeFiles.close();
}

void gameChoice(SDL_Renderer* renderer, bool& quit, int& level,
                Character*& playerCharacter, Texture*& detail,
                Texture*& skinTexture, Texture*& hairTexture) {
    Texture gameMenu = Texture(renderer,
                               "Images/menus/gameMenu.png");
    Texture selectorIcon = Texture(renderer,
                                   "Images/menus/gameSelector.png",
                                   300, 250, 2, 1);
    if (TEST_MODE) {
        //PICK SPECIFICS??
        loadGame(renderer, quit, level, playerCharacter, detail,
                 skinTexture, hairTexture);
    } else {
        SDL_Event event;
        int selector = 0;
        
        while (!quit) {
            if (selection(&event,quit,selector,1)) break;
            
            //RENDER PHASE
            SDL_RenderClear(renderer);
            
            //Background Render
            gameMenu.render(ScreenPoint());
            selectorIcon.render(ScreenPoint(250 + 400*selector, 500), selector);
        
            SDL_RenderPresent(renderer);
        }
        if (quit) return;
        if (selector == 0) {
            newGame(renderer, quit, level, playerCharacter, detail,
                    skinTexture, hairTexture);
        } else if (selector == 1) {
            loadGame(renderer, quit, level, playerCharacter, detail,
                     skinTexture, hairTexture);
        }
    }
}

unsigned int encode(unsigned int rawNumber) {
    return ((rawNumber + 4382) * 58311) % 65536;
}

unsigned int decode(unsigned int encodedNumber) {
    return ((encodedNumber * 60919) - 4382) % 65536;
}


unsigned int makeSave(Character* playerCharacter) {
    bool isPrinceS = playerCharacter->isPrinceS();
    bool female = playerCharacter->isFemale();
    int skin = playerCharacter->skin();
    int hair = playerCharacter->hair();
    int level = playerCharacter->furthestLevel();
    
    return makeSave(isPrinceS, female, skin, hair, level,
                    false, false, false, false);
}

//These extra variables are for items
unsigned int makeSave(bool isPrinceS, bool female, int skin, int hair,
                      int level, bool A, bool B, bool C, bool D) {
    unsigned int save = 40 * (5 * ((hair-1)%5) + (skin-1)%5) + (level % 40);
    save <<= 1;
    save |= (D ? 1 : 0);
    save <<= 1;
    save |= (C ? 1 : 0);
    save <<= 1;
    save |= (B ? 1 : 0);
    save <<= 1;
    save |= (A ? 1 : 0);
    save <<= 1;
    save |= (female ? 1 : 0);
    save <<= 1;
    save |= (isPrinceS ? 1 : 0);
    return encode(save);
}

void loadSave(SDL_Renderer* renderer, int& level, Character*& playerCharacter,
              Texture*& detail, Texture*& skinTexture, Texture*& hairTexture,
              unsigned int fileNumber, std::string name) {
    int save = int(decode(fileNumber));
    bool isPrinceS = (save % 2) == 1;
    save >>= 1;
    bool female = (save % 2) == 1;
    save >>= 1;
    bool A = (save % 2) == 1;
    save >>= 1;
    bool B = (save % 2) == 1;
    save >>= 1;
    bool C = (save % 2) == 1;
    save >>= 1;
    bool D = (save % 2) == 1;
    save >>= 1;
    int setLevel = int(save % 40);
    save /= 40;
    int skin = int(save % 5);
    int hair = save / 5;
    
    makeCharacter(renderer, level, isPrinceS, female, skin, hair,
                  playerCharacter, detail, skinTexture, hairTexture,
                  name, setLevel, A, B, C, D);
}
