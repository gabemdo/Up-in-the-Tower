//
//  character.cpp
//  Up in the Tower
//
//  Created by Gabe on 10/28/20.
//

#include "character.hpp"

Character::Character(SDL_Renderer* renderer, int skin, int hair, bool female,
                     std::string name, Texture* detail,
                     Texture* skinTexture, Texture* hairTexture) :
MovingSprite(skinTexture), _skin{skin}, _hair{hair}, _female{female},
_name{name}, _detail{detail}, _hairTexture{hairTexture}, _renderer{renderer} {
    changeHeading(D);
    IDTAG = character;
    _frameCount = 10;
}

void Character::setBoard(Board* board) {
    _board = board;
    if (_attack != NULL) {
        _attack->setBoard(board);
    }
}

void Character::reset(BoardPoint boardPosition) {
    _boardPosition = boardPosition;
    localize();
    _attack->setBoard(_board);
}

void Character::reinitialize() {
    _attacking = 0;
    _HP = 4;
    _frame = -1;
    _endFrame = 0;
    _ready = true;
    _positionCalculated = false;
}

void Character::setAttack(MovingSprite* attack) {
    _attack = attack;
    _attack->setBoard(_board);
}

int Character::currentFrame(int time) {
    int fr =  int(((40/_speed) + time -_endFrame)/(4/_speed));
    return 10*((int) _heading)+fr;
}

void Character::render(int time) {
    if (_HP == 0) {
        if (time >= _endFrame) {
            _endFrame = time + 225;
        }
        if (time < _endFrame) {
            _texture->opacityRender(Uint8((_endFrame - time) * 255.f/225.f),
                                    _renderPosition,
                                    10*(int(_heading)), _scale);
            if (_hairTexture != NULL) {
                _hairTexture->opacityRender(Uint8((_endFrame-time)*255.f/225.f),
                                            _renderPosition,
                                            10*(int(_heading)), _scale);
            }
        }
    } else if (_ready) {
        _texture->render(_renderPosition, 10*((int) _heading), _scale);
        if (_hairTexture != NULL) {
            _hairTexture->render(_renderPosition, 10*((int) _heading), _scale);
        }
    } else {
        _texture->render(_renderPosition, currentFrame(time), _scale);
        if (_hairTexture != NULL) {
            _hairTexture->render(_renderPosition, currentFrame(time), _scale);
        }
        
    }
}

void Character::renderHP(int time) {
    _HPTexture->render(ScreenPoint(850, 10), (4-_HP)*6 + (time/6)%6, 0.75f);
}

void Character::renderPauseMenu() {
    if (_pauseMenuTexture == NULL) {
        std::cout << "ERROR: PAUSE MENU TEXTURE HAS NOT BEEN SET." << std::endl;
    } else {
        _pauseMenuTexture->render(ScreenPoint(200, 100));
    }
}

void Character::renderDeath() {
    int frame = _frame;
    if (0 <= frame && frame <= 30) {
        //Fade on Character
        _detail->setOpacity(Uint8((frame/30.f)*255.f));
        _detail->render(ScreenPoint(300, 150), _skin);
        _detail->render(ScreenPoint(300, 150), 6 + _hair);
    } else if (frame <= 42) {
        //Grow Teardrop
        _detail->setOpacity(255);
        _detail->render(ScreenPoint(300, 150), _skin);
        _detail->render(ScreenPoint(300, 150), 6 + _hair);
        _tear->render(ScreenPoint(640, 475), (frame - 31)/2, 0.3f);
    } else if (frame <= 120) {
        //Drop Teardrop
        _detail->render(ScreenPoint(300, 150),_skin);
        _detail->render(ScreenPoint(300, 150),6+_hair);
        _tear->render(ScreenPoint(640, 475 + (frame - 43)*4 ), 5, 0.3f);
    } else if (frame <= 151) {
        //Fade on Message
        _detail->render(ScreenPoint(300, 150), _skin);
        _detail->render(ScreenPoint(300, 150), 6 + _hair);
        _death->setOpacity(Uint8(((frame - 121.f)/30.f)*255.f));
        _death->render(ScreenPoint(300, 0), 0);
    } else {
        //Pause
        _detail->render(ScreenPoint(300, 150), _skin);
        _detail->render(ScreenPoint(300, 150), 6 + _hair);
        _death->render(ScreenPoint(300, 0), 0);
    }
}

