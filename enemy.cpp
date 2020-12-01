//
//  enemy.cpp
//  Up in the Tower
//
//  Created by Gabe on 11/2/20.
//

#include "enemy.hpp"
#include "character.hpp"

Enemy::Enemy(int m, int n, Texture* enemyTexture) :
MovingSprite(m, n, enemyTexture) {
    IDTAG = enemy;
}

Enemy::Enemy(int m, int n, Texture* texture, Direction d,
             int speed, Board* board) :
MovingSprite(m, n, texture, d, speed, board) {
    IDTAG = enemy;
}

void Enemy::render(int time) {
    if (_alive) {
        MovingSprite::render(time);
    }
}

void Enemy::renderIcon(int time, ScreenPoint screenPosition) {
    int clip = ((_alive? 4: 3) * _frameCount) + (time/4) % _frameCount;
    _texture->render(screenPosition, clip, 0.5f);
}


bool Enemy::takeTurn(int time) {
    if (_alive) {
        if (_ready) {
            switch (_board->id(_boardPosition, _heading)) {
                case openSquare:
                    startmove(_heading, time);
                    break;
                case character:
                    _board->character()->steppedOn();
                    //Fall through to new heading.
                default:
                    int newHeading = std::rand() % 3;
                    if (newHeading >= (int) _heading) ++newHeading;
                    _heading = (Direction) newHeading;
                    break;
            }
        } else {
            step(time);
        }
    }
    return _alive;
}

bool Enemy::startmove(Direction d, int time) {
    checkPosition();
    _heading = d;
    if (_board != NULL) {
        if (_board->move(_boardPosition, d)) {
            _ready = false;
            _endFrame = time + 2*(20/_speed) - 2;
            this->step(time);
            return true;
        }
    } else {
        std::cout << "MovingSprite has not been assigned a board." << std::endl;
    }
    return false;
}

bool Enemy::steppedOn() {
    if (_HP > 0) {
        _HP--;
    }
    if (_HP == 0) {
        _alive = false;
        _board->clear(_boardPosition);
        deathBehavior();
    }
    return !_alive;
}



Guard::Guard(int m, int n, Texture* texture,
             Direction d, int speed, Board* board) :
Enemy(m, n, texture, d, speed, board) {
    IDTAG = enemy;
    _HP = 2;
    _frameCount = 10;
}

Guard::Guard(int m, int n, Texture* texture,
             Direction d, int speed, Board* board, Graphic* replacementIcon) :
Enemy(m, n, texture, d, speed, board) {
    IDTAG = enemy;
    _HP = 2;
    _replacementIcon = replacementIcon;
    _frameCount = 10;
}

int Guard::currentFrame(int time) {
    return (time/5)%6;
}

bool Guard::takeTurn(int time) {
    if (_alive) {
        if (_ready) {
            BoardPoint inFront = moveOne(_boardPosition,_heading);
            switch (_board->id(inFront)) {
                case character:
                    _board->character()->steppedOn();
                case openSquare:
                    if (!isEdge(inFront)) {
                        startmove(_heading, time);
                        break;
                    } //Fall through to pick new direction if edge in front
                default:
                    int newHeading = std::rand() % 3;
                    if (newHeading >= (int) _heading) ++newHeading;
                    _heading = (Direction) newHeading;
                    break;
            }
        } else {
            step(time);
        }
    }
    return _alive;
}

bool Guard::step(int time) {
    switch (_heading) {
        case U:
            _abstractPosition.dy(7*_speed);
            break;
        case R:
            _abstractPosition.dx(7*_speed);
            break;
        case L:
            _abstractPosition.dx(-7*_speed);
            break;
        case D:
            _abstractPosition.dy(-7*_speed);
            break;
        default:
            break;
    }
    calculatePosition();
    if (_endFrame <= time) _ready = true;
    return _ready;
}

void Guard::deathBehavior() {
    if (_replacementIcon != NULL) {
        _replacement = new Potion(_boardPosition.x(), _boardPosition.y(),
                                  _replacementIcon, _board);
    }
}



