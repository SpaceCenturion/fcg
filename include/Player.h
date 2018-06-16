#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include "Sphere.h"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#define SPEED_INCREMENT 1.0f
#define MAX_SPEED 12.0f
#define ACCELERATION 2.0f
#define TURN_ANGLE 0.3f
#define MAX_ANGLE 1.2f

class Player : public Object  {
public:
    glm::vec3 speed;
    glm::vec3 direction;
    glm::vec3 acceleration;
    double last_update;

    Player() : Object("jet", "../../data/jet.obj")
    {
        this->speed.x = 0;
        this->speed.y = 0;
        this->speed.z = 0;
        this->turned_x = false;
        this->turned_z = false;
        this->last_update = 0;

        this->acceleration = glm::vec3(0.0f,0.0f,0.0f);
    }

//        virtual ~Player();

    void update_player(double dt, vector<Object*> objs){
        this->checkCollision(objs);
        this->speed.x += this->acceleration.x * dt;
        this->speed.y += this->acceleration.y * dt;
        this->speed.z += this->acceleration.z * dt;

        this->speed.x = (speed.x >= MAX_SPEED ? MAX_SPEED : speed.x);
        this->speed.y = (speed.y >= MAX_SPEED ? MAX_SPEED : speed.y);
        this->speed.z = (speed.z >= MAX_SPEED ? MAX_SPEED : speed.z);

        glm::vec3 newpos(this->pos.x + this->speed.x * dt,
                         this->pos.y + this->speed.y * dt,
                         this->pos.z + this->speed.z * dt);

        this->setPos(newpos);
    }

    void move(glm::vec3 direction) {
        this->acceleration = direction;
    }

    void move_up() {
        this->speed.y += SPEED_INCREMENT;
        if (turned_x == false) {
            this->rad.x += TURN_ANGLE;
            turned_x = true;
        }
    }

    void move_down() {
        this->speed.y -= SPEED_INCREMENT;
        if (turned_x == false) {
            this->rad.x -= TURN_ANGLE;
            turned_x = true;
        }
    }

    void move_left() {
        this->speed.x -= SPEED_INCREMENT;
        if (turned_z == false) {
            this->rad.y += TURN_ANGLE;
            turned_z = true;
        }
    }

    void move_right() {
        this->speed.x += SPEED_INCREMENT;
        if (turned_z == false) {
            this->rad.y -= TURN_ANGLE;
            turned_z = true;
        }
    }

    void move_foward(){
        this->speed.z += SPEED_INCREMENT;
    }

    void move_backwards(){
        this->speed.z -= SPEED_INCREMENT;
    }

    void unturn_left(){
        this->rad.y -= TURN_ANGLE;
        this->speed.x = 0;
        turned_z = false;
    }

    void unturn_right(){
        this->rad.y += TURN_ANGLE;
        this->speed.x = 0;
        turned_z = false;
    }

    void unturn_up(){
        this->rad.x -= TURN_ANGLE;
        this->speed.y = 0;
        turned_x = false;
    }

    void unturn_down(){
        this->rad.x += TURN_ANGLE;
        this->speed.y = 0;
        turned_x = false;
    }

    void checkCollision(vector<Object *> vect){
        for(auto const obj : vect){
            if(obj->name != this->name){
                switch (obj->obj_type) {
                    case SPHERE:
                        checkCollisionSphere((Sphere *) obj);
                        break;
                    case PLANE:
                        checkCollisionPlane(obj);
                        break;
                    case AABB:
                        checkCollisionAABB(obj);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void checkCollisionAABB(Object* obj){
        bool col_x = false;
        bool col_y = false;
        bool col_z = false;
        if(this->bbox_max.x >= obj->bbox_min.x
           && obj->bbox_max.x >= this->bbox_min.x) {
            col_x = true;
            //            std::cout << "col_x: " << this->name << " " << obj->name << std::endl;
        }

        if(this->bbox_max.y >= obj->bbox_min.y
           && obj->bbox_max.y >= this->bbox_min.y) {
            col_y = true;
            //            std::cout << "col_y: " << this->name << " " << obj->name << std::endl;
        }

        if(this->bbox_max.z >= obj->bbox_min.z
           && obj->bbox_max.z >= this->bbox_min.z) {
            col_z = true;
            //            std::cout << "col_z: " << this->name << " " << obj->name << std::endl;
        }

        if(col_x && col_y && col_z){
            //obj->destroyed = true;
            printf("boom %s\n", obj->name.c_str());
            //return true;
        }

    }

    void checkCollisionSphere(Sphere* obj) {
            //https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection

            glm::vec3 difference = obj->pos - this->pos;
             glm::vec3 half_extents;
             half_extents.x = abs(this->bbox_max.x - this->bbox_min.x) / 2;
             half_extents.y = abs(this->bbox_max.y - this->bbox_min.y) / 2;
             half_extents.z = abs(this->bbox_max.z - this->bbox_min.z) / 2;

            glm::vec3 clamped = glm::clamp(difference, -half_extents, half_extents);
            glm::vec3 closest = this->pos + clamped;
            difference = closest - obj->pos;

            if(glm::length(difference) < obj->radius ){ //I have no idea how to make this work tbh
               //obj->destroyed = true;
                cout << "sphere boom" << endl;
            }

        }

        void checkCollisionPlane(Object* obj){
            glm::vec3 half_extents = this->bbox_max - this->pos;
            glm::vec3 half_extents_plane = obj->bbox_max - obj->pos;

            glm::vec3 p1 = obj->bbox_max;
            glm::vec3 p2;
            ///this kinda assumes axis aligned planes i guess
            if(half_extents_plane.x > 0.1){
                p2 = p1 - glm::vec3(2*half_extents_plane.x, 0.0f, 0.0f);
            }
            else{
                p2 = p1 - glm::vec3(0.0f,2*half_extents_plane.y, 0.0f);
            }

            glm::vec3 plane_normal = cross((obj->pos - p2),(obj->pos - p1));
            //https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
            float projection =  half_extents.x * abs(plane_normal.x) +half_extents.y * abs(plane_normal.y) + half_extents.z * abs(plane_normal.z);

            float plane_d = glm::length((obj->pos)-glm::vec3(0.0f,0.0f,0.0f));
            float distance = dot((this->pos - obj->pos),(plane_normal)) - plane_d;

            if(abs(distance) < projection){
                cout << "plane boom" <<endl;
            }

        }


        void /*gambiarra*/ fix_bbox(){
            float aux = this->bbox_min.x;
            this->bbox_min.x = this->bbox_max.x;
            this->bbox_max.x = aux;

            aux = this->bbox_min.z;
            this->bbox_min.z = this->bbox_max.z;
            this->bbox_max.z = aux;
        }

        ///gambiarra to fiz the monstrous size of the jet hitbox
        void  update_bbox(){
            this->bbox_max.x = 10;
            this->bbox_max.y = 0.5; //The y axis is unchangeable for some reason......
            this->bbox_max.z = 16;

            this->bbox_min.x = -10;
            this->bbox_min.y = -0.5;
            this->bbox_min.z = - 16;
        }

protected:

private:
    bool turned_x;
    bool turned_z;




};

#endif // PLAYER_H
