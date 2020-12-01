//
//  effects.cpp
//  Up in the Tower
//
//  Created by Gabe on 11/1/20.
//

#include "effects.hpp"



Potion::Potion(int m, int n, Graphic* texture, Board* board) :
Sprite(m,n, texture, board) {
        IDTAG = item;
}

int Potion::currentFrame(int time) {
    return (time/5)%6;
}

bool Potion::steppedOn() {
    clear();
    return true;
}

void Potion::clear() {
    _board->clear(_boardPosition);
}



Attack::Attack(int m, int n, Texture* texture) : MovingSprite(m,n,texture) {
    IDTAG = magic;
}

Attack::Attack(int m, int n, Texture* texture, Direction d, int speed,
               Board* board) : MovingSprite(m,n,texture,d,speed,board) {
    IDTAG = magic;
}

void Attack::render(int time) {
    if (_active) {
        calculatePosition();
        _texture->render(_renderPosition, (time/3)%4, _scale);
    }
}

void Attack::activate(BoardPoint position, Direction heading, int time) {
    if (_active) {
        return;
    }
    _active = true;
    _ready = false;
    _endFrame = time + 2*(20/_speed)-4;
    _heading = heading;
    _boardPosition = position;
    localize();
    step(time);
    _boardPosition = moveOne(position,heading);
    _board->set(this, _boardPosition);
}

bool Attack::takeTurn(int time) {
    if (_ready && _endFrame != 0) {
        if (_active)
        {
            switch (_board->id(_boardPosition,_heading))
            {
                case openSquare:
                    _endFrame = time + 2*(20/_speed) - 2;
                    startmove(_heading, time);
                    break;
                case wall:
                    clear();
                    break;
                case enemy:
                    //kill enemy
                    _board->get(_boardPosition,_heading)->steppedOn();
                    clear();
                    break;
                default:
                    clear();
                    break;
            }
        }
    }
    else if (time <= _endFrame )
    {
        step(time);
    }
    return _active;
}

bool Attack::step(int time)
{
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

bool Attack::startmove(Direction d, int time) {
    _ready = false;
    _active = false;
    calculatePosition();
    _heading = d;
    if (_board->move(_boardPosition, d)) {
        _active = true;
        step(time);
        return true;
    }
    return false;
}

void Attack::clear() {
    if (_active) {
        _board->clear(_boardPosition);
    }
    _ready = true;
    _active = false;
    _endFrame = -1;
}



GemKey::GemKey(int m, int n, Graphic* texture, Board* board) :
    Sprite(m,n, texture, board) {
        IDTAG = key;
}

int GemKey::currentFrame(int time) {
    return ((_gemCount%6)*6) + (time/5)%6;
}


void GemKey::clear() {
    _board->clear(_boardPosition);
}

bool GemKey::steppedOn() {
    clear();
    return collectOne();
}

bool GemKey::collectOne() {
    ++_gemCount;
    if ( _gemCount >= 6 ) {
        return true;
    }
    _boardPosition = generateSpot();
    localize();
    _board->set(this,_boardPosition);
    return false;
}

BoardPoint GemKey::generateSpot() {
    int m=0, n=0;
    int charm = _board->character()->m();
    int charn = _board->character()->n();
    
    bool validSpot = false;
    while (!validSpot) {
       /*
        Check that randomly generated spot is not within a row or column of the
        characters present location, and that the spot is available.
        */
        m = (std::rand() % 12);
        if (-3 < (m - charm) && (m - charm) < 3) continue;
        n = (std::rand() % 12);
        if (-3 < (n - charn) && (n - charn) < 3) continue;
        if (_board->open(BoardPoint(m,n))) validSpot = true;
    }
    return BoardPoint(m,n);
}

void Laser::activate(BoardPoint boardPosition, Direction d, int time) {
    //_active = true;
    //_endFrame = time + _duration;
    _boardPosition = boardPosition;
    _heading = d;
    ScreenPoint center;
    switch (_heading) {
        case R:
            center = ScreenPoint(50,200);
            break;
        case U:
            center = ScreenPoint(70,200);
            break;
        case L:
            center = ScreenPoint(210,200);
            break;
        default:
            center = ScreenPoint(70,200);
    }
    _abstractPosition = InterPoint(_boardPosition);
    calculatePosition(center);
}

//DEPRECATE
bool Laser::takeTurn(int time) {
    return _active;
}

void Laser::backRender(int time) {
    switch (_heading) {
        case R:
            _texture->render(_renderPosition, Size(260, 320),
                             7*((time/3)%2),  _scale);
            break;
        case U:
            _texture->render(_renderPosition, Size(140,320),
                             7*((time/3)%2) + 1, _scale);
            break;
        case L:
            _texture->render(_renderPosition, Size(260, 320),
                             7*((time/3)%2) + 2, _scale);
            break;
        default:
            break;
    }
}

void Laser::foreRender(int time) {
    if (_heading == D) {
        _texture->render(_renderPosition, Size(140,320),
                         7*((time/3)%2) + m()/3 + 3, _scale);
    }
}