Ghost::Ghost(int m, int n, Texture* texture,
             Direction d, int speed, Board* board) : Enemy(m, n, texture) {
    _board = board;
    _board->setGhost(this, &_boardPosition);
    _heading = d;
    _speed = speed;
    IDTAG = ghost;
    _HP = 2;
}

void Ghost::ghostRender(int time) {
    if (!_positionCalculated) {
        calculatePosition();
    }
    _texture->render(_renderPosition, currentFrame(time), _scale);
}

bool Ghost::takeTurn(int time) {
    if (_alive) {
        if (_ready) {
            bool newDirection = false;
            switch (_board->id(_boardPosition, _heading)) {
                case wall:
                case door:
                case levelChange:
                case ghost:
                    newDirection = true;
                    break;
                case character:
                    _board->character()->steppedOn();
                    startmove(_heading, time);
                    return _alive;
                default: {
                    int randomChange = std::rand() % 4;
                    if (randomChange < 1) {
                        newDirection = true;
                    } else {
                        startmove(_heading, time);
                        return _alive;
                    }
                }
            }
            if (newDirection) {
                int newHeading = std::rand() % 3;
                if (newHeading >= (int) _heading) ++newHeading;

                _heading = (Direction) newHeading;
            }
        } else {
            step(time);
        }
    }
    return _alive;
}

bool Ghost::startmove(Direction d, int time) {
    checkPosition();
    _heading = d;
    
    if (_board != NULL) {
        if (_board->id(_boardPosition)==ghost) {
            _board->clear(_boardPosition);
        }

        BoardPoint newPosition = moveOne(_boardPosition,d);
        
        if (_board->open(newPosition)) {
            _board->set(this, newPosition);
        }
        
        _boardPosition = newPosition;
        _ready = false;
        _endFrame = time + 2*(20/_speed) - 2;
        step(time);
        return true;
    } else {
        std::cout << "MovingSprite has not been assigned a board." << std::endl;
    }
    return false;
}

bool Ghost::step(int time) {
    switch (_heading) {
        case U:
            _abstractPosition.dy(7*_speed);
            break;
        case R:
            _abstractPosition.dx(7*_speed);
            break;
        case L:
            _abstractPosition.dx(-7*_speed);
            break;
        case D:
            _abstractPosition.dy(-7*_speed);
            break;
        default:
            break;
    }
    calculatePosition();
    if (_endFrame <= time) {
        _ready = true;
    }
    return _ready;
}

void Ghost::deathBehavior() {
    if (_replacementIcon != NULL) {
        _replacement = new Potion(_boardPosition.x(), _boardPosition.y(),
                                  _replacementIcon, _board);
    }
}


Sentry::Sentry(int m, int n, Texture* sentryTexture, Texture* laserTexture,
               Direction d, int speed, Board* board) :
Enemy(m, n, sentryTexture, d, speed, board) {
    IDTAG = enemy;
    _HP = 1;
    _laser = new Laser(laserTexture);
    _frameCount = 1;
}

Sentry::Sentry(int m, int n, Texture* sentryTexture, Texture* laserTexture,
               Direction d, int speed, Board* board, Graphic* replacementIcon) :
Enemy(m, n, sentryTexture, d, speed, board) {
    IDTAG = enemy;
    _HP = 1;
    _laser = new Laser(laserTexture);
    _replacementIcon = replacementIcon;
    _frameCount = 1;
}

void Sentry::render(int time) {
    if (!_positionCalculated) calculatePosition();
    
    if (_state & ATTACKING) _laser->backRender(time);
    _texture->render(_renderPosition, currentFrame(time), _scale);
    if (_state & ATTACKING) _laser->foreRender(time);
}