void Character::renderWin() {
    int frame = _frame;
    if (0 <= frame && frame <= 30) {
        float scale = frame/30.f;
        _boom->render(ScreenPoint(600 - int(300*scale),
                                  500 - int(300*scale)),
                      0, scale);
    } else if (30 < frame && frame <= 45) {
        _boom->setOpacity(Uint8(255*(45 - frame)/15.f));
        _boom->render(ScreenPoint(300, 200), 0);
        _boom->setOpacity(255);
    }
    
    if (15 < frame && frame <= 45) {
        float scale = (frame - 15)/30.f;
        _detail->render(ScreenPoint(600 - int(300*scale),
                                    500 - int(300*scale)),
                        _skin, scale);
        _detail->render(ScreenPoint(600 - int(300*scale),
                                    500 - int(300*scale)),
                        6+_hair, scale);
        _smile->render(ScreenPoint(500 + 100 - int(100*scale),
                                   575 - 75 + int(75*scale)),
                       2, scale);
    } else if (45 < frame) {
        _detail->render(ScreenPoint(300, 200), _skin);
        _detail->render(ScreenPoint(300, 200), 6 + _hair);
        _smile->render(ScreenPoint(500, 575), 2);
    }
    
    if (45 < frame && frame <= 75) {
        _win->setOpacity(Uint8(255*(frame - 45)/30.f));
        _win->render(ScreenPoint(300, 0), 0);
        _win->setOpacity(255);
    } else if (75 < frame) {
        _win->render(ScreenPoint(300, 0), 0);
    }
}

void Character::scanMove(const Uint8* currentKeyStates, int time) {
    //Change Name, this function scans move if ready and steps if not.
    if (_ready) {
        if (currentKeyStates[SDL_SCANCODE_SPACE] && _attacking == 0) {
            attack(time);
        } else if (currentKeyStates[SDL_SCANCODE_UP] ||
                   currentKeyStates[SDL_SCANCODE_W]) {
            startmove(U, time);
        } else if(currentKeyStates[SDL_SCANCODE_DOWN] ||
                  currentKeyStates[SDL_SCANCODE_S]) {
            startmove(D, time);
        } else if(currentKeyStates[SDL_SCANCODE_LEFT] ||
                  currentKeyStates[SDL_SCANCODE_A]) {
            startmove(L, time);
        } else if(currentKeyStates[ SDL_SCANCODE_RIGHT] ||
                  currentKeyStates[ SDL_SCANCODE_D]) {
            startmove(R, time);
        }
    } else {
        step(time);
    }
    
    _attack->takeTurn(time);
}


void Character::attack(int time) {
    switch (_board->id(_boardPosition,_heading)) {
        case openSquare:
            _attack->activate(_boardPosition, _heading, time);
            break;
        default:
            break;
    }
}

bool Character::startmove(Direction d, int time) {
    checkPosition();
    _heading = d;
    if (_board != NULL) {
        BoardPoint newPosition = moveOne(_boardPosition, d);
        switch (_board->id(newPosition)) {
            case levelChange: {
                if (!_board->isWon()) {
                    return false;
                }
                _board->set(_board->get(&_boardPosition), newPosition);
                _board->clear(_boardPosition);
                _boardPosition = newPosition;
                
                _ready = false;
                _endFrame = time + (40/_speed) - 2;
                step(time);
                return true;
                break;
            }
            case item: {
                drinkPotion();
                
                _board->get(&newPosition)->steppedOn();
                
                _board->set(_board->get(&_boardPosition), newPosition);
                _board->clear(_boardPosition);
                _boardPosition = newPosition;
                
                _ready = false;
                _endFrame = time + 2 * (20/_speed) - 2;
                step(time);
                return true;
                break;
            }
            case key: {
                //FOR ICON STUFF have icon count increase???
                if (_board->get(&newPosition)->steppedOn()) {
                    _board->won();
                }
                _board->set(_board->get(&_boardPosition), newPosition);
                _board->clear(_boardPosition);
                _boardPosition = newPosition;
                
                _ready = false;
                _endFrame = time + 2 * (20/_speed) - 2;
                step(time);
                return true;
                break;
            }
            case door: {
                if (!_board->isWon()) {
                    return false;
                }
                _board->walkOver(_boardPosition, d);
            }
            case openSquare: {
                _board->set(_board->get(&_boardPosition), newPosition);
                _board->clear(_boardPosition);
                _boardPosition = newPosition;
                
                _ready = false;
                _endFrame = time + 2 * (20/_speed) - 2;
                step(time);
                return true;
            }
            default:
                return false;
        }
        if (_board->move(_boardPosition, d)) {
            _ready = false;
            _endFrame = time + 2 * (20/_speed) - 2;
            step(time);
            return true;
        }
    } else {
        std::cout << "MovingSprite has not been assigned a board." << std::endl;
    }
    return false;
}

