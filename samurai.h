#ifndef SAMURAI_H
#define SAMURAI_H
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

    bool first_skill(Unit *target);
    bool attack(Unit *target);

};
#endif