bool Sentry::takeTurn(int time) {
   
    if (_alive) {
        //clear state if time has passed the _endFrame for the state
        if (_state && (time >= _endFrame)) {
            if (_state & DYING) _alive = false;
            _state = READY;
        }
        
        //IF in a STATE
        //if (_state & WAITING) break;
        if (_state & MOVING) step(time);
        //if (_state & TURNING) break;
        //if (_state & ATTACKING) break; //render will handle
        //if (_state & HURTING) break; //render will handle
        //if (_state & DYING) break; //render and end state will handle
        
        if (_state == READY) { //What should it do?
            
            BoardPoint inFront = moveOne(_boardPosition, _heading);
            
            if (_board->id(moveOne(inFront, _heading)) == character &&
                _board->open(inFront)) { //attack?
                
                //Extract to Function, FROM HERE
                if (_laser == NULL) {
                    std::cout << "No Laser assigned" << std::endl;
                } else {
                    _laser->activate(_boardPosition, _heading, time);
                    if (!_lasing) {
                        _board->character()->steppedOn();
                        _state = ATTACKING;
                        _endFrame = time + 15;
                    }
                }
                //Extract to Function, TO HERE
                
            } else if (scanForCharacter()) { //turn toward character?
                
                _state = TURNING;
                _endFrame = time + 20;
                
            } else if (_board->open(inFront)) { //step forard?
                
                startmove(_heading, time);
                _state = MOVING;
                
            } else { //try turning?
                
                int newHeading = std::rand() % 3;
                if (newHeading >= (int) _heading) ++newHeading;
                _heading = (Direction) newHeading;
                _state = TURNING;
                _endFrame = time + 20;
                
            }
        }
    }
    return _alive;
}

bool Sentry::scanForCharacter() { //returns if direction changes
    bool sameX, sameY;
    sameX = (_boardPosition.x() == _board->character()->m());
    sameY = (_boardPosition.y() == _board->character()->n());
    if (sameX) {
        if (_boardPosition.y() > _board->character()->n()) {
            if (_heading != D) {
                _heading = D;
                return true;
            }
            return false;
        } else {
            if (_heading != U) {
                _heading = U;
                return true;
            }
            return false;
        }
    }
    if (sameY) {
        if (_board->character()->m() < 0 || _board->character()->m() > 11) {
            return false;
        } else if (_boardPosition.x() > _board->character()->m()) {
            if (_heading != L) {
                _heading = L;
                return true;
            }
            return false;
        } else {
            if (_heading !=R) {
                _heading = R;
                return true;
            }
            return false;
        }
    }
    return false;
}

bool Sentry::step(int time) {
    _laser->deactivate();
    _lasing = false;
    switch (_heading) {
        case U:
            _abstractPosition.dy(7*_speed);
            break;
        case R:
            _abstractPosition.dx(7*_speed);
            break;
        case L:
            _abstractPosition.dx(-7*_speed);
            break;
        case D:
            _abstractPosition.dy(-7*_speed);
            break;
        default:
            break;
    }
    calculatePosition();
    if (_endFrame <= time)
    {
        _ready = true;
    }
    return _ready;
}

void Sentry::deathBehavior() {
    if (_replacementIcon != NULL) {
        _replacement = new Potion(_boardPosition.x(), _boardPosition.y(),
                                  _replacementIcon, _board);
    }
}



Ogre::Ogre(int m, int n, Texture* texture,
           Direction d, int speed, Board* board) :
Enemy(m, n, texture, d, speed, board) {
    IDTAG = enemy;
    _HP = 4;
    _frameCount = 10;
}

Ogre::Ogre(int m, int n, Texture* texture, Direction d, int speed, Board* board,
           Graphic* replacementIcon) : Enemy(m, n, texture, d, speed, board) {
    IDTAG = enemy;
    _HP = 4;
    _replacementIcon = replacementIcon;
    _frameCount = 10;
}

