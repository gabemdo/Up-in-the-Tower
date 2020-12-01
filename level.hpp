//
//  level.hpp
//  Up in the Tower
//
//  Created by Gabe on 11/3/20.
//

#ifndef level_hpp
#define level_hpp

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "sprite.hpp"
#include "character.hpp"
#include "effects.hpp"
#include "enemy.hpp"
#include "menus.hpp"

/*
 ENEMY NOTES:
 ordered by easy for PrinceS / difficulty for Knight
 
 Guard
 Ghost (magic weak, mid speed)
 GolfTee (slow sleep tracker)
 Tank (magic resist)
 Swarm
 */

enum LevelType {
    collectAll,
    killAll
};

class Level {
public:
    Level(int levelNumber, SDL_Renderer* renderer, Character* character,
          std::string backgroundImage, std::string doorImage,
          std::string foregroundImage = "", LevelType type = collectAll);
    ~Level();

    SDL_Renderer* renderer() {return _renderer;}
    Board* board() {return &_board;}
    std::vector<Enemy*>* enemies() {return &_enemies;}
    std::vector<Sprite*>* setPieces() {return &_setPieces;}
    
    void setGem(GemKey* gem) {_gem = gem;}
    int runLevel();
    
    void checkWin(int time, int &finalFrame);
    
    int winMenu();
    int loseMenu();
    
    void renderInfo(int frame);
    void renderMonsterCount(int time);
    
private:
    
    SDL_Renderer* _renderer;
    Character* _character;
    
    LevelType _levelType;
    Board _board;
    Texture* _doors;
    GemKey* _gem = NULL;
    
    std::vector<Enemy*> _enemies;
    std::vector<Sprite*> _setPieces;
    
    bool _wonAndDone = false;
    
    int _activeEnemies = 0;
    int _totalEnemies = 0;
    int _levelNumber;
    int _time = 0;
};

int runTestLevel(SDL_Renderer* renderer, Character* character);

//The Entry Hall
//Gremlin, First Draft complete!
int gremlinLevel(SDL_Renderer* renderer, Character* character);

//The Conservatory
//Ogre, First Draft complete!
int    ogreLevel(SDL_Renderer* renderer, Character* character);

//The Statuary
//Sentry, First draft, make statues
int  sentryLevel(SDL_Renderer* renderer, Character* character);

//The Library
//Ghost, First draft complete!
int   ghostLevel(SDL_Renderer* renderer, Character* character);

//The Dungeon
// Guard, First Draft complete!
int   guardLevel(SDL_Renderer* renderer, Character* character);

#endif /* level_hpp */
