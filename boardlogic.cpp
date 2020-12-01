//
//  boardlogic.cpp
//  Up in the Tower
//
//  Created by Gabe on 11/10/20.
//

#include "boardlogic.hpp"



bool ScreenPoint::onScreen() {
    return (!((0 <= _x) && (_x < 1200) && (0 <= _y) && (_y < 800)));
}

bool ScreenPoint::onScreen(Size size) {
    bool xintersect, yintersect;
    xintersect = ((0 <= _x) && (_x < 1200));
    xintersect |= ((0 <= (_x + size.x())) && ((_x + size.x()) < 1200));
    xintersect |= ((_x < 0) && (1200 <= (_x + size.x())));
    
    yintersect = ((0 <= _y) && (_y < 800));
    yintersect |= ((0 <= (_y + size.y())) && ((_y + size.y()) < 800));
    yintersect |= ((_y < 0) && (800 <= (_y + size.y())));
    return xintersect & yintersect;
}

int BoardPoint::arrayPoint() {
    if (_y == 6) {
        if (_x == -2) {
            return -2;
        } else if (_x == -1) {
            return -1;
        } else if (_x == 12) {
            return 144;
        } else if (_x == 13) {
            return 145;
        }
    }
    return _x + 12 * _y;
}

bool isEdge(BoardPoint position) {
    if (position.x() <=  0) return true;
    if (11 <= position.x()) return true;
    if (position.y() <=  0) return true;
    if (11 <= position.y()) return true;
    
    return false;
}

BoardPoint moveOne(BoardPoint position, Direction heading) {
    BoardPoint newPosition = position;
    switch (heading) {
        case R:
            newPosition.dx(1);
            break;
        case U:
            newPosition.dy(1);
            break;
        case L:
            newPosition.dx(-1);
            break;
        case D:
            newPosition.dy(-1);
            break;
        case UR:
            newPosition.dx(1);
            newPosition.dy(1);
            break;
        case UL:
            newPosition.dx(-1);
            newPosition.dy(1);
            break;
        case LL:
            newPosition.dx(-1);
            newPosition.dy(-1);
            break;
        case LR:
            newPosition.dx(1);
            newPosition.dy(-1);
            break;
        default:
            break;
    }
    return newPosition;
}

BoardPoint diagonalLeft(BoardPoint position, Direction heading) {
    BoardPoint newPosition = position;
    switch (heading) {
        case R:
            newPosition.dx(1);
            newPosition.dy(1);
            break;
        case U:
            newPosition.dx(-1);
            newPosition.dy(1);
            break;
        case L:
            newPosition.dx(-1);
            newPosition.dy(-1);
            break;
        case D:
            newPosition.dx(1);
            newPosition.dy(-1);
            break;
        default:
            std::cout << "DIAGONAL ERROR in DIAGONAL LEFT" << std::endl;
            break;
    }
    return newPosition;
}

BoardPoint diagonalRight(BoardPoint position, Direction heading) {
    BoardPoint newPosition = position;
    switch (heading) {
        case R:
            newPosition.dx(1);
            newPosition.dy(-1);
            break;
        case U:
            newPosition.dx(1);
            newPosition.dy(1);
            break;
        case L:
            newPosition.dx(-1);
            newPosition.dy(1);
            break;
        case D:
            newPosition.dx(-1);
            newPosition.dy(-1);
            break;
        default:
            std::cout << "DIAGONAL ERROR in DIAGONAL LEFT" << std::endl;
            break;
    }
    return newPosition;
}

Direction oppositeDirection(Direction direction) {
    switch (direction) {
        case R:
            return L;
            break;
        case U:
            return D;
            break;
        case L:
            return R;
            break;
        case D:
            return U;
            break;
        default:
            return TOTAL;
    }
}


