#include <QRandomGenerator>
#include <QUrl>
#include <QDir>
#include <windows.h>
#include <cmath>
#include <thread>
#include <random>
#include <chrono>
#include "object.h"
#include "unit.h"
#include "sceleton.h"
class Skeleton : public Unit {
private:

public:
    Sceleton(){
    moves_max = 10;
    range1 = 1;
    range5 = 1;
    sprite_now = "1";
    sprite_type_now = "stand";
    sprite_object_now = "skeleton";
    hp = 110;
    max_hp = 110;
    mp = 20;
    max_mp = 20;
    hp_reg = 20;
    mp_reg = 5;
    dmg = 50;
    attacked = false;
    }
    bool first_skill(Unit *target) {
        if (target != NULL) {
            if (mp >= 20) {
                mp -= 20;

                target->sprite_type_now = "getdmg";
                target->sprite_now = "1";
                target->get_stuned(1);
                return target->get_dmg(20);
            } else {
                return false;
            }
        }else {
            return false;
        }
    }

    bool second_skill() {
        if (mp >= 20) {
            mp -= 20;

            if (hp + 50 < max_hp) {
                hp += 50;
            } else {
                hp = max_hp;
            }
            return true;
        } else {
            return false;
        }
    }

    bool attack(Unit *target) {
        if (target != NULL) {
            attacked = true;
            dynamic_cast<Unit*>(target)->sprite_type_now = "getdmg";
            dynamic_cast<Unit*>(target)->sprite_now = "1";
            return target->get_dmg(dmg);
        } else {
            return false;
        }
    }
};
