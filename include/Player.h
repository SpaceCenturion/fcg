#ifndef PLAYER_H
#define PLAYER_H
#include "Object.h"
#define SPEED_INCREMENT 0.005f
#define TURN_ANGLE 0.3f
#define MAX_ANGLE 1.2f
class Player:public Object
{
    public:
        cord speed;
        double last_update;
        Player();
        virtual ~Player();
        void move_up();
        void move_down();
        void move_left();
        void move_right();
        void move_foward();
        void move_backwards();
        void update_player(double time);
        void unturn_left();
        void unturn_right();
        void unturn_up();
        void unturn_down();



    protected:

    private:
};

#endif // PLAYER_H