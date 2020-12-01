//
//  level.cpp
//  Up in the Tower
//
//  Created by Gabe on 11/3/20.
//


#include "level.hpp"

Level::Level(int levelNumber, SDL_Renderer* renderer, Character* character,
             std::string backgroundImage, std::string doorImage,
             std::string foregroundImage, LevelType type) :
_levelNumber{levelNumber}, _renderer{renderer}, _character{character},
_board{Board(renderer, character, backgroundImage, foregroundImage)},
_levelType{type} {
    _doors = new Texture(renderer, doorImage, 400, 150, 3, 2);
    
    bool isPrinceS = _character->isPrinceS();
    _character->setBoard(&_board);
    _character->reset(BoardPoint(isPrinceS));
    _character->changeHeading(isPrinceS? R : L);
    _board.set(_character, BoardPoint(isPrinceS));
    _board.setDoors(_doors);
    _character->setBoard(&_board);
}

//TODO: make doors only open on one side.
//TODO: eventually make a level selector.
Level::~Level() {
    if (_doors != NULL) delete _doors;
    if (_gem != NULL) delete _gem;
    for (Enemy* enemy : _enemies) delete enemy;
    for (Sprite* set : _setPieces) delete set;
}

int Level::runLevel() {
    _totalEnemies = int(_enemies.size());
    
    bool quit = false;
    bool paused = false;
    bool menu = false;
    int finalFrame = -1;
    SDL_Event event;

    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    while (!quit && !menu) {
        if (paused) {
            paused ^= pollQuit(&event,quit,true);
        } else {
            paused ^= pollQuit(&event,quit,true);
            
            if (_wonAndDone) {
                _character->win();
            } else if ((_time % 2) == 0 && !_wonAndDone) {
                _activeEnemies = 0;
                for (Enemy* enemy : _enemies) {
                    if (enemy->takeTurn(_time)) ++_activeEnemies;
                }
                
                if (_activeEnemies == 0 && _levelType == killAll) {
                    _board.won();
                }
                
                if (_character->hp() > 0) {
                    _character->scanMove(currentKeyStates, _time);
                } else {
                    if (finalFrame < 0) {
                        finalFrame = _time + 225;
                    }
                    _character->die();
                }
            }
        }
        
        //RENDER PHASE
        SDL_RenderClear(_renderer);
 
        checkWin(_time,finalFrame);
        
        //BOARD
        if ( _wonAndDone ) {
            _board.render(0);
        } else {
            _board.render(_time);
        }
        
        //RENDER INFO
        renderInfo(_time);
        
        //OVERLAYS
        if (_wonAndDone) {
            _character->renderWin();
        } else if (paused) {
            _character->renderPauseMenu();
        } else if (_character->hp() == 0) {
            _character->renderDeath();
        }

        SDL_RenderPresent(_renderer);
        
        //frame rate is 30 fps
        if (!paused) {
            _time++;
            /* Timer for testing
            if (_time % 100 == 0) {
                std::cout << "TIME : " << _time << std::endl;
            }*/
        }
        
        if (_time == finalFrame) menu = true;
    }
    
    _character->reinitialize();
    
    if (quit) {
        return -1;
    } else if (_wonAndDone) {
        return winMenu();
    } else {
        return loseMenu();
    }
    
}

void Level::checkWin(int time, int &finalFrame) {
    if (_board.isWon()) {
        if (_character->n() == 6 &&
            (_character->m() == -2 || _character->m() == 13)) {
            if (finalFrame < 0) {
                finalFrame = time + 150;
            }
            _wonAndDone = true;
        }
    }
}

int Level::winMenu() {
    int selector = 0;
    std::string folder = baseFolder(_character->isPrinceS());
    Texture winMenu = Texture(_renderer, folder + "menus/win.png");
    Texture selectorIcon = Texture(_renderer, folder + "menus/selectors.png",
                                   150, 100, 4, 1);
        
    SDL_Event event;

    bool selected = false;
    bool quit = false;
    
    while (!selected) {
        //SCAN QUIT PHASE
        selected = selection(&event, quit, selector, 2);
        if (quit) return -1;
        if (selected && selector == 1) {
            saveGame(_renderer, quit, _character);
            selected = false;
        }
        
        //RENDER PHASE
        SDL_RenderClear(_renderer);
        
        //Background Render
        winMenu.render(ScreenPoint(300, 200));
        
        //Selector Pointer Render
        selectorIcon.render(ScreenPoint(525, 225 + 125*selector),
                            (selector - 3) % 3 + 3);
        
        SDL_RenderPresent(_renderer);
    }
    if (selector == 0) {
        if (_character->isPrinceS()) {
            return _levelNumber - 1;
        } else {
            return _levelNumber + 1;
        }
    }
    
    return -1;
}

