//
//  enemy.hpp
//  Up in the Tower
//
//  Created by Gabe on 11/2/20.
//

#ifndef enemy_hpp
#define enemy_hpp

#include <cstdlib>

#include "sprite.hpp"
#include "effects.hpp"

class Enemy : public MovingSprite {
    
  public:
    Enemy(int m, int n, Texture* enemyTexture);
    Enemy(int m, int n, Texture* texture,
          Direction d, int speed, Board* board);
    
    virtual Effect* attackEffect() {return NULL;}
    void render(int time) override;
    void renderIcon(int time, ScreenPoint screenPosition);
    int hp() {return _HP;}
    
    bool takeTurn(int time) override;
    bool startmove(Direction d, int time) override;
    bool steppedOn() override;
    virtual void deathBehavior() {return;}
    
  protected:
    int _HP = 3;
    int _endFrame = 0;
    bool _alive = true;
};

class Guard final : public Enemy {
    
  public:
    Guard(int m, int n, Texture* texture,
          Direction d, int speed, Board* board);
    Guard(int m, int n, Texture* texture,
          Direction d, int speed, Board* board, Graphic* replacementIcon);
    
    int currentFrame(int time) override;
    bool takeTurn(int time) override;
    bool step(int step) override;

    void deathBehavior() override;
    
  private:
    Sprite* _replacement = NULL;
    Graphic* _replacementIcon = NULL;
};

class Ghost final : public Enemy {
    
  public:
    Ghost(int m, int n, Texture* texture,
          Direction d, int speed, Board* board);
    
    //int currentFrame(int time) override;
    void ghostRender(int time) override;
    void render(int) override {return;} //only renders through ghostRender()
    
    bool takeTurn(int time) override;
    bool startmove(Direction d, int time) override;
    bool step(int time) override;
    void deathBehavior() override;
    
  private:
    Sprite* _replacement = NULL;
    Graphic* _replacementIcon = NULL;
};

class Sentry final : public Enemy {
    
  public:
    Sentry(int m, int n, Texture* sentryTexture, Texture* laserTexture,
           Direction d, int speed, Board* board);
    Sentry(int m, int n, Texture* sentryTexture, Texture* laserTexture,
           Direction d, int speed, Board* board, Graphic* replacementIcon);
    
    Effect* attackEffect() override {return _laser;}
    
    //int currentFrame(int time) override;
    void render(int time) override;
    bool takeTurn(int time) override;
    bool step(int step) override;
    void deathBehavior() override;
   
  private:
    bool scanForCharacter();
    
    Sprite* _replacement = NULL;
    Graphic* _replacementIcon = NULL;
    Laser* _laser = NULL;
    bool _lasing = false;
};

class Ogre final : public Enemy {
  public:
    Ogre(int m, int n, Texture* texture,
         Direction d, int speed, Board* board);
    Ogre(int m, int n, Texture* texture,
         Direction d, int speed, Board* board, Graphic* replacementIcon);
    
    //int currentFrame(int time) override;
    bool takeTurn(int time) override;
    bool step(int step) override;
    void deathBehavior() override;
    
  private:
    Direction newDirection();
    
    Sprite* _replacement = NULL;
    Graphic* _replacementIcon = NULL;
};

class Gremlin final : public Enemy {
public:
    Gremlin(int m, int n, Texture* texture,
            Direction d, int speed, Board* board);
    
    //int currentFrame(int time) override;
    bool step(int step) override;
};


#endif /* enemy_hpp */


