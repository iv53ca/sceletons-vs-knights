#ifndef HEROES_H
#define HEROES_H
#include <QRandomGenerator>
#include <QUrl>
#include <QDir>
#include <windows.h>
#include <cmath>
#include <thread>
#include <random>
#include <chrono>
class Object {
public:
    int x;
    int y;
    int w;
    int cell_size = 128;
    int h;
    bool right;
    QString sprite_now;
    QString sprite_type_now;
    QString sprite_object_now;
    QString sprite_path;

    struct three_obj {
        int x;
        int y;
        QImage img;
        bool right;
        three_obj(int xi, int yi, QImage imgi, bool righti) {
            x = xi;
            right = righti;
            y = yi;
            img = imgi;
        }
    };

    std::queue<three_obj> draw_queue;

    void one_tenth_sec_tic() {
        if (sprite_object_now != "s") {
            if (sprite_now.toInt() + 1 <= 16) {
                sprite_now.setNum(sprite_now.toInt() + 1);
            } else {
                sprite_now.setNum(1);
            }
            sprite_path = "sprites/" + sprite_object_now + "_" + sprite_type_now + "_" + sprite_now + ".PNG";
        } else {
            sprite_path = "sprites/" + sprite_type_now + "_" + sprite_now + ".PNG";
        }
    }
    virtual void move(int dx, int dy) {
        x += dx;
        y += dy;
    }
    virtual void draw() {
        draw_queue.push(three_obj(x, y, QImage(sprite_path), right));
    }
    Object(QString sprite_now1, QString sprite_type_now1, QString sprite_object_now1, int x1, int y1) {
        sprite_now = sprite_now1;
        x = x1;
        y = y1;
        right = 0;
        sprite_type_now = sprite_type_now1;
        sprite_object_now = sprite_object_now1;
    }
    Object(){}
    virtual ~Object(){}
};
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
class Skeleton : public Unit {
public:
    Skeleton() {
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
class Samurai : public Unit {
public:
    Samurai() {
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
#endif