int Level::loseMenu() {
    int selector = 0;
    std::string folder = baseFolder(_character->isPrinceS());
    Texture loseMenu = Texture(_renderer, folder + "menus/lose.png");
    Texture selectorIcon = Texture(_renderer, folder + "menus/selectors.png",
                                   150, 100, 4, 1);

        
    SDL_Event event;

    bool selected = false;
    bool quit = false;
    
    while (!selected) {
        //SCAN QUIT PHASE
        selected = selection(&event, quit, selector, 2);
        if (quit) return -1;
        if (selected && selector == 1) {
            saveGame(_renderer, quit, _character);
            selected = false;
        }
        
        //RENDER PHASE
        SDL_RenderClear(_renderer);
        
        //Background Render
        loseMenu.render(ScreenPoint(300, 200));
        
        //Selector Pointer Render
        selectorIcon.render(ScreenPoint(525, 225 + 125*selector), selector);
        
        SDL_RenderPresent(_renderer);
    }
    if (selector == 0) return _levelNumber;
    return -1;
}


void Level::renderInfo(int time) {
    if (_levelType == killAll) {
        renderMonsterCount(time);
    } else if (_levelType == collectAll && _gem != NULL) {
        _gem->renderInfo(time);
    }
    _character->renderHP(time);
}

void Level::renderMonsterCount(int time) {
    int xposition = 275;
    for (Enemy* enemy : _enemies) {
        enemy->renderIcon(time, ScreenPoint(xposition, 10));
        xposition += int(enemy->size().x()*0.5) + 10;
    }
}

