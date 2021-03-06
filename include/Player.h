#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include "Sphere.h"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <vector>
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
    int lives;
    bool collision;
    glm::vec3 braking;

    Player() : Object("jet", "../../data/jet.obj")
    {
        this->speed = glm::vec3(0.0f);
        this->turned_x = false;
        this->turned_z = false;
        this->last_update = 0;
        this->lives = 3;
        this->collision = false;
        this->braking = glm::vec3(1,1,1);
        this->acceleration = glm::vec3(0.0f,0.0f,-0.2f);
    }

//        virtual ~Player();

    void update(double dt, vector<Object *> objs){
        bool col = this->checkCollision(objs);

        if(!col && this->collision){
            this->collision = false;
            this->lives--;
            cout << "outch? ";
            cout << lives;
        }

//        glm::vec3 rotation = this->rad;

        if (braking.x) {
            this->speed.x = abs(speed.x) <= 0.1f ? 0.0f : this->speed.x - this->acceleration.x * dt;
//            rotation.z -= (speed.x * M_PI / 180) * dt * 10;
        } else {
            this->speed.x += this->acceleration.x * dt;
            this->speed.x = (speed.x >= MAX_SPEED ? MAX_SPEED : speed.x);
//            rotation.z += (speed.x * M_PI / 180) * dt * 10;
        }

        if (braking.y) {
            this->speed.y = abs(speed.y) <= 0.1f ? 0.0f : this->speed.y - this->acceleration.y * dt;
//            rotation.x -= (speed.y * M_PI / 180) * dt * 10;
        } else {
            this->speed.y += this->acceleration.y * dt;
            this->speed.y = (speed.y >= MAX_SPEED ? MAX_SPEED : speed.y);
//            rotation.x += (speed.y * M_PI / 180) * dt * 10;
        }

        if (braking.z) {
            this->speed.z = 0;
        } else {
            this->speed.z += this->acceleration.z * dt;
            this->speed.z = (speed.z >= MAX_SPEED ? MAX_SPEED : speed.z);
        }

//        rotation = glm::clamp(rotation,glm::vec3(-1.57,rotation.y,-1.57),glm::vec3(1.57,rotation.y,1.57));

        glm::vec3 newpos(this->pos.x + this->speed.x * dt,
                         this->pos.y + this->speed.y * dt,
                         this->pos.z + this->speed.z * dt);

//        if(newpos.x < -3.0f || newpos.x > 3.0f){
//            newpos.x = glm::clamp(newpos.x, -3.0f, 3.0f);
//            this->acceleration.x = 0.0f;
//        }
//
//        if(newpos.y < -2.0f || newpos.y > 3.0f){
//            newpos.y = glm::clamp(newpos.y, -5.0f, 5.0f);
//            this->acceleration.y = 0.0f;
//        }

//        this->rad = rotation;
        this->setPos(newpos);
    }

    void move(glm::vec3 direction) {
        if(direction.x)
            this->braking.x = 0;
        if(direction.y)
            this->braking.y = 0;
        if(direction.z)
            this->braking.z = 0;
        this->acceleration = direction;
    }

#define X 1
#define Y 2
#define Z 3

    void brake(int axis) {
        switch(axis){
            case X:
                this->braking.x = 1;
                break;
            case Y:
                this->braking.y = 1;
                break;
            case Z:
                this->braking.z = 1;
                break;
            default:
                break;
        }
//        this->braking = true;
//        this->acceleration = -this->acceleration;
    }

    bool checkCollision(vector<Object *> vect){
        bool ret = false;
        for(auto const obj : vect){
            if(obj->name != this->name){
                switch (obj->hitbox_type) {
                    case SPHERE:
                         if (checkCollisionSphere((Sphere *) obj)){
                             ret = true;
                             this->collision = true;
                         }
                        break;
                    case PLANE:
                         if(checkCollisionPlane(obj)){

                         }
                        break;
                    case AABB:
                        if(checkCollisionAABB(obj)) {
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        return ret;
    }

    bool checkCollisionAABB(Object* obj){
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
            obj->destroyed = true;
            printf("boom %s\n", obj->name.c_str());
            return true;
        }

        return false;

    }

    //https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection
    bool checkCollisionSphere(Sphere* obj) {
        glm::vec3 difference = obj->pos - this->pos;
        glm::vec3 half_extents;
        half_extents.x = abs(this->bbox_max.x - this->bbox_min.x) / 2;
        half_extents.y = abs(this->bbox_max.y - this->bbox_min.y) / 2;
        half_extents.z = abs(this->bbox_max.z - this->bbox_min.z) / 2;

        glm::vec3 clamped = glm::clamp(difference, -half_extents, half_extents);
        glm::vec3 closest = this->pos + clamped;
        difference = closest - obj->pos;

        if(glm::length(difference) < obj->radius ){ //I have no idea how to make this work tbh
           if(this->lives <= 0){
               this->destroyed = true;
               cout << "RIP" << endl;
               return false;
           }
            cout << "sphere boom" << endl;
            return true;
        }

        return false;
    }

    //https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    bool checkCollisionPlane(Object* obj){
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
        plane_normal = glm::normalize(plane_normal);

        float projection =  half_extents.x * abs(plane_normal.x) +
                            half_extents.y * abs(plane_normal.y) +
                            half_extents.z * abs(plane_normal.z);

        float plane_d = glm::length(glm::vec3(0.0f,0.0f,0.0f) - (obj->pos));

        float distance = glm::dot(this->pos,(plane_normal)) - plane_d;

        if(abs(distance) < projection){
            if(checkCollisionAABB(obj)) {
                obj->destroyed = true;
                cout << "plane boom" << endl;
                return true;
            }
        }

        return false;
    }

    void fix_bbox(){
        float aux = this->bbox_min.x;
        this->bbox_min.x = this->bbox_max.x;
        this->bbox_max.x = aux;

        aux = this->bbox_min.z;
        this->bbox_min.z = this->bbox_max.z;
        this->bbox_max.z = aux;
    }


    void  update_bbox(){
        this->bbox_max.x = 10;
        this->bbox_max.y = 0.5; //The y axis is unchangeable for some reason......
        this->bbox_max.z = 16;

        this->bbox_min.x = -10;
        this->bbox_min.y = -0.5;
        this->bbox_min.z = - 16;
    }

private:
    bool turned_x;
    bool turned_z;
};

#endif // PLAYER_H
