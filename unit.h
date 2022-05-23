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
private:

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

    void get_stuned(int n);

    bool get_dmg(int damage);
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
    void one_turn_tic();
    void control_move(int dx, int dy);
};
#endif
