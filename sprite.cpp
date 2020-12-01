//
//  sprite.cpp
//  Up in the Tower
//
//  Created by Gabe on 10/28/20.
//

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string.h>
#include <iostream>
#include "sprite.hpp"


void Graphic::render(ScreenPoint position, Size size,
                     int clip, float scale) {
    _size = size;
    render(position, clip, scale);
}

void Graphic::opacityRender(Uint8 opacity, ScreenPoint position,
                            int clip, float scale) {
    setOpacity(opacity);
    render(position, clip, scale);
    setOpacity(255);
}

Size Graphic::size() {
    //if (_size.trivial())  throw "No size assigned.";
    return _size;
}

Size Graphic::size(Size size) {
    _size = size;
    return _size;
}



Texture::Texture(SDL_Renderer* renderer, std::string path) :
    _renderer{renderer} {load(path);}

Texture::Texture(SDL_Renderer* renderer, std::string path,
                 int frameWidth, int frameHeight,
                 int xclip, int yclip) : _renderer{renderer} {
    load(path);
    for (int i = 0; i < yclip; i++) {
        for (int j = 0; j < xclip; j++) {
            setClip(frameWidth*j,frameHeight*i,frameWidth,frameHeight);
        }
    }
    _size = Size(frameWidth, frameHeight);
}

bool Texture::load(std::string path)
{
    free();
    
    SDL_Texture* replacement = NULL;
    SDL_Surface* loaded = IMG_Load(path.c_str());
    
    if (loaded == NULL) {
        std::cout << "Can't load " << path.c_str() << std::endl;
        std::cout << "Error: " << IMG_GetError() << std::endl;
    } else {
        replacement = SDL_CreateTextureFromSurface(_renderer, loaded);
        if (replacement == NULL) {
            std::cout << "Can't render " << path.c_str() << std::endl;
            std::cout << "Error: " << SDL_GetError() << std::endl;
        } else {
            _size = Size(loaded->w, loaded->h);
            SDL_FreeSurface(loaded);
        }
    }
    _texture = replacement;
    return _texture != NULL;
}

void Texture::free() {
    if (_texture != NULL) {
        SDL_DestroyTexture(_texture);
        _texture = NULL;
        _size.set(0,0);
    }
}

int Texture::setClip(int xp, int yp, int wp, int hp) {
    if ((xp >= 0) && (yp >= 0) && (wp > 0) && (hp > 0)) {
        _clips.push_back(new SDL_Rect);
        _clips.back()->x = xp;
        _clips.back()->y = yp;
        _clips.back()->w = wp;
        _clips.back()->h = hp;
        return _clipCount++;
    }
    return -1;
}

int Texture::setClip(int xp, int yp, int wp, int hp, SDL_Rect &clipFrame) {
    if ((xp >= 0) && (yp >= 0) && (wp > 0) && (hp > 0)) {
        clipFrame.x = xp;
        clipFrame.y = yp;
        clipFrame.w = wp;
        clipFrame.h = hp;
        
        _clips.push_back(&clipFrame);
        return _clipCount++;
    } else {
        return -1;
    }
}

SDL_Rect* Texture::getClip(int clip) {
    if (_texture == NULL) {
        std::cout << "No texture is loaded for this clip." << std::endl;
        return NULL;
    }
    if ((0 <= clip)&&(clip < _clipCount)) {
        return _clips[unsigned(long(clip))];
    } else {
        return NULL;
    }
}

void Texture::setLaserClips() {
    setClip(   0,   0, 260, 320);
    setClip( 260,   0, 140, 320);
    setClip( 400,   0, 260, 320);
    setClip( 660,   0, 140, 320);
    setClip( 800,   0, 140, 320);
    setClip( 940,   0, 140, 320);
    setClip(1080,   0, 140, 320);
    
    setClip(   0, 320, 260, 320);
    setClip( 260, 320, 140, 320);
    setClip( 400, 320, 260, 320);
    setClip( 660, 320, 140, 320);
    setClip( 800, 320, 140, 320);
    setClip( 940, 320, 140, 320);
    setClip(1080, 320, 140, 320);

    setClip(   0, 640, 260, 320);
    setClip( 260, 640, 140, 320);
    setClip( 400, 640, 260, 320);
    setClip( 660, 640, 140, 320);
    setClip( 800, 640, 140, 320);
    setClip( 940, 640, 140, 320);
    setClip(1080, 640, 140, 320);
}

