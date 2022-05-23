#include <QRandomGenerator>
#include <QUrl>
#include <QDir>
#include <windows.h>
#include <cmath>
#include <thread>
#include <random>
#include "object.h"
#include <chrono>
class Object {
private:

public:
    QString sprite_now;
    QString sprite_type_now;
    QString sprite_object_now;
    QString sprite_path;

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

    int x;
    int y;
    int w;
    int cell_size = 128;
    int h;
    bool right;


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