bool Character::step(int time) {
    switch (_heading) {
        case U:
            _abstractPosition.dy(7 * _speed);
            break;
        case R:
            _abstractPosition.dx(7 * _speed);
            break;
        case L:
            _abstractPosition.dx(-7 * _speed);
            break;
        case D:
            _abstractPosition.dy(-7 * _speed);
            break;
        default:
            break;
    }
    calculatePosition();

    if (_endFrame <= time) {
        _board->started();
        _ready = true;
    }
    return _ready;
}



bool Character::steppedOn() {
    if (_HP > 0) {
        _HP--;
    }
    if (_HP == 0) {
        die();
        return true;
    }
    return false;
}

void Character::die() {
    if (_frame < 0) {
        _endFrame = 0;
        _frame = 0;
    } else {
        ++_frame;
    }
}

void Character::win()//Unused??
{
    die();
}



PrinceS::PrinceS(SDL_Renderer* renderer, int skin, int hair, bool female,
                 std::string name, Texture* detail,
                 Texture* skinTexture, Texture* hairTexture) :
Character(renderer, skin, hair, female, name,
          detail, skinTexture, hairTexture) {
    _isPrinceS = true;
    _startPosition = BoardPoint(-1, 6);
    _HPTexture = new Texture(_renderer, "Images/icons/hp.png",
                             100, 150, 6, 5);
    _attackTexture = new Texture(_renderer, "Images/icons/forceBeam.png",
                                 100, 100, 6, 1);
    _attack = new Attack(0, 0, _attackTexture);
    
    _pauseMenuTexture = new Texture(_renderer, "Images/chr/PrinceS/menus/pause.png");
    
    _characterIconSheet = new Texture(_renderer, "Images/chr/PrinceS/menus/icons.png");
    _tear  = new Icon(_characterIconSheet, 1);
    _death = new Icon(_characterIconSheet, 2, 600, 150, 1);
    _win   = new Icon(_characterIconSheet, 3, 600, 150, 1);
    _smile = new Icon(_characterIconSheet, 8, 200, 150, 3);
    _boom  = new Icon(_characterIconSheet, 1, 600, 600, 1);
}

PrinceS::~PrinceS() {
    delete _HPTexture;
    delete _attackTexture;
    delete _attack;
    delete _pauseMenuTexture;
    delete _characterIconSheet;
    delete _tear;
    delete _death;
    delete _win;
    delete _smile;
    delete _boom;
    //delete _detail;
}



Knight::Knight(SDL_Renderer* renderer, int skin, int hair, bool female,
               std::string name, Texture* detail, Texture* skinTexture) :
Character(renderer, skin, hair, female, name, detail, skinTexture, NULL) {
    _isPrinceS = false;
    _speed = 1;
    _startPosition = BoardPoint(12, 6);
    _HPTexture = new Texture(_renderer, "Images/icons/hp.png",
                             100, 150, 6, 5);
    _attackTexture = new Texture(_renderer, "Images/icons/forceBeam.png",
                                 100, 100, 6, 1);
    _attack = new Attack(0, 0, _attackTexture);
    
    _pauseMenuTexture = new Texture(_renderer,
                                    "Images/chr/Knight/menus/pause.png");
    
    _characterIconSheet = new Texture(_renderer,
                                      "Images/chr/Knight/menus/icons.png");
    _tear  = new Icon(_characterIconSheet, 1);
    _death = new Icon(_characterIconSheet, 2, 600, 150, 1);
    _win   = new Icon(_characterIconSheet, 3, 600, 150, 1);
    _smile = new Icon(_characterIconSheet, 8, 200, 150, 3);
    _boom  = new Icon(_characterIconSheet, 1, 600, 600, 1);
}

Knight::~Knight() {
    delete _HPTexture;
    delete _attackTexture;
    delete _attack;
    delete _pauseMenuTexture;
    delete _characterIconSheet;
    delete _tear;
    delete _death;
    delete _win;
    delete _smile;
    delete _boom;
    //delete _detail;
}