Direction Ogre::newDirection() {
    bool openUL, openUR, openLL, openLR;
    bool openU, openR, openL, openD;
    
    openUR = _board->openRef(_boardPosition,UR);
    openUL = _board->openRef(_boardPosition,UL);
    openLL = _board->openRef(_boardPosition,LL);
    openLR = _board->openRef(_boardPosition,LR);
    
    openR = openU = openL = openD = false;
    
    if (openLR && openUR && _board->openRef(_boardPosition,R)) {
        openR = true;
    }
    if (openUL && openUR && _board->openRef(_boardPosition,U)) {
        openU = true;
    }
    if (openLL && openUL && _board->openRef(_boardPosition,L)) {
        openL = true;
    }
    if (openLL && openLR && _board->openRef(_boardPosition,D)) {
        openD = true;
    }
    
    if (!(openR || openU || openL || openD)) {
        openR = _board->openRef(_boardPosition,R);
        openU = _board->openRef(_boardPosition,U);
        openL = _board->openRef(_boardPosition,L);
        openD = _board->openRef(_boardPosition,D);
    }
    
    int totalOpenings = (openR?1:0) + (openU?1:0) + (openL?1:0) + (openD?1:0);
    int direction;
    
    if (totalOpenings == 0) {
        std::cout << "ERROR!! NO OGRE DIRECTION FOUND!" << std::endl;
        //TODO: make it jump somewhere if there is no direction
    } else if (totalOpenings == 1) {
        if (openR) return R;
        if (openU) return U;
        if (openL) return L;
        if (openD) return D;
    } else while (true) {
        direction = (Direction) (std::rand() % 4);
        switch (direction) {
            case 0:
                if (openR) return R;
                break;
            case 1:
                if (openU) return U;
                break;
            case 2:
                if (openL) return L;
                break;
            case 3:
                if (openD) return D;
                break;
        }
    }
    return R;
}

bool Ogre::takeTurn(int time) {
    bool changeDirection = false;
    
    if (_alive) {
        if (_ready) {
            switch (_board->id(diagonalLeft(_boardPosition,_heading))) {
                case openSquare:
                    break;
                case character:
                    _board->character()->steppedOn();
                    break;
                default:
                    changeDirection = true;
                    break;
            }
            
            switch (_board->id(diagonalRight(_boardPosition,_heading))) {
                case openSquare:
                    break;
                case character:
                    _board->character()->steppedOn();
                    break;
                default:
                    changeDirection = true;
                    break;
            }
            
            switch (_board->id(_boardPosition,_heading)) {
                case openSquare:
                    break;
                case character:
                    _board->character()->steppedOn();
                default:
                    changeDirection = true;
                    break;
            }
            
            if (changeDirection) {
                _heading = newDirection();
            }
            startmove(_heading, time);
        } else {
            step(time);
        }
    }
    return _alive;
}

bool Ogre::step(int time) {
    switch (_heading) {
        case U:
            _abstractPosition.dy(7*_speed);
            break;
        case R:
            _abstractPosition.dx(7*_speed);
            break;
        case L:
            _abstractPosition.dx(-7*_speed);
            break;
        case D:
            _abstractPosition.dy(-7*_speed);
            break;
        default:
            break;
    }
    calculatePosition();
    if (_endFrame <= time)  _ready = true;
    return _ready;
}

void Ogre::deathBehavior() {
    if (_replacementIcon != NULL) {
        _replacement = new Potion(_boardPosition.x(), _boardPosition.y(),
                                  _replacementIcon, _board);
    }
}



Gremlin::Gremlin(int m, int n, Texture* texture, Direction d, int speed,
                 Board* board) : Enemy(m,n,texture,d,speed,board) {
    IDTAG = enemy;
    _HP = 1;
    _frameCount = 10;
}

bool Gremlin::step(int time) {
    switch (_heading) {
        case U:
            _abstractPosition.dy(7*_speed);
            break;
        case R:
            _abstractPosition.dx(7*_speed);
            break;
        case L:
            _abstractPosition.dx(-7*_speed);
            break;
        case D:
            _abstractPosition.dy(-7*_speed);
            break;
        default:
            break;
    }
    calculatePosition();
    if (_endFrame <= time)  _ready = true;
    return _ready;
}
