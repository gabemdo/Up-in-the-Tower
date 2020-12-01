//
//  effects.hpp
//  Up in the Tower
//
//  Created by Gabe on 11/1/20.
//

#ifndef effects_hpp
#define effects_hpp

#include <cstdlib>

#include "sprite.hpp"

class Potion final : public Sprite {
   /*
    Some enemies will randomly drop potions which heal 2 HP.
    */
  public:
    Potion(int m, int n, Graphic* texture, Board* board);
    
    int currentFrame(int time) override;
    bool steppedOn() override;
    void clear();
    
  protected:
    int _frameCount = 6;
};

class Attack : public MovingSprite {
   /*
    The PrinceS force attack.
    */
  public:
    Attack(int m, int n, Texture* texture);
    Attack(int m, int n, Texture* texture,
           Direction d, int speed, Board* board);
    
    void render(int time) override;
    
    void activate(BoardPoint position, Direction heading, int time) override;
    bool takeTurn(int time) override;
    bool step(int step) override;
    bool startmove(Direction d, int time) override;
    
    void clear();
    
  protected:
    bool _active = false;
};

class GemKey final : public Sprite {
   /*
    Some levels are beaten by collected all of the GemKeys. The spawn randomly
    and away from the playerCharacter.
    */
  public:
    GemKey(int m, int n, Graphic* texture, Board* board);
    int currentFrame(int time) override;
    
    bool steppedOn() override;
    bool collectOne();
    BoardPoint generateSpot();
    
    void renderInfo(int time) {
        for (int i = 0; i < 6; i++) {
            _texture->render(ScreenPoint(275 + i*84,25),
                             ((i>=_gemCount ? 6 : i)*6) + (time/5)%6, 0.5f);
        }
    }
    
    void clear();
    
  private:
    bool _allCollected = false;
    int _gemCount = 0;
};

class Effect : public Sprite {
   /*
    Purely virtual class for effects that occur on the same square as a sprite.
    It can render an image behind (backRender()) and in front of (foreRender())
    its calling sprite;
    */
  public:
    Effect(Graphic* texture) : Sprite(texture) {}
    virtual bool takeTurn(int time) = 0;
    virtual void backRender(int time) = 0;
    virtual void foreRender(int time) = 0;
    bool active() {return _active;}
    
  protected:
    bool _active = false;
};

class Laser final : public Effect {
   /*
    The sentries shoot lasers at the playerCharacter when 2 squares in front.
    */
  public:
    Laser(Graphic* texture) : Effect(texture) {}
    void activate(BoardPoint boardPosition, Direction d, int time) override;
    void deactivate() {_active = false; _endFrame = 0;}
    bool takeTurn(int time) override;
    void backRender(int time) override;
    void foreRender(int time) override;
    
  private:
    Direction _heading = TOTAL;
    int _endFrame = 0;
    int _duration = 20;
};

#endif /* effects_hpp */
