#ifndef UNIT_H
#define UNIT_H
#include <QRandomGenerator>
#include <QUrl>
#include <QDir>
#include <windows.h>
#include <cmath>
#include <thread>
#include <random>
#include <chrono>
#include "object.h"
class Unit : public Object {
public:
    int hp;
    int stuned;
    bool enemy;
    int cell_size_s;
    int max_hp;
    int hp_reg;
    int mp;
    int max_mp;
    int mp_reg;
    int range1;
    int range2;
    int range3;
    int range4;
    int range5;
    bool dead;

    virtual bool first_skill(Unit* target){}
    virtual bool second_skill(Unit* target){}
    virtual bool attack(Unit* target){}
    bool attacked;
    int dmg;
    int moves_left;
    int moves_max;
    Unit() {
        cell_size_s = cell_size;
        stuned = 0;
        dead = false;
    }

    void get_stuned(int n) {
        stuned += n;
    }
    bool get_dmg(int damage) {
        if (hp - damage <= 0) {
            hp = 0;
            return true;
        } else {
            hp -= damage;
            return false;
        }
    }


    void one_turn_tic() {
        if (max_hp >= hp + hp_reg) {
            hp += hp_reg;
        } else {
            hp = max_hp;
        }
        if (max_mp >= mp + mp_reg) {
            mp += mp_reg;
        } else {
            mp = max_mp;
        }

        if (stuned > 0) stuned--;
        moves_left = moves_max;
        attacked = false;
    }
    void control_move(int dx, int dy) {
        if (dx + dy <= moves_left) {
            moves_left -= abs(dx) + abs(dy);
            move(dx * cell_size_s, dy * cell_size_s);
        }
    }
};
#endif