void Texture::render(ScreenPoint position, int clip, float scale) {
    if (_texture == NULL) {
        std::cout << "No texture is loaded to render." << std::endl;
        return;
    }
    SDL_Rect renderFrame = {position.x(), position.y(),
        int(_size.x()*scale + 0.5), int(_size.y()*scale + 0.5)};
    SDL_RenderCopy(_renderer, _texture, getClip(clip), &renderFrame);
}

void Texture::render(ScreenPoint position, Size size,
                     int clip, float scale) {
    if (_texture == NULL) {
        std::cout << "No texture is loaded to render." << std::endl;
        return;
    }
    SDL_Rect renderFrame = {position.x(), position.y(),
        int(size.x()*scale + 0.5), int(size.y()*scale + 0.5)};
    SDL_RenderCopy(_renderer, _texture, getClip(clip), &renderFrame);
}

void Texture::render(SDL_Rect *clip, SDL_Rect *frame) {
    if (_texture == NULL) {
        std::cout << "No texture is loaded to render." << std::endl;
        return;
    }
    SDL_RenderCopy(_renderer, _texture, clip, frame);
}

void Texture::setOpacity(Uint8 opacity) {
    SDL_SetTextureAlphaMod(_texture, opacity);
}

void Texture::opacityRender(Uint8 opacity, ScreenPoint position,
                            int clip, float scale) {
    setOpacity(opacity);
    render(position, clip, scale);
    setOpacity(255);
}



Icon::Icon(Texture* texture, int rowNumber,
           int frameWidth, int frameHeight, int clipCount) :
_texture{texture}, _clipCount{clipCount},
_width{frameWidth}, _height{frameHeight}
{
    for (int i = 0; i < clipCount; i++) {
        _clips.push_back(new SDL_Rect);
        _clips.back()->x = frameWidth * i;
        _clips.back()->y = frameHeight * rowNumber;
        _clips.back()->w = frameWidth;
        _clips.back()->h = frameHeight;
    }
    _size = Size(frameWidth, frameHeight);
}

SDL_Rect* Icon::getClip(int clip) {
    if (0 <= clip && clip < _clipCount) {
        return _clips[unsigned(long(clip))];
    } else {
        return NULL;
    }
}

void Icon::render(ScreenPoint position, int clip, float scale) {
    SDL_Rect renderFrame = {position.x(), position.y(),
        int(_width*scale + 0.5), int(_height*scale + 0.5)};
    _texture->render(getClip(clip), &renderFrame);
}


Board::Board(SDL_Renderer* renderer, Sprite* character,
             std::string backgroundPath,
             std::string foregroundPath,
             bool ghostLayer) :
             _character{character},
             _ghostLayer{ghostLayer} {
    _background = new Texture(renderer,backgroundPath);
    if (!foregroundPath.empty()) {
        std::cout << "Foreground not empty" << std::endl;
        _foreground = new Texture(renderer,foregroundPath);
    }
}

Board::~Board() {
    if (_background != NULL) {
        delete _background;
        _background = NULL;
    }
    if (_foreground != NULL) {
        delete _foreground;
        _foreground = NULL;
    }
}

void Board::render(int time) {
    _background->render(ScreenPoint());
    
    int col;
    for (int row = 11; row >= 0; --row) {
        if (_ghostLayer) {
            for (DoubleSpot ghost : _ghosts) {
                if (ghost.position->y() == row) {
                    ghost.transparent->ghostRender(time);
                }
            }
        }
        if (row == 6) {
            //render right hall contents
            if (_rightDoor != NULL) {
                _rightDoor->render(time);
            }
            if (_leftDoor != NULL) {
                _leftDoor->render(time);
            }
        }
        for (int i = 0; i < 12; ++i) {
            //render the row in outside-first order
            col = ( i%2 == 0 ) ? 11 - (i/2) : (i - 1)/2;
            if (_board[12*row + col] != NULL) {
                _board[12*row + col]->render(time);
            }
        }
        if (row == 6) {
            if (_door != NULL) {
                if (isWon() || !isStarted()) {
                    _door->render(ScreenPoint(800,350),2);
                    _door->render(ScreenPoint(0,350),0);
                } else {
                    _door->render(ScreenPoint(800,350),5);
                    _door->render(ScreenPoint(0,350),3);
                }
            } else {
                //ERROR
            }
        }
    }
    if (_foreground != NULL) {
        _foreground->render(ScreenPoint());
    }
}

