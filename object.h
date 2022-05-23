#ifndef OBJECT_H
#define OBJECT_H
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
    QString sprite_now;
    QString sprite_type_now;
    QString sprite_object_now;
    QString sprite_path;

    void one_tenth_sec_tic();

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


    virtual void move(int dx, int dy);
    virtual void draw() ;
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
#endif
