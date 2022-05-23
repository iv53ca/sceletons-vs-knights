#ifndef SCELETON_H
#define SCELETON_H
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
    bool first_skill(Unit *target);

    bool second_skill();

    bool attack(Unit *target);
};
#endif