Sprite* Board::get(BoardPoint *boardPosition) {
    int arrayPosition = boardPosition->arrayPoint();
    
    if (0 <= arrayPosition && arrayPosition < 144) {
         return _board[arrayPosition];
    } else if (arrayPosition == -1) {
        return _leftDoor;
    } else if (arrayPosition == 144) {
        return _rightDoor;
    } else if (arrayPosition == -2 || arrayPosition == 145) {
        return NULL;
    } else {
        return NULL;
    }
}

Sprite* Board::get(BoardPoint boardPosition, Direction heading) {
    BoardPoint newPosition = moveOne(boardPosition,heading);
    Sprite* sprite = get(&newPosition);
    return sprite;
}

bool Board::set(Sprite* prop, BoardPoint boardPosition) {
    if (id(boardPosition) == levelChange) {
        return true;
    } else if (id(boardPosition) == door) {
        if (boardPosition.x() == -1) {
            _leftDoor = prop;
            return true;
        } else if (boardPosition.x() == 12) {
            _rightDoor = prop;
            return true;
        }
    } else if (open(boardPosition)) {
        int point = boardPosition.arrayPoint();
        if (0 <= point && point < 144) {
            _board[point] = prop;
        } else {
            std::cout << "BAD CALL IN BOARD::SET" << std::endl;
        }
        return true;
    }
    return false;
}

void Board::clear(BoardPoint &boardPosition) {
    int point = boardPosition.arrayPoint();
    if (point == -1) {
        _leftDoor = NULL;
    } else if (point == 144) {
        _rightDoor = NULL;
    } else if (0 <= point && point < 144) {
        _board[point] = NULL;
    } else {
        std::cout << "CLEAR BOARD POINT ERROR: (" <<
            boardPosition.x() << "," << boardPosition.y() << ")" << std::endl;
    }
}

ID Board::id(BoardPoint position) {
    if (position.y() == 6 && (position.x() == -1 || position.x() == 12)) {
        return door;
    }
    if (position.y() == 6 && (position.x() == -2 || position.x() == 13)) {
        return levelChange;
    }
    if (position.x() < 0 || position.x() >= 12 ||
        position.y() < 0 || position.y() >= 12) {
        return wall;
    }
    if (get(&position) == NULL) {
        return openSquare;
    }
    return get(&position)->id();
}

ID Board::id(BoardPoint &oldPosition, Direction h) {
    return id(moveOne(oldPosition,h));
}

bool Board::open(BoardPoint boardPosition) {
    return id(boardPosition) == openSquare;
}

bool Board::open(BoardPoint oldPosition, Direction h) {
    return open(moveOne(oldPosition, h));
}

bool Board::openRef(BoardPoint &oldPosition, Direction h) {
    return open(moveOne(oldPosition, h));
}

bool Board::move(BoardPoint &oldPosition, BoardPoint &newPosition) {
    if (open(oldPosition)) {
        set(get(&oldPosition), newPosition);
        clear(oldPosition);
        return true;
    }
    return false;
}

bool Board::move(BoardPoint &oldPosition, Direction h) {
    if (openRef(oldPosition,h)) {
        BoardPoint newPosition = moveOne(oldPosition,h);
        set(get(&oldPosition),newPosition);
        clear(oldPosition);
        oldPosition = newPosition;
        return true;
    }
    return false;
}

void Board::walkOver(BoardPoint boardPosition, Direction heading) {
    switch (id(boardPosition,heading)) {
        case openSquare:
        case door:
        case levelChange:
            break;
        default:
            get(boardPosition,heading)->steppedOn();
            break;
    }
}

bool Board::ghostAt(int arrayPoint) {
    for (DoubleSpot ghost : _ghosts) {
        if (ghost.position->arrayPoint() == arrayPoint) return true;
    }
    return false;
}

bool Board::ghostAt(BoardPoint position) {
    if (0 <= position.x() && position.x() < 12) {
        return ghostAt(position.arrayPoint());
    }
    return false;
}

bool Board::setGhost(Sprite* prop, BoardPoint* boardPosition) {
    _ghostLayer = true;
    _ghosts.push_back((DoubleSpot){boardPosition,prop});
    return _ghostLayer;
}



Sprite::Sprite(int m, int n, Graphic* texture, int frameCount) :
_boardPosition{BoardPoint(m,n)}, _abstractPosition{InterPoint(BoardPoint(m,n))},
_texture{texture}, _frameCount{frameCount} {
    
    IDTAG = physicalObject;
    _center = ScreenPoint(_texture->x()/2,_texture->y()-20);
    calculatePosition();
}

