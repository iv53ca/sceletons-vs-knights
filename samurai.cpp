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
#include "samurai.h"
class Samurai : public Unit {
private:

public:
    Samurai(){
    moves_max = 10;
    range1 = 2;
    range5 = 2;
    sprite_now = "1";
    sprite_type_now = "stand";
    sprite_object_now = "samurai";
    hp = 1000;
    max_hp = 1000;
    mp = 100;
    max_mp = 100;
    hp_reg = 10;
    mp_reg = 5;
    dmg = 40;
    attacked = false;
    }

    bool first_skill(Unit *target) {
        if (target != NULL) {
            if (mp >= 20) {
                mp -= 20;

                target->sprite_type_now = "getdmg";
                target->sprite_now = "1";
                return target->get_dmg(dmg * 2);
            } else {
                return false;
            }
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
