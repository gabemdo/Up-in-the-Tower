//
//  character.hpp
//  Up in the Tower
//
//  Created by Gabe on 10/28/20.
//

#ifndef character_hpp
#define character_hpp

#include "sprite.hpp"
#include "effects.hpp"

#include <string>

class Character : public MovingSprite {
  public:
Character(SDL_Renderer* renderer, int skin, int hair, bool female,
          std::string name, Texture* detail,
          Texture* skinTexture, Texture* hairTexture);
    
    void setBoard(Board* board) override;
    void reset(BoardPoint boardPosition);
    void reinitialize();
    
    void setAttack(MovingSprite* attack);
    Texture* attackSetting() {return _attackTexture;}
    MovingSprite* beamSetting() {return _attack;}
    
    int currentFrame(int time) override;
    void render(int time) override;
    void renderHP(int time);
    
    void renderPauseMenu();
    void renderDeath();
    void renderWin();
    
    bool isPrinceS() {return _isPrinceS;}
    bool isFemale() {return _female;}
    int skin() {return _skin;}
    int hair() {return _hair;}
    int furthestLevel() {return _level;}
    
    std::string name() {return _name;}
    bool ready() {return _ready;}
    int hp() {return _HP;}
    
    void scanMove(const Uint8* currentKeyStates, int time);
    void attack(int time);
    bool startmove(Direction d, int time) override;
    bool step(int time) override;
    
    void drinkPotion() { _HP = ( _HP >= 2 ) ? 4 : 3; }
    bool steppedOn() override;
    
    void die();
    void win();

  protected:
    SDL_Renderer* _renderer;
    int _frame = -1;
    int _HP = 4; //TODO: Restore to 4;
    int _attacking = 0;
    int _skin;
    int _hair;
    int _level = 0;
    bool _isPrinceS = true;
    bool _female = true;
    std::string _name = "UNINITIALIZED"; //for TESTING
    
    BoardPoint _startPosition;
    Texture* _hairTexture;
    Texture* _detail;
    
    Texture* _HPTexture;
    Texture* _attackTexture;
    Texture* _pauseMenuTexture;
    
    MovingSprite* _attack = NULL;
    
    Texture* _characterIconSheet;
    Icon* _tear;
    Icon* _death;
    Icon* _win;
    Icon* _smile;
    Icon* _boom;
};



class PrinceS final : public Character {
  public:
    PrinceS(SDL_Renderer* renderer, int skin, int hair, bool female,
            std::string name, Texture* detail,
            Texture* skinTexture, Texture* hairTexture);
    ~PrinceS();
};



class Knight final : public Character {
  public:
    Knight(SDL_Renderer* renderer, int skin, int hair, bool female,
           std::string name, Texture* detail, Texture* skinTexture);
    ~Knight();
};


#endif /* character_hpp */