Sprite::Sprite(int m, int n, Graphic* texture, Board* board, int frameCount) :
_boardPosition{BoardPoint(m,n)}, _abstractPosition{InterPoint(BoardPoint(m,n))},
_texture{texture}, _frameCount{frameCount} {
    _board = board;
    board->set(this,BoardPoint(m,n));
    IDTAG = physicalObject;
    _center = ScreenPoint(texture->x()/2, texture->y()-20);
    calculatePosition();
}

Sprite::Sprite(int m, int n, Graphic* texture, Board* board, ScreenPoint center,
               int frameCount, int timeShift, int rate) :
_boardPosition{BoardPoint(m,n)}, _abstractPosition{InterPoint(BoardPoint(m,n))},
_texture{texture}, _frameCount{frameCount}, _shift{timeShift}, _rate{rate} {
    _board = board;
    board->set(this,BoardPoint(m,n));
    IDTAG = physicalObject;
    _center = ScreenPoint(texture->x()/2-center.x(), texture->y()-center.y());
    calculatePosition();
}

Sprite::Sprite(int m, int n, Board* board, int frameCount) :
_boardPosition{BoardPoint(m,n)}, _board{board}, _frameCount{frameCount} {
    board->set(this,_boardPosition);
}
    
void Sprite::calculatePosition(ScreenPoint center) {
    if (_texture->size().trivial()) return; //TODO: ERROR HANDLING
    
    //shift to the center of the square (+70,+70);
    int a = _abstractPosition.x() - 770;
    int b = 3290 - _abstractPosition.y();

    int scale = 3430 + _abstractPosition.y();
    /*Projection. This projection is based on a
        board:
            x in [-1, 1]
            y in [ 2, 4]
            z == 0
        projecting onto a screen:
            y == z, for all x
        through the point:
            (0, 0, 4);
      This board is divided into 12 by 12.
      Each of these squares is divided into a further 140.
      Steps of 10 on flats parts,
         and (7,7) on the diagonal are approximately the same length.
     */
    float unscaled_x = 480.f * (5*a - 7*center.x());
    //2400.f * (a - 840.f) - 3360.f * center.x();
    float unscaled_y = 160.f * (5*b - 21*center.y());
    //800.f * (3360.f - b) - 3360.f * center.y();
    
    int x = int(600.f + unscaled_x/scale + 0.5);
    int y = int(unscaled_y / scale + 0.5);
    
    _renderPosition.set(x,y);
    _scale = float(3360.f / scale);
    _positionCalculated = true;
}

void Sprite::calculatePosition() {
    int a = _abstractPosition.x() - 770;
    int b = 3290 - _abstractPosition.y();
    
    int scale = 3430 + _abstractPosition.y();
    
    float unscaled_x = 480.f * (5*a - 7*_center.x());
    float unscaled_y = 160.f * (5*b - 21*_center.y());
    
    int x = int(600.f + unscaled_x/scale + 0.5);
    int y = int(unscaled_y / scale + 0.5);
    
    _renderPosition.set(x,y);
    _scale = 3360.f / scale;
    _positionCalculated = true;
}

void Sprite::checkPosition() {
    _abstractPosition.set(_boardPosition);
}

void Sprite::localize() {
    _abstractPosition = InterPoint(_boardPosition);
    calculatePosition();
}

void Sprite::render(int time) {
    if (!_positionCalculated) {
        calculatePosition();
    }
    _texture->render(_renderPosition, currentFrame(time), _scale);
}

int Sprite::currentFrame(int time) {
    if (_frameCount==1) return -1;
    return ((time + _shift)/_rate) % _frameCount;
    
}


MovingSprite::MovingSprite(int m, int n, Texture* texture) :
    Sprite(m, n, texture) {
        IDTAG = movingObject;
        _frameCount = 6;
}

MovingSprite::MovingSprite(int m, int n, Texture* texture,
                           Direction d, int speed, Board* board) :
    Sprite(m, n, texture, board) {
        _heading = d;
        _speed = speed;
        IDTAG = movingObject;
        _frameCount = 6;
}

int MovingSprite::currentFrame(int time) {
    return _heading * _frameCount + (time/4)%_frameCount;
}

void MovingSprite::render(int time) {
    _texture->render(_renderPosition, currentFrame(time), _scale);
}

bool MovingSprite::startmove(Direction d, int time) {
    checkPosition();
    _heading = d;
    if (_board != NULL) {
        if (_board->move(_boardPosition, d)) {
            _ready = false;
            _endFrame = time + 2*(20/_speed) - 2;
            step(time);
            return true;
        }
    } else {
        std::cout << "MovingSprite has not been assigned a board." << std::endl;
    }
    return false;
}

bool MovingSprite::step(int time) {
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
