//
//  boardlogic.hpp
//  Up in the Tower
//
//  Created by Gabe on 11/10/20.
//

#ifndef boardlogic_hpp
#define boardlogic_hpp

#include <iostream>
//#include <string>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const char* const WINDOW_TITLE = "Up in the Tower";
const int maxLevel = 5;

//testing variables
const int READOUT_LEVEL = 0;
const int TEST_LEVEL = 1;
const bool TEST_MODE = true;// false;//

/*
 1 : Gremlin
 2 : Ogre
 3 : Sentry
 4 : Ghost
 5 : Guard
 */

enum Direction {
    R=0,
    U,
    L,
    D,
    UR,
    UL,
    LL,
    LR,
    TOTAL
};

enum ID {
    wall,
    door,
    levelChange,
    openSquare,
    character,
    physicalObject,
    movingObject,
    enemy,
    ghost,
    magic,
    projectile,
    item,
    key
};

class Point {
  /*
   (x,y)
   */
  public:
    
    virtual ~Point() {};
    
    void set(int xp, int yp) {_x = xp; _y = yp;}
    
    int x() {return _x;}
    int y() {return _y;}
    
    int x(int x) {_x = x; return _x;}
    int y(int y) {_y = y; return _y;}
    
    void dx(int d) {_x += d;}
    void dy(int d) {_y += d;}
    
    void scale(int factor) {_x *= factor; _y *= factor;}
    Point(const Point& point) : _x{point._x}, _y{point._y} {}
    
  protected:
    int _x, _y;
    Point() {_x = 0; _y = 0;}
    Point(int xp, int yp) : _x(xp), _y(yp) {}
    
};

class Size : public Point {
  public:
    Size() : Point() {}
    Size(int w, int h) : Point(w,h) {
        if (!(0 <= w && 0 <= h)) {
            _x = 0;
            _y = 0;
        }
    }
    
    bool trivial() {return (_x == 0) && (_y == 0);}
};

class BoardPoint : public Point {
   /*
    (m,n)
    The board operates as a 12 by 12 (m,n) with 4 additional positions: (m,6)
    for m = -1,-2,12,13. These spots correspond to the exits and doorways.
    Nothing can occupy the exits, but they control the logic for leaving the
    level. Only the playerCharacter can occupy the doorways. ArrayPoint deals
    with the conversion between points in (m,n) form and indices (but real
    0...143, and abstract -2, -1, 144, 145 (corresponding to exits and
    doorways)).
    */
  public:
    BoardPoint(bool isLeftDoor) : Point((isLeftDoor ? -1 : 12), 6) {}
    BoardPoint(int m = 0, int n = 0) : Point(m,n) {
        if ( m == -1 && n == -1) return; //The don't-render spot.
        if (!(0 <= m && m < 12 && 0 <= n && n < 12) &&
            !((n == 6) && (-2 <= m && m <= 13))) {
            _x = -1;
            _y = -1;
        }
    }
    BoardPoint(int arrayPoint) : Point(arrayPoint%12, arrayPoint/12) {
        if (arrayPoint < -2 || 145 < arrayPoint) { //off board
            _x = -1;
            _y = -1;
        } else if (arrayPoint == -2) { //left level-change spot
            _x = -2;
            _y = 6;
        } else if (arrayPoint == -1) { //left door
            _x = -1;
            _y = 6;
        } else if (arrayPoint == 144) { //right door
            _x = 12;
            _y = 6;
        } else if (arrayPoint == 145) { //right level-change spot
            _x = 12;
            _y = 6;
        }
    }
    int arrayPoint();
};

class InterPoint : public Point {
    /*
     (a,b)
     These points are for intermediate points on the board. Where each of the
     144 squares is divided into 140x140 intermediate points.
     */
  public:
    InterPoint(int a=0, int b=0) : Point(a,b) {}
    InterPoint(BoardPoint boardPosition) : Point(boardPosition) {scale(140);}
    void set(BoardPoint boardPosition) {
        _x = boardPosition.x() * 140;
        _y = boardPosition.y() * 140;
    }
};

class ScreenPoint : public Point {
   /*
    Assuming a window size of 1200 wide and 800 tall.
    */
  public:
    ScreenPoint(int xp = 0, int yp = 0) : Point(xp,yp) {}
    bool onScreen();
    bool onScreen(Size size);
};

bool isEdge(BoardPoint position);

BoardPoint moveOne(BoardPoint position, Direction heading);

BoardPoint diagonalLeft(BoardPoint position, Direction heading);

BoardPoint diagonalRight(BoardPoint position, Direction heading);

Direction oppositeDirection(Direction direction);

#endif /* boardlogic_hpp */
