//
//  sprite.hpp
//  Up in the Tower
//
//  Created by Gabe on 10/28/20.
//

#ifndef sprite_hpp
#define sprite_hpp

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <iostream>
#include <vector>

#include "boardlogic.hpp"


class Graphic {
   /*
    This purely virtual class is the base class for Textures and Icons. Each
    sprite contains a pointer to a Graphic. Textures contain sprite sheets and
    clips to access various parts of the sprite sheet.
    
    When the Graphic that a Sprite points to is a Texture, the texture is
    assumed to have a sprite sheet with clips only for that sprite. Icons are
    for cases when a single sprite sheet contains images for different sprites.
    Icons contain a pointer to a Texture, but the clips for the icon pertain
    only to the sprite that points to it. In effect, Icon makes it possible to
    give different names to different parts of the same image file.
    */
  public:
    virtual ~Graphic() {};
    virtual void render(ScreenPoint position, int clip, float scale) = 0;
    virtual void render(ScreenPoint position, Size size, int clip, float scale);
    virtual void setOpacity(Uint8 opacity) = 0;
    virtual void opacityRender(Uint8 opacity, ScreenPoint position,
                               int clip = -1, float scale = 1.f);
    Size size();
    Size size(Size size);
    int x() {return _size.x();}
    int y() {return _size.y();}
    virtual void free() {return;}
    
  protected:
    Size _size = Size();
};

class Texture : public Graphic {
  //See Graphic for more details.
  public:
    Texture(SDL_Renderer* renderer) : _renderer{renderer} {}
    Texture(SDL_Renderer* renderer, std::string path);
    Texture(SDL_Renderer* renderer, std::string path,
            int frameWidth, int frameHeight,
            int xclip, int yclip);
    ~Texture() {free();}
    
    bool load(std::string path);
    void free() override;
    
    int setClip(int x, int y, int w, int h);
    int setClip(int x, int y, int w, int h, SDL_Rect &clipFrame);
    SDL_Rect* getClip(int clip);
    
    void setLaserClips();

    void render(ScreenPoint position,
                int clip = -1, float scale = 1.f) override;
    void render(ScreenPoint position, Size size,
                int clip = -1, float scale = 1.f) override;
    void render(SDL_Rect *frame, SDL_Rect *clip);
    void setOpacity(Uint8 opacity) override;
    void opacityRender(Uint8 opacity, ScreenPoint position,
                       int clip = -1, float scale = 1.f) override;
    
  protected:
    SDL_Renderer* _renderer;
    SDL_Texture* _texture = NULL;
    std::vector<SDL_Rect*> _clips; 
    int _clipCount = 0;
};

class Icon : public Graphic {
  //See Graphic for more details.
  public:
    Icon(Texture* texture, int rowNumber,
         int frameWidth = 100, int frameHeight = 150, int clipCount = 6);
    void free() override {return;}
    
    SDL_Rect* getClip(int clip);
    
    void setOpacity(Uint8 opacity) override {_texture->setOpacity(opacity);}
    void render(ScreenPoint position, int clip = -1, float scale = 1) override;
    
  protected:
    Texture* _texture;
    std::vector<SDL_Rect*> _clips;
    
    int _clipCount;
    int _width;
    int _height;
};

class Sprite;
class Character;

struct DoubleSpot {
    BoardPoint *position;
    Sprite* transparent;
};

class Board {
   /*
    Logic:
    The fundamental logic of game play is that everything: the playerCharacter,
    enemies, props, projectile attacks, potions, gemKeys, uniquely occupy a
    square at any one time, and their positions on the board govern when they
    are rendered so that the 2D image files produce an appropriately layered 3D
    effect. The board contains the textures for the background and foreground
    as well as pointers to every sprite on the board to handle the rendering of
    scene for each frame. (There are other classes that control the rendering
    for information such as health, pause screens, and menus).
    
    Organization:
    The board is a collection of pointers to the various sprites located on it.
    The main board is a 12x12 board with points (m, n) (counted--row-major--
    from the bottom left corner). These are stored in an array indexed with 144
    spots.
    
    There are two additional spaces on the board: _leftDoor and _rightDoor,
    which are spots located abstractly at (-1,6) and (12,6). These are spots
    that only the playerCharacter can occupy, and are the square where gameplay
    starts from.
    
    The playerCharacter:
    In addition to the pointer  for the playerCharacter on either _board or
    _leftDoor or _rightDoor, the board also contains an additional pointer to
    the playerCharacter in _character. This gives everything on the board
    (which each contain a pointer to the board they are on) access to the
    playerCharacter without having to scan the board. This is so that
    interactions like taking damage are quick and directed to a method within
    Character.
    
    Ghosts:
    The ghosts violate the rule that only one sprite can occupy a square at a
    time. They can travel through objects. To handle this, the board uses
    special logic that is a little slower. It is only used when _ghostLayer is
    true. In this case there is a vector of DoubleSpots called _ghosts. Each
    double spot contains a pointer to the position of the ghost and a pointer to
    the ghost. The ghosts occupy squares on the board when they are empty, but
    leave the pointers to previous objects when they share a square. Rendering
    is managed separately.
    
    Winning:
    Depending on the level, different objects control whether or not a level has
    been beaten. To mediate this information between the controlling object and
    the level, the controlling object tells board when a win has been achieved.
    When this happens the doors open and the playerCharacter can exit to the
    next level. Similarly when the level opens the playerCharacter starts in the
    doorway, but the doors start open. The bool _started closes the doors once
    the playerCharacter steps out of the doorway.
    */
  public:
   /*
    Board(Sprite* character) : _character{character} {}
    Board(Sprite* character, bool ghostLayer) :
      _character{character}, _ghostLayer{ghostLayer} {}
    */
    Board(SDL_Renderer* renderer, Sprite* character,
          std::string backgroundPath,
          std::string foregroundPath = "",
          bool ghostLayer = false);
    ~Board();
    