int runTestLevel(SDL_Renderer* renderer, Character* playerCharacter) {
    //CREATE LEVEL
    Level level = Level(1, renderer, playerCharacter,
                        "Images/scene/background/test.png",
                        "Images/scene/doors/test.png");

    Texture iconSheet = Texture(level.renderer(),
                                "Images/icons/icons.png");
    Icon potionTexture = Icon(&iconSheet, 0);
    
    Texture gemKeyTexture = Texture(level.renderer(),
                                    "Images/icons/gem.png",
                                    100, 150, 6, 6);

    //SET SETPIECES
    Texture columnTexture = Texture(level.renderer(),
                                    "Images/scene/prop/columns/tall.png");
    level.setPieces()->push_back(new Sprite(3, 3, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(3, 8, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(8, 3, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(8, 8, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new GemKey(6, 6, &gemKeyTexture,
                                            level.board()));
    
    //SET ENEMIES
    Texture guardTexture = Texture(level.renderer(),
                                   "Images/enemies/guard.png",
                                   150, 200, 6, 1);
    level.enemies()->push_back(new Guard(3, 4, &guardTexture, U, 1,
                                         level.board(), &potionTexture));
    level.enemies()->push_back(new Guard(7, 3, &guardTexture, L, 2,
                                         level.board(), &potionTexture));
    level.enemies()->push_back(new Guard(8, 7, &guardTexture, D, 1,
                                         level.board(), &potionTexture));
    level.enemies()->push_back(new Guard(4, 8, &guardTexture, R, 2,
                                         level.board(), &potionTexture));
    
    Texture gremlinTexture = Texture(level.renderer(),
                                     "Images/enemies/gremlin.png",
                                     50, 100, 10, 4);
    level.enemies()->push_back(new Gremlin( 1, 11, &gremlinTexture, D, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin( 4, 11, &gremlinTexture, D, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin( 7, 11, &gremlinTexture, D, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin(10, 11, &gremlinTexture, D, 1,
                                           level.board()));
    
    return level.runLevel();
}

int gremlinLevel(SDL_Renderer* renderer, Character* playerCharacter) {
    //CREATE LEVEL
    Level level = Level(1, renderer, playerCharacter,
                        "Images/scene/background/entry.png",
                        "Images/scene/doors/entry.png", "", killAll);
    
    Texture iconSheet = Texture(level.renderer(), "Images/icons/icons.png");
    Icon potionTexture = Icon(&iconSheet, 0);
    
    //SET SETPIECES
    Texture throneTexture = Texture(level.renderer(),
                                    "Images/scene/prop/castle/throne.png");
    level.setPieces()->push_back(new Sprite(1, 5, &throneTexture, level.board(),
                                            ScreenPoint(48, 35)));
    level.setPieces()->push_back(new NullSprite(1, 6, level.board()));
    level.setPieces()->push_back(new NullSprite(2, 5, level.board()));
    level.setPieces()->push_back(new NullSprite(2, 6, level.board()));
    
    Texture lpotTexture = Texture(level.renderer(),
                                    "Images/scene/prop/pots/lPurple.png");
    Texture rpotTexture = Texture(level.renderer(),
                                    "Images/scene/prop/pots/rPurple.png");
    level.setPieces()->push_back(new Sprite( 0, 2, &lpotTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite( 0, 9, &lpotTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite(11, 2, &rpotTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite(11, 9, &rpotTexture,
                                            level.board()));
    
    Texture fArmorTexture = Texture(level.renderer(),
                                    "Images/scene/prop/castle/fArmor.png");
    Texture bArmorTexture = Texture(level.renderer(),
                                    "Images/scene/prop/castle/bArmor.png");
    
    level.setPieces()->push_back(new Sprite( 1, 11, &fArmorTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite( 4, 11, &fArmorTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite( 7, 11, &fArmorTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite(10, 11, &fArmorTexture,
                                            level.board()));
    
    level.setPieces()->push_back(new Sprite( 1,  0, &bArmorTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite( 4,  0, &bArmorTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite( 7,  0, &bArmorTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(10,  0, &bArmorTexture,
                                            level.board(), ScreenPoint(0, 40)));
    
    //SET ENEMIES
    Texture gremlinTexture = Texture(level.renderer(),
                                     "Images/enemies/gremlin.png",
                                     50, 100, 10, 5);
    level.enemies()->push_back(new Gremlin( 2, 3, &gremlinTexture, R, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin( 3, 4, &gremlinTexture, R, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin( 3, 7, &gremlinTexture, R, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin(2, 8, &gremlinTexture, R, 1,
                                           level.board()));
    
    level.enemies()->push_back(new Gremlin( 4, 10, &gremlinTexture, D, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin( 6, 10, &gremlinTexture, D, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin( 8, 10, &gremlinTexture, D, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin(10, 10, &gremlinTexture, D, 1,
                                           level.board()));
    
    level.enemies()->push_back(new Gremlin( 4,  1, &gremlinTexture, U, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin( 6,  1, &gremlinTexture, U, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin( 8,  1, &gremlinTexture, U, 1,
                                           level.board()));
    level.enemies()->push_back(new Gremlin(10,  1, &gremlinTexture, U, 1,
                                           level.board()));
    
    return level.runLevel();
}

int ogreLevel(SDL_Renderer* renderer, Character* playerCharacter) {
    //CREATE LEVEL
    Level level = Level(2, renderer, playerCharacter,
                        "Images/scene/background/conservatory.png",
                        "Images/scene/doors/conservatory.png", "", killAll);
    
    Texture iconSheet = Texture(level.renderer(), 
                                "Images/icons/icons.png");
    Icon potionTexture = Icon(&iconSheet, 0);
    
    //SET SETPIECES
    
    std::cout << "setting up" << std::endl;
    Texture tallPlantTexture = Texture(level.renderer(),
                                       "Images/scene/prop/plants/tall.png");
    Texture medPlantTexture = Texture(level.renderer(),
                                        "Images/scene/prop/plants/short.png");
    Texture widePlantTexture = Texture(level.renderer(),
                                       "Images/scene/prop/plants/wide.png");
    Texture columnTexture = Texture(level.renderer(),
                                    "Images/scene/prop/column.png");
    level.setPieces()->push_back(new Sprite( 0,  0, &tallPlantTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite( 0, 11, &tallPlantTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite(11,  0, &tallPlantTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite(11, 11, &tallPlantTexture,
                                            level.board()));
                                 
    level.setPieces()->push_back(new Sprite( 3,  3, &medPlantTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite( 3,  8, &medPlantTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite( 8,  3, &medPlantTexture,
                                            level.board()));
    level.setPieces()->push_back(new Sprite( 8,  8, &medPlantTexture,
                                            level.board()));
    
    level.setPieces()->push_back(new Sprite( 5,  5, &widePlantTexture,
                                            level.board(), ScreenPoint(50, 0)));
    level.setPieces()->push_back(new NullSprite(5, 6, level.board()));
    level.setPieces()->push_back(new NullSprite(6, 5, level.board()));
    level.setPieces()->push_back(new NullSprite(6, 6, level.board()));
    
    //SET ENEMIES
    Texture ogreTexture = Texture(level.renderer(), 
                                  "Images/enemies/ogre.png",
                                  200, 200, 10, 5);
    level.enemies()->push_back(new Ogre( 1, 10, &ogreTexture, D, 1,
                                        level.board()));
    level.enemies()->push_back(new Ogre( 4, 10, &ogreTexture, D, 1,
                                        level.board(), &potionTexture));
    level.enemies()->push_back(new Ogre( 7, 10, &ogreTexture, D, 1,
                                        level.board(), &potionTexture));
    level.enemies()->push_back(new Ogre(10, 10, &ogreTexture, D, 1,
                                        level.board()));
    return level.runLevel();
}

int sentryLevel(SDL_Renderer* renderer, Character* playerCharacter) {
    //CREATE LEVEL
    Level level = Level(3, renderer, playerCharacter,
                        "Images/scene/background/generic.png",
                        "Images/scene/doors/generic.png");
    
    Texture iconSheet = Texture(level.renderer(), 
                                "Images/icons/icons.png");
    Icon potionTexture = Icon(&iconSheet, 0);
    
    Texture gemKeyTexture = Texture(level.renderer(), 
                                    "Images/icons/gem.png",
                                    100, 150, 6, 7);
    
    Texture laserTexture = Texture(level.renderer(), 
                                   "Images/effects/laser.png");
    laserTexture.setLaserClips();

    //SET SETPIECES
    Texture columnTexture = Texture(level.renderer(),
                                    "Images/scene/prop/columns/tall.png");
    
    Texture bustTexture = Texture(level.renderer(),
                                    "Images/scene/prop/columns/bust.png");
    level.setPieces()->push_back(new Sprite(3, 3, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(3, 8, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(8, 3, &bustTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(8, 8, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
   
    level.setGem(new GemKey(6, 6, &gemKeyTexture, level.board()));
    
    //SET ENEMIES
    Texture sentryTexture = Texture(level.renderer(),
                                    "Images/enemies/sentry.png",
                                    100, 200, 4, 1);
    
    level.enemies()->push_back(new Sentry( 0,  0, &sentryTexture,
                                          &laserTexture, R, 1, level.board()));
    level.enemies()->push_back(new Sentry( 0, 11, &sentryTexture,
                                          &laserTexture, D, 1, level.board()));
    level.enemies()->push_back(new Sentry(11,  0, &sentryTexture,
                                          &laserTexture, U, 1, level.board()));
    level.enemies()->push_back(new Sentry(11, 11, &sentryTexture,
                                          &laserTexture, L, 1, level.board()));
    
    return level.runLevel();
}

int ghostLevel(SDL_Renderer* renderer, Character* playerCharacter) {
    //CREATE LEVEL
    Level level = Level(4, renderer, playerCharacter,
                        "Images/scene/background/generic.png",
                        "Images/scene/doors/generic.png",
                        "Images/scene/foreground/ghost.png");
    
    Texture iconSheet = Texture(level.renderer(), 
                                "Images/icons/icons.png");
    Icon potionTexture = Icon(&iconSheet, 0);
    
    Texture gemKeyTexture = Texture(level.renderer(), 
                                    "Images/icons/gem.png",
                                    100, 150, 6, 7);
    
    level.setGem(new GemKey(8, 9, &gemKeyTexture, level.board()));
    

    //SET SETPIECES
    Texture lbookTexture = Texture(level.renderer(),
                                    "Images/scene/prop/library/lShelf.png");
    level.setPieces()->push_back(new Sprite( 2, 11, &lbookTexture,
                                            level.board(), ScreenPoint(50,30)));
    level.setPieces()->push_back(new NullSprite(3, 11, level.board()));
    
    
    Texture rbookTexture = Texture(level.renderer(),
                                    "Images/scene/prop/library/rShelf.png");
    level.setPieces()->push_back(new Sprite( 8, 11, &rbookTexture,
                                            level.board(), ScreenPoint(50,30)));
    level.setPieces()->push_back(new NullSprite(9, 11, level.board()));
    
    Texture lfbookTexture = Texture(level.renderer(),
                                    "Images/scene/prop/library/lfShelf.png");
    level.setPieces()->push_back(new Sprite( 2, 0, &lfbookTexture,
                                            level.board(), ScreenPoint(50,35)));
    level.setPieces()->push_back(new NullSprite(3, 0, level.board()));
    
    
    Texture rfbookTexture = Texture(level.renderer(),
                                    "Images/scene/prop/library/rfShelf.png");
    level.setPieces()->push_back(new Sprite( 8, 0, &rfbookTexture,
                                            level.board(), ScreenPoint(50,30)));
    level.setPieces()->push_back(new NullSprite(9, 0, level.board()));
    
    
    Texture lpotTexture = Texture(level.renderer(),
                                  "Images/scene/prop/pots/lTeal.png");
    level.setPieces()->push_back(new Sprite( 1, 11, &lpotTexture,
                                            level.board(), ScreenPoint(0, 25)));
    level.setPieces()->push_back(new Sprite( 4, 11, &lpotTexture,
                                            level.board(), ScreenPoint(0, 25)));
    level.setPieces()->push_back(new Sprite( 1,  0, &lpotTexture,
                                            level.board(), ScreenPoint(0, 25)));
    level.setPieces()->push_back(new Sprite( 4,  0, &lpotTexture,
                                            level.board(), ScreenPoint(0, 25)));
    
    
    Texture rpotTexture = Texture(level.renderer(),
                                  "Images/scene/prop/pots/rteal.png");
    level.setPieces()->push_back(new Sprite( 7, 11, &rpotTexture,
                                            level.board(), ScreenPoint(0, 25)));
    level.setPieces()->push_back(new Sprite(10, 11, &rpotTexture,
                                            level.board(), ScreenPoint(0, 25)));
    level.setPieces()->push_back(new Sprite( 7,  0, &rpotTexture,
                                            level.board(), ScreenPoint(0, 25)));
    level.setPieces()->push_back(new Sprite(10,  0, &rpotTexture,
                                            level.board(), ScreenPoint(0, 25)));
    
    
    Texture mbookTexture = Texture(level.renderer(),
                                    "Images/scene/prop/library/mShelf.png");
    level.setPieces()->push_back(new Sprite( 5, 3, &mbookTexture,
                                            level.board(), ScreenPoint(50,30)));
    level.setPieces()->push_back(new NullSprite(6, 3, level.board()));
    level.setPieces()->push_back(new NullSprite(5, 4, level.board()));
    level.setPieces()->push_back(new NullSprite(6, 4, level.board()));
    level.setPieces()->push_back(new NullSprite(5, 5, level.board()));
    level.setPieces()->push_back(new NullSprite(6, 5, level.board()));
    level.setPieces()->push_back(new NullSprite(5, 6, level.board()));
    level.setPieces()->push_back(new NullSprite(6, 6, level.board()));
    level.setPieces()->push_back(new NullSprite(5, 7, level.board()));
    level.setPieces()->push_back(new NullSprite(6, 7, level.board()));
    level.setPieces()->push_back(new NullSprite(5, 8, level.board()));
    level.setPieces()->push_back(new NullSprite(6, 8, level.board()));
    
    
    Texture globeTexture = Texture(level.renderer(),
                                   "Images/scene/prop/study/globe.png");
    level.setPieces()->push_back(new Sprite( 2, 6, &globeTexture,
                                            level.board(), ScreenPoint(0, 20)));
    
    
    Texture moonTexture = Texture(level.renderer(),
                                    "Images/scene/prop/study/moon.png");
    level.setPieces()->push_back(new Sprite( 9, 6, &moonTexture,
                                            level.board(), ScreenPoint(0, 20)));
    
    
    Texture torchTexture = Texture(level.renderer(),
                                   "Images/scene/prop/dungeon/blueBrazier.png",
                                   80, 220, 6, 1);
    
    level.setPieces()->push_back(new Sprite( 1, 8, &torchTexture, level.board(),
                                            ScreenPoint(0, 40), 6, 0, 5));
    level.setPieces()->push_back(new Sprite( 1, 4, &torchTexture, level.board(),
                                            ScreenPoint(0, 40), 6, 3, 7));
    level.setPieces()->push_back(new Sprite(10, 8, &torchTexture, level.board(),
                                            ScreenPoint(0, 40), 6, 1, 7));
    level.setPieces()->push_back(new Sprite(10, 4, &torchTexture, level.board(),
                                            ScreenPoint(0, 40), 6, 4, 5));

    
    //SET ENEMIES
    Texture ghostTexture = Texture(level.renderer(),
                                   "Images/enemies/ghost.png",
                                   50, 100, 6, 4);
    level.enemies()->push_back(new Ghost( 3, 11, &ghostTexture, R, 1,
                                         level.board()));
    level.enemies()->push_back(new Ghost( 0,  8, &ghostTexture, D, 1,
                                         level.board()));
    level.enemies()->push_back(new Ghost( 8, 11, &ghostTexture, L, 1,
                                         level.board()));
    level.enemies()->push_back(new Ghost(11,  8, &ghostTexture, D, 1,
                                         level.board()));
    
    level.enemies()->push_back(new Ghost( 3, 0, &ghostTexture, R, 1,
                                         level.board()));
    level.enemies()->push_back(new Ghost( 0, 2, &ghostTexture, U, 1,
                                         level.board()));
    level.enemies()->push_back(new Ghost( 8, 0, &ghostTexture, L, 1,
                                         level.board()));
    level.enemies()->push_back(new Ghost(11, 2, &ghostTexture, U, 1,
                                         level.board()));
    
    return level.runLevel();
}

int guardLevel(SDL_Renderer* renderer, Character* playerCharacter) {
    //CREATE LEVEL
    Level level = Level(5, renderer, playerCharacter,
                        "Images/scene/background/dungeon.png",
                        "Images/scene/doors/dungeon.png", "");
    
    Texture iconSheet = Texture(level.renderer(), 
                                "Images/icons/icons.png");
    Icon potionTexture = Icon(&iconSheet, 0);
    
    Texture gemKeyTexture = Texture(level.renderer(), 
                                    "Images/icons/gem.png",
                                    100, 150, 6, 7);
    level.setGem(new GemKey(6, 6, &gemKeyTexture, level.board()));
    
    //SET SETPIECES
    Texture columnTexture = Texture(level.renderer(),
                                    "Images/scene/prop/columns/tall.png");
    level.setPieces()->push_back(new Sprite(3, 3, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(3, 8, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(8, 3, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    level.setPieces()->push_back(new Sprite(8, 8, &columnTexture,
                                            level.board(), ScreenPoint(0, 40)));
    
    Texture lstocksTexture = Texture(level.renderer(),
                                    "Images/scene/prop/dungeon/lStocks.png");
    level.setPieces()->push_back(new Sprite(1, 11, &lstocksTexture,
                                            level.board()));
    
    Texture rstocksTexture = Texture(level.renderer(),
                                    "Images/scene/prop/dungeon/rStocks.png");
    level.setPieces()->push_back(new Sprite(10, 11, &rstocksTexture,
                                            level.board()));
    
    Texture lbarrelTexture = Texture(level.renderer(),
                                    "Images/scene/prop/dungeon/lBarrel.png");
    level.setPieces()->push_back(new Sprite(0, 0, &lbarrelTexture,
                                            level.board()));
    Texture rbarrelTexture = Texture(level.renderer(),
                                    "Images/scene/prop/dungeon/rBarrel.png");
    level.setPieces()->push_back(new Sprite(11, 0, &rbarrelTexture,
                                            level.board()));
    
    
        
    //SET ENEMIES
    Texture guardTexture = Texture(level.renderer(),
                                   "Images/enemies/guard.png",
                                   150, 200, 6, 1);
    level.enemies()->push_back(new Guard( 3,  4, &guardTexture, U, 1,
                                         level.board(), &potionTexture));
    level.enemies()->push_back(new Guard( 7,  3, &guardTexture, L, 2,
                                         level.board()));
    level.enemies()->push_back(new Guard( 8,  7, &guardTexture, D, 1,
                                         level.board(), &potionTexture));
    level.enemies()->push_back(new Guard( 4,  8, &guardTexture, R, 2,
                                         level.board()));
    level.enemies()->push_back(new Guard( 1,  1, &guardTexture, R, 1,
                                         level.board()));
    level.enemies()->push_back(new Guard(10,  1, &guardTexture, U, 2,
                                         level.board()));
    level.enemies()->push_back(new Guard(10, 10, &guardTexture, L, 1,
                                         level.board()));
    level.enemies()->push_back(new Guard( 1, 10, &guardTexture, D, 2,
                                         level.board()));
    
    return level.runLevel();
}