    Sprite* character() {return _character;}
    void render(int time);
        
    Sprite* get(BoardPoint *boardPosition);
    Sprite* get(BoardPoint boardPosition, Direction heading);
    bool set(Sprite* prop, BoardPoint boardPosition);
    void clear(BoardPoint &boardPosition);
    
    ID id(BoardPoint position);
    ID id(BoardPoint &oldPosition, Direction h);
    bool open(BoardPoint boardPosition);
    bool open(BoardPoint oldPosition, Direction h);
    bool openRef(BoardPoint &oldPosition, Direction h);
    bool move(BoardPoint &oldPosition, BoardPoint &newPosition);
    bool move(BoardPoint &oldPosition, Direction h);
    
    void walkOver(BoardPoint boardPosition, Direction heading);
    
    bool ghosts() {return _ghostLayer;}
    bool ghostAt(int arrayPoint);
    bool ghostAt(BoardPoint position);
    
    bool setGhost(Sprite* prop, BoardPoint* boardPosition);
    void setDoors(Texture* door) {_door = door;}
    
    bool isWon() {return _won;}
    void won() {_won = true;}
    
    bool isStarted() {return _started;}
    void started() {_started = true;}
    
  private:
    Sprite* _board[144] {NULL};
    Sprite* _leftDoor = NULL;
    Sprite* _rightDoor = NULL;
    
    Sprite* _character;
    
    Texture* _background = NULL;
    Texture* _foreground = NULL;
    Texture* _door = NULL;
    
    bool _won = false;
    bool _started = false;
    
    bool _ghostLayer = false;
    std::vector<DoubleSpot> _ghosts;
};

class Sprite {
   /*
    This class is the base class for object on the board. Each position on the
    board has a pointer to the Sprite on that spot. The sprite also countains
    its own board position which it must update both in itself and on the board
    as it moves. This class does the rendering for each object on the board.
    The board controls the order.
    
    The calculatePosition() method handles the geometry for the projective view
    of the board. See the source for that method for details.
    */
  public:
    Sprite(Graphic* texture) : _texture{texture} {
        _center = ScreenPoint(_texture->x()/2,_texture->y()-20);
    }
    Sprite(int m, int n, Graphic* texture, int frameCount = 1);
    Sprite(int m, int n, Graphic* texture, Board* board, int frameCount = 1);
    Sprite(int m, int n, Graphic* texture, Board* board, ScreenPoint center,
           int frameCount = 1, int timeShift = 0, int rate = 2);
    Sprite(int m, int n, Board* board, int frameCount = 1);
    virtual ~Sprite() {};
    
    virtual void setBoard(Board* board) {_board = board;}
    int m() {return _boardPosition.x();}
    int n() {return _boardPosition.y();}
    BoardPoint& boardPosition() {return _boardPosition;}
    Size size() {return _texture->size();}
    
    void calculatePosition(ScreenPoint center);
    virtual void calculatePosition();
    void checkPosition();
    void localize();
    
    virtual void render(int time);
    virtual int currentFrame(int time);
    ID id() {return IDTAG;}
    
    virtual bool steppedOn() {return false;}
    virtual void activate(BoardPoint, Direction, int) {return;}
    virtual void ghostRender(int) {return;}

  protected:
    Graphic* _texture = NULL;
    Board* _board = NULL;
    
    InterPoint _abstractPosition;
    BoardPoint _boardPosition;
    ScreenPoint _renderPosition;
    
    ScreenPoint _center = ScreenPoint();
    //measured a distance from the center of the image at the bottom
    bool _positionCalculated = false;
    float _scale = 1.f;
    ID IDTAG;
    int _frameCount = 1;
    int _shift = 0; //time shift
    int _rate = 2;
};

class NullSprite : public Sprite {
   /*
    Every square that cannot be stepped on contains a pointer to an object
    derived from Sprite. Some of the art covers more than one square, but can
    only point at one spot. In order to avoid that same image being rendered
    multiple times, occupied squares have a NullSprite.
    */
  public:
    NullSprite(int m, int n, Board* board) : Sprite(m,n,board) {}
    void render(int) override {return;}
    void calculatePosition() override {return;}
};


//TODO: use STATES for logic for moving sprites
typedef unsigned int State;
const State READY      = 0;
const State WAITING    = (1<<0);
const State TURNING    = (1<<1);
const State MOVING     = (1<<2);
const State ATTACKING  = (1<<3);
const State HURTING    = (1<<4);
const State DYING      = (1<<5);

const State ALL_STATES = (1<<6) - 1; 

class MovingSprite : public Sprite {
   /*
    Sprites that move and take actions. Provides virtual method takeTurn which
    derived classes use to react to the board.
    */
  public:
    MovingSprite(Graphic* texture) : Sprite(texture) {IDTAG = movingObject;}
    MovingSprite(int m, int n, Texture* texture);
    MovingSprite(int m, int n, Texture* texture, Direction d,
                 int speed, Board* board);
    
    int currentFrame(int time) override;
    void render(int time) override;
    
    void changeHeading(Direction nowFacing) {_heading = nowFacing;}
    void changeSpeed(int speed) {_speed = speed;}
    void changePosition(BoardPoint position) {_boardPosition = position;}
    
    virtual bool startmove(Direction d, int time);
    virtual bool step(int time);
    virtual bool takeTurn(int) {return _ready;}
    
  protected:
    State _state = READY;
    bool _ready = true; //deprecate after states are ready
    Direction _heading = D;
    int _speed = 2;
    int _endFrame = 0; //perhaps change to decrementing to 0
};

#endif /* sprite_hpp */
