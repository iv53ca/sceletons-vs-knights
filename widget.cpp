#include "widget.h"
#include "unit.h"
#include "object.h"
#include "sceleton.h"
#include "samurai.h"
#include "ui_widget.h"
#include <QRandomGenerator>
#include <QUrl>
#include <QDir>
#include <windows.h>
#include <thread>
#include <random>
#include <chrono>

bool menu = true;
int x_buf;
bool game_end = false;
bool it_is_your_turn = true ;
int dx = 0;
int dy = 350;
int y_buf;
int cell_size = 128;
int fps_in_game = 60;
int casting_skill = -1;
int death_x;
int death_y;

int rand_uns(int min, int max)
{
        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        static std::default_random_engine e(seed);
        std::uniform_int_distribution<int> d(min, max);
        return d(e);
}

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
std::vector<Object*> objects;
Object pointer;
bool game_runs = false;
std::vector<std::pair<std::string,Unit*> > enemies;
std::vector<std::pair<std::string, Unit*> > your_units;
std::vector<std::vector<std::pair<std::string, Object*> > > map;
std::pair<std::string, Object*> obj_buf = std::pair<std::string, Object*> ("", NULL);
std::queue<std::pair<std::string, Object*> > dying_obj;
std::vector<std::pair<int, int>> lights;
std::vector<std::vector<QColor>>shadows;
int shadow_size = 10;
#include <cmath>
int w = 45;
void make_shadows() {
    if (rand_uns(0, 100) < 1 || w > 45) {
        if (w > 42) {
            w += rand_uns(0, 2) - 1;
        } else {
            w = 45;
}
    }
    for (int i = 0;i < shadows.size();++i) {
        for (int j = 0;j < shadows[i].size();++j) {
            int bright = 0;
            for (int k = 0;k < lights.size();++k) {
                if (w - std::pow(std::pow(abs(lights[k].first + dx - i * shadow_size) / shadow_size, 2) + std::pow(abs(lights[k].second + dy - j * shadow_size) / shadow_size, 2), 0.5) >= 0) {
                    bright += w - std::pow(std::pow(abs(lights[k].first + dx - i * shadow_size) / shadow_size, 2) + std::pow(abs(lights[k].second + dy - j * shadow_size) / shadow_size, 2), 0.5);
                }
            }
            if (bright > w) {
                bright = w;
            }

            shadows[i][j] = QColor(100/ w * bright + rand_uns(0, 10), 0 / w * bright , 2 / w * bright, 255 - 200 / w * bright);
        }
    }
}



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setMouseTracking(true);
    ui->pushButton->hide();
    //m->setPlaybackMode(QMediaPlaylist::Random);
  }

void Widget::start_game() {
    //m->setPlaybackMode(QMediaPlaylist::Random);
    game_runs = true;
    pointer.sprite_path = "sprites/pointing.PNG";
    map.resize(15, std::vector<std::pair<std::string, Object*> >(10));
    ui->pushButton->show();
    ui->pushButton_2->hide();
    lights.push_back(std::pair<int, int>(2 * cell_size, cell_size));
    lights.push_back(std::pair<int, int>(6 * cell_size, cell_size));
    lights.push_back(std::pair<int, int>(10 * cell_size, cell_size));
    lights.push_back(std::pair<int, int>(14 * cell_size, cell_size));
    lights.push_back(std::pair<int, int>(2 * cell_size, cell_size * 5));
    lights.push_back(std::pair<int, int>(6 * cell_size, cell_size * 5));
    lights.push_back(std::pair<int, int>(10 * cell_size, cell_size * 5));
    lights.push_back(std::pair<int, int>(14 * cell_size, cell_size * 5));
    lights.push_back(std::pair<int, int>(2 * cell_size, cell_size * 9));
    lights.push_back(std::pair<int, int>(6 * cell_size, cell_size * 9));
    lights.push_back(std::pair<int, int>(10 * cell_size, cell_size * 9));
    lights.push_back(std::pair<int, int>(14 * cell_size, cell_size * 9));
    shadows.resize(250,std::vector<QColor>(130));
    map[7][4] = std::pair<std::string, Object*>("tree", new Object("3", "tree", "s", 7 * cell_size, 4 * cell_size - cell_size / 2));
    map[7][4].second->right = true;
    map[7][4].second->one_tenth_sec_tic();
    map[7][5] = std::pair<std::string, Object*>("tree", new Object("", "", "s", 7 * cell_size, 5 * cell_size - cell_size / 2));
    map[8][5] = std::pair<std::string, Object*>("tree", new Object("", "", "s", 8 * cell_size, 5 * cell_size - cell_size / 2));
    map[8][4] = std::pair<std::string, Object*>("tree", new Object("", "", "s", 8 * cell_size, 4 * cell_size - cell_size / 2));
    map[14][4] = std::pair<std::string, Object*>("Samurai", new Samurai());

    dynamic_cast<Unit*>(map[14][4].second)->x = 14 * cell_size;
    dynamic_cast<Unit*>(map[14][4].second)->enemy = false;
    dynamic_cast<Unit*>(map[14][4].second)->y = 4 * cell_size - cell_size / 2;
    dynamic_cast<Unit*>(map[14][4].second)->one_turn_tic();
    your_units.push_back(std::pair<std::string, Unit*>("Samurai", dynamic_cast<Unit*>(map[14][4].second)));
    map[14][5] = std::pair<std::string, Object*>("Samurai", new Samurai());
    dynamic_cast<Unit*>(map[14][5].second)->x = cell_size * 14;
    dynamic_cast<Unit*>(map[14][5].second)->enemy = false;
    dynamic_cast<Unit*>(map[14][5].second)->y = 5 * cell_size - cell_size / 2;
    dynamic_cast<Unit*>(map[14][5].second)->one_turn_tic();
    your_units.push_back(std::pair<std::string, Unit*>("Samurai", dynamic_cast<Unit*>(map[14][5].second)));

    map[0][8] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][8].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][8].second)->y = 8 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][8].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][8].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][8].second)));
    map[0][7] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][7].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][7].second)->y = 7 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][7].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][7].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][7].second)));
    map[0][6] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][6].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][6].second)->y = 6 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][6].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][6].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][6].second)));
    map[0][5] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][5].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][5].second)->y = 5 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][5].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][5].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][5].second)));
    map[0][4] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][4].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][4].second)->y = 4 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][4].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][4].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][4].second)));
    map[0][3] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][3].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][3].second)->y = 3 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][3].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][3].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][3].second)));
    map[0][2] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][2].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][2].second)->y = 2 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][2].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][2].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][2].second)));
    map[0][1] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][1].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][1].second)->y = 1 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][1].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][1].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][1].second)));
    map[0][0] = std::pair<std::string, Object*>("Skeleton", new Skeleton());
    dynamic_cast<Unit*>(map[0][0].second)->x = 0 * cell_size;
    dynamic_cast<Unit*>(map[0][0].second)->y = 0 * cell_size - cell_size/2;
    dynamic_cast<Unit*>(map[0][0].second)->one_turn_tic();
    dynamic_cast<Unit*>(map[0][0].second)->enemy = true;
    enemies.push_back(std::pair<std::string, Unit*>("Skeleton", dynamic_cast<Unit*>(map[0][0].second)));

    one_tenth_sec_tic = new QTimer();
    connect(one_tenth_sec_tic, SIGNAL(timeout()), this, SLOT(one_tenth()));
    one_tenth_sec_tic->start(100);
    fps = new QTimer();
    connect(fps, SIGNAL(timeout()), this, SLOT(redrawing()));
    fps->start(1000 / fps_in_game);
}

bool k = true;

void Widget::one_tenth() {
    if (obj_buf.second  != NULL) {
        dynamic_cast<Unit*>(obj_buf.second)->one_tenth_sec_tic();

    }
    if(game_end && k) {
        k = false;

    }
    if (it_is_your_turn) {
        ui->pushButton->show();
    }
    for (std::vector<Object*>::iterator i = objects.begin();i < objects.end();++i) {
        (*i)->draw();
        if ((*i)->sprite_now == "16") {
            delete *i;
            objects.erase(i);
        }
    }
    if (!dying_obj.empty()) {
        dynamic_cast<Unit*>(dying_obj.front().second)->one_tenth_sec_tic();
    }
    for (int i = 0;i < 15;++i)
        for (int j = 0;j < 10;++j) {
            if (map[i][j].second != NULL) {
                if (map[i][j].first != "tree") {
                    dynamic_cast<Unit*>(map[i][j].second)->one_tenth_sec_tic();
                } else {
                    (map[i][j].second)->one_tenth_sec_tic();
                }
            }
        }
}



void delete_in_mas(Unit* a) {
    for (int i = 0;i < enemies.size();++i) {
        if (enemies[i].second == a) {
            enemies.erase(enemies.begin() + i);
            return;
        }
    }
    for (int i = 0;i < your_units.size();++i) {
        if (your_units[i].second == a) {
            your_units.erase(your_units.begin() + i);
            return;
        }
    }
}

void Widget::redrawing() {
    if (casting_skill > 0) {
        pointer.draw();
    }
    for (int i = 0;i < 15;++i) {
        for (int j = 0;j < 10;++j) {
            if (map[i][j].second != NULL) {
                map[i][j].second->draw();
                if (map[i][j].first != "tree") {
                if (dynamic_cast<Unit*>(map[i][j].second)->sprite_type_now == "attack" && dynamic_cast<Unit*>(map[i][j].second)->sprite_now == "15") {
                    dynamic_cast<Unit*>(map[i][j].second)->sprite_type_now = "stand";
                }
                if (dynamic_cast<Unit*>(map[i][j].second)->sprite_type_now == "first_skill" && dynamic_cast<Unit*>(map[i][j].second)->sprite_now == "15") {
                    dynamic_cast<Unit*>(map[i][j].second)->sprite_type_now = "stand";
                }
                if (dynamic_cast<Unit*>(map[i][j].second)->sprite_type_now == "getdmg" && dynamic_cast<Unit*>(map[i][j].second)->sprite_now == "8") {
                    dynamic_cast<Unit*>(map[i][j].second)->sprite_type_now = "stand";
                    dynamic_cast<Unit*>(map[i][j].second)->sprite_now = "1";
                }
                }
            }
        }
    }
    for (int i = 0;i < 0;++i) {

    }

    if (!dying_obj.empty()) {
        dynamic_cast<Object*>(dying_obj.front().second)->draw();
        if (dynamic_cast<Unit*>(dying_obj.front().second)->sprite_now == "16") {
            delete_in_mas(dynamic_cast<Unit*>(dying_obj.front().second));
            dying_obj.pop();
        }
    }

    if (obj_buf.second  != NULL) {
        ui->hp->setText(QString::number(dynamic_cast<Unit*>(obj_buf.second)->hp) + " / " + QString::number(dynamic_cast<Unit*>(obj_buf.second)->max_hp));
        ui->mp->setText(QString::number(dynamic_cast<Unit*>(obj_buf.second)->mp) + " / " + QString::number(dynamic_cast<Unit*>(obj_buf.second)->max_mp));
        obj_buf.second->draw();
    }



    repaint();
}



void Widget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(QColor(qRgba(0,0,0,0)));
    painter.drawImage(dx, dy-360, QImage("sprites/wall.PNG"));
    painter.drawImage(dx, dy, QImage("sprites/phone.PNG"));
    while (!draw_queue.empty()) {
        draw_queue.front().img.size();
        if (!draw_queue.front().right) {
            painter.drawImage(draw_queue.front().x + dx + cell_size - draw_queue.front().img.width(), draw_queue.front().y + dy, draw_queue.front().img.mirrored(!draw_queue.front().right, 0));
        } else {
            painter.drawImage(draw_queue.front().x + dx, draw_queue.front().y + dy, draw_queue.front().img.mirrored(!draw_queue.front().right, 0));
        }
        draw_queue.pop();
    }

    make_shadows();
    for (int i = 0;i < shadows.size();++i) {
        for (int j = 0;j < shadows[i].size();++j) {
            painter.fillRect(i * shadow_size, j * shadow_size, shadow_size, shadow_size, QBrush(shadows[i][j]));
        }
    }
    if (obj_buf.second != NULL) {
        ui->hp->show();
        ui->mp->show();
        ui->name->show();
        painter.setBrush(QColor(qRgb(58,126,85)));
        painter.drawRect(0,0, 240, 80);
        painter.setBrush(QColor(qRgb(220, 20, 60)));
        painter.drawRect(20,20, 200 * dynamic_cast<Unit*>(obj_buf.second)->hp / dynamic_cast<Unit*>(obj_buf.second)->max_hp, 20);
        painter.setBrush(QColor(qRgb(30, 144, 255)));
        painter.drawRect(20,40, 200 * dynamic_cast<Unit*>(obj_buf.second)->mp / dynamic_cast<Unit*>(obj_buf.second)->max_mp, 20);
    } else {
        ui->hp->hide();
        ui->mp->hide();
        ui->name->hide();
    }
    if (menu) {
        painter.drawImage(0,0,QImage("sprites/menu.PNG"));
    }
    if (game_end) {
        painter.drawImage(356, 0, QImage("sprites/youdied.PNG"));
    }

}

bool moving = false;

void Widget::mousePressEvent(QMouseEvent *event) {

    if (it_is_your_turn && game_runs) {
            x_buf = event->x() - dx;
            y_buf = event->y() - dy;
        if (map[x_buf / cell_size][(y_buf + cell_size/2) / cell_size].first != "tree") {
        if (x_buf >= 0 && y_buf >= 0&& x_buf /cell_size < 15 && (y_buf + cell_size / 2) / cell_size < 10) {
        if (event->button() == 1) {
            if (map[x_buf /cell_size][y_buf/cell_size].second != NULL) {
                if (!dynamic_cast<Unit*>(map[x_buf /cell_size][y_buf/cell_size].second)->enemy && dynamic_cast<Unit*>(map[x_buf /cell_size][y_buf/cell_size].second)->stuned == 0) {
                    obj_buf = map[x_buf/cell_size][y_buf/cell_size];
                    dynamic_cast<Unit*>(obj_buf.second)->sprite_type_now = "move";
                    map[x_buf/cell_size][y_buf/cell_size].second = NULL;
                    map[x_buf/cell_size][y_buf/cell_size].first = "";
                    moving = true;
                }
            }
        } else {
            if (casting_skill == -1) {
                x_buf = event->x() - dx;
                y_buf = event->y() - dy;
                obj_buf = map[x_buf/cell_size][y_buf/cell_size];
                ui->name->setText(QString::fromStdString(obj_buf.first));
            } else {
                if (obj_buf.first == "Skeleton") {
                    if (casting_skill == 1&&obj_buf.second != NULL && map[x_buf/cell_size][y_buf/cell_size].second != NULL &&dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->enemy
                            && dynamic_cast<Skeleton*>(obj_buf.second)->range1 >=
                            abs(x_buf/cell_size - obj_buf.second->x/cell_size) + abs(y_buf/cell_size - obj_buf.second->y/cell_size)) {
                        if (map[x_buf/cell_size][y_buf/cell_size].second != obj_buf.second && map[x_buf/cell_size][y_buf/cell_size].second != NULL) {
                            objects.push_back(new Object("1", "1", "hit", x_buf/cell_size * cell_size, y_buf/cell_size * cell_size));
                            dynamic_cast<Unit*>(obj_buf.second)->sprite_type_now = "attack";
                            dynamic_cast<Unit*>(obj_buf.second)->sprite_now = "1";
                            obj_buf.second->right = obj_buf.second->x > x_buf;
                            if(dynamic_cast<Skeleton*>(obj_buf.second)->first_skill(dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second))) {
                                dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->sprite_type_now = "death";
                                dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->sprite_now = "1";
                                dying_obj.push(map[x_buf/cell_size][y_buf/cell_size]);
                                map[x_buf/cell_size][y_buf/cell_size].second = NULL;
                                map[x_buf/cell_size][y_buf/cell_size].first = "";
                            } else {

                            }
                        }
                    } else
                    if (casting_skill == 5&&obj_buf.second != NULL && map[x_buf/cell_size][y_buf/cell_size].second != NULL &&!dynamic_cast<Unit*>(obj_buf.second)->attacked && dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->enemy
                            && dynamic_cast<Skeleton*>(obj_buf.second)->range5 >=
                            abs(x_buf/cell_size - obj_buf.second->x/cell_size) + abs((y_buf + cell_size / 2)/cell_size - (obj_buf.second->y + cell_size / 2)/cell_size)) {
                        if (map[x_buf/cell_size][y_buf/cell_size].second != obj_buf.second && map[x_buf/cell_size][y_buf/cell_size].second != NULL) {
                            dynamic_cast<Unit*>(obj_buf.second)->sprite_type_now = "attack";
                            dynamic_cast<Unit*>(obj_buf.second)->sprite_now = "1";
                            obj_buf.second->right = obj_buf.second->x > x_buf;
                            if(dynamic_cast<Skeleton*>(obj_buf.second)->attack(dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second))) {
                                dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->sprite_type_now = "death";
                                dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->sprite_now = "1";
                                dying_obj.push(map[x_buf/cell_size][y_buf/cell_size]);
                                map[x_buf/cell_size][y_buf/cell_size].second = NULL;
                                map[x_buf/cell_size][y_buf/cell_size].first = "";
                            }
                        }
                    }
                }
                if (obj_buf.first == "Samurai") {
                    if (casting_skill == 1&&obj_buf.second != NULL && map[x_buf/cell_size][y_buf/cell_size].second != NULL &&dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->enemy
                            && dynamic_cast<Samurai*>(obj_buf.second)->range1 >=
                            abs(x_buf/cell_size - obj_buf.second->x/cell_size) + abs((y_buf + cell_size/2)/cell_size - (obj_buf.second->y + cell_size/2)/cell_size)) {
                        if (map[x_buf/cell_size][y_buf/cell_size].second != obj_buf.second && map[x_buf/cell_size][y_buf/cell_size].second != NULL) {
                            objects.push_back(new Object("1", "1", "hit", x_buf/cell_size * cell_size, y_buf/cell_size * cell_size));
                            dynamic_cast<Unit*>(obj_buf.second)->sprite_type_now = "first_skill";
                            dynamic_cast<Unit*>(obj_buf.second)->sprite_now = "1";
                            obj_buf.second->right = obj_buf.second->x <= x_buf;
                            if(dynamic_cast<Samurai*>(obj_buf.second)->first_skill(dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second))) {
                                dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->sprite_type_now = "death";
                                dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->sprite_now = "1";
                                dying_obj.push(map[x_buf/cell_size][y_buf/cell_size]);
                                map[x_buf/cell_size][y_buf/cell_size].second = NULL;
                                map[x_buf/cell_size][y_buf/cell_size].first = "";
                            } else {

                            }
                        }
                    } else
                    if (casting_skill == 5&&obj_buf.second != NULL && !dynamic_cast<Unit*>(obj_buf.second)->attacked && map[x_buf/cell_size][y_buf/cell_size].second != NULL &&  dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->enemy
                            && dynamic_cast<Samurai*>(obj_buf.second)->range5 >=
                            abs(x_buf/cell_size - obj_buf.second->x/cell_size) + abs((y_buf + cell_size/2)/cell_size - (obj_buf.second->y + cell_size/2)/cell_size)) {
                        if (map[x_buf/cell_size][y_buf/cell_size].second != obj_buf.second && map[x_buf/cell_size][y_buf/cell_size].second != NULL) {
                            dynamic_cast<Unit*>(obj_buf.second)->sprite_type_now = "attack";
                            dynamic_cast<Unit*>(obj_buf.second)->sprite_now = "1";
                            obj_buf.second->right = obj_buf.second->x <= x_buf;
                            if(dynamic_cast<Samurai*>(obj_buf.second)->attack(dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second))) {
                                dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->sprite_type_now = "death";
                                dynamic_cast<Unit*>(map[x_buf/cell_size][y_buf/cell_size].second)->sprite_now = "1";
                                dying_obj.push(map[x_buf/cell_size][y_buf/cell_size]);
                                map[x_buf/cell_size][y_buf/cell_size].second = NULL;
                                map[x_buf/cell_size][y_buf/cell_size].first = "";
                            }
                        }
                    }
                }

            }
            casting_skill = -1;
        }
        }
        }
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event) {
    if (it_is_your_turn) {
        if (moving) {

            if (obj_buf.second != NULL) {
                obj_buf.second->x = event->x() - dx - cell_size / 2;
                obj_buf.second->y = event->y() - dy - cell_size / 2;
            }
        }
            pointer.x = ((event->x() - dx) / cell_size) * cell_size;
            pointer.y = ((event->y() - dy) / cell_size) * cell_size;
    }

}

void Widget::mouseReleaseEvent(QMouseEvent *event) {
    if (it_is_your_turn ) {
        if (obj_buf.second != NULL && moving) {

            if (event->x() - dx>= 0&&event->x() - dx < 128 * 15 && event->y() - dy > 0 && event->y() - dy < 128 * 10 && map[(event->x() - dx)/cell_size][(event->y() - dy)/cell_size].second == NULL && abs((x_buf / cell_size) - ((event->x()-dx) / cell_size)) + abs((y_buf / cell_size) - ((event->y() - dy) / cell_size)) < dynamic_cast<Unit*>(obj_buf.second)->moves_left) {
                map[(event->x() - dx)/cell_size][(event->y() - dy)/cell_size] = obj_buf;
                obj_buf.second->x =((event->x() - dx) / cell_size) * cell_size;
                dynamic_cast<Unit*>(obj_buf.second)->moves_left -= abs((x_buf / cell_size) - ((event->x() - dx) / cell_size)) + abs((y_buf / cell_size) - ((event->y()-dy) / cell_size));
                obj_buf.second->y =((event->y() - dy) / cell_size) * cell_size - cell_size / 2;
                dynamic_cast<Unit*>(obj_buf.second)->sprite_type_now = "stand";
                obj_buf.second = NULL;
            } else {
                map[(x_buf)/cell_size][(y_buf)/cell_size] = obj_buf;
                dynamic_cast<Unit*>(obj_buf.second)->sprite_type_now = "stand";
                obj_buf.second->x =(x_buf / cell_size) * cell_size;
                obj_buf.second->y =(y_buf / cell_size) * cell_size - cell_size / 2;
                obj_buf.second = NULL;

            }
        }
        moving = false;
    }

}

void Widget::keyPressEvent(QKeyEvent *event) {
    if (it_is_your_turn) {
        if (event->key() == Qt::Key::Key_1) {
            casting_skill = 1;
        }
        if (event->key() == Qt::Key::Key_2) {
            casting_skill = 2;
        }
        if (event->key() == Qt::Key::Key_3) {
            casting_skill = 3;
        }
        if (event->key() == Qt::Key::Key_4) {
            casting_skill = 4;
        }
        if (event->key() == Qt::Key::Key_Q) {
            casting_skill = 5;
        }

        if (event->key() == Qt::Key::Key_W) {
            if (dy <= 355) {
                dy += 10;
            }
        }
        if (event->key() == Qt::Key::Key_S) {
            if (dy >= -190) {
                dy -= 10;
            }
        }
        if (event->key() == Qt::Key::Key_A) {
                    dx += 10;
                }
                if (event->key() == Qt::Key::Key_D) {
                    dx -= 10;
                }
    }
}


bool attack_for_bot(int xa, int ya, int x, int y) {// xa meanes attacked
    if (map[xa][ya].second != NULL && map[x][y].second != NULL&&!(xa == x && ya == y)) {
        int range = abs(xa - x) + abs(ya - y);
        if (range <= dynamic_cast<Unit*>(map[x][y].second)->range5) {
            map[x][y].second->right = x <= xa;
            dynamic_cast<Unit*>(map[x][y].second)->sprite_type_now = "attack";
            dynamic_cast<Unit*>(map[x][y].second)->sprite_now = "1";
            if (map[x][y].first == "Skeleton") {
                if (dynamic_cast<Skeleton*>(map[x][y].second)->attack(dynamic_cast<Unit*>(map[xa][ya].second))) {
                    dynamic_cast<Unit*>(map[xa][ya].second)->sprite_type_now = "death";
                    dynamic_cast<Unit*>(map[xa][ya].second)->sprite_now = "1";
                    dying_obj.push(map[xa][ya]);
                    map[xa][ya].second = NULL;
                    map[xa][ya].first = "";
                }
            }
            if (map[x][y].first == "Samurai") {
                if (dynamic_cast<Samurai*>(map[x][y].second)->attack(dynamic_cast<Unit*>(map[xa][ya].second))) {
                    dynamic_cast<Unit*>(map[xa][ya].second)->sprite_type_now = "death";
                    dynamic_cast<Unit*>(map[xa][ya].second)->sprite_now = "1";
                    dying_obj.push(map[xa][ya]);
                    map[xa][ya].second = NULL;
                    map[xa][ya].first = "";
                }
            }
            return true;
        }
    } else {
        return false;
    }
}

bool use_first_skill_for_bot(int xa, int ya, int x, int y) {
    if (map[xa][ya].second != NULL && map[x][y].second != NULL) {
        int range = abs(xa - x) + abs(ya - y);
        if (range <= dynamic_cast<Unit*>(map[x][y].second)->range5) {
            dynamic_cast<Unit*>(map[x][y].second)->sprite_type_now = "attack";
            dynamic_cast<Unit*>(map[x][y].second)->sprite_now = "1";
            map[x][y].second->right = x <= xa;
            if (map[x][y].first == "Skeleton") {
                if (dynamic_cast<Skeleton*>(map[x][y].second)->first_skill(dynamic_cast<Unit*>(map[xa][ya].second))) {
                    dynamic_cast<Unit*>(map[xa][ya].second)->sprite_type_now = "death";
                    dynamic_cast<Unit*>(map[xa][ya].second)->sprite_now = "1";
                    dying_obj.push(map[xa][ya]);
                    map[xa][ya].second = NULL;
                    map[xa][ya].first = "";
                }
            }
            if (map[x][y].first == "Samurai") {
                if (dynamic_cast<Samurai*>(map[x][y].second)->first_skill(dynamic_cast<Unit*>(map[xa][ya].second))) {
                    dynamic_cast<Unit*>(map[xa][ya].second)->sprite_type_now = "death";
                    dynamic_cast<Unit*>(map[xa][ya].second)->sprite_now = "1";
                    dying_obj.push(map[xa][ya]);
                    map[xa][ya].second = NULL;
                    map[xa][ya].first = "";
                }
            }
            return true;
        }
    } else {
        return false;
    }
}

bool move_unit_for_bot(int xs, int ys, int x, int y) {
    Unit* moving_unit = dynamic_cast<Unit*>(map[xs][ys].second);
    if (x >= 0&& x <= 9&&y >= 0&&y <= 9)
    if (moving_unit != NULL && map[x][y].second == NULL && moving_unit->enemy && moving_unit->stuned == 0) {
        if (abs(xs - x) + abs(ys - y) <= moving_unit->moves_left) {
            if (xs >= 0&& xs <= 9&&ys >= 0&&ys <= 9) {
                moving_unit->moves_left -= abs(xs - x) + abs(ys - y);
                map[x][y] = std::pair<std::string, Object*>(map[xs][ys].first, dynamic_cast<Object*>(moving_unit));
                moving_unit->x = x * cell_size;
                moving_unit->y = y * cell_size - cell_size / 2;
                map[xs][ys].first = "";
                map[xs][ys].second = NULL;
                return true;
            }
        }
    }
    return false;
}

int danger(Unit* enemy, Unit* you, int x, int y) {

    int d = 0;
    if (abs(x - enemy->x/cell_size) + abs(y - (enemy->y + cell_size / 2)/cell_size) == 1) {
        if (you->dmg >= enemy->hp) {
            d -= 10000;
        }
        d-= 100000;
        if (you->hp + you->hp_reg <= enemy->dmg * 2) {
            d += 1000;
        }
    }
    if (enemy->moves_max >= abs(x - enemy->x/cell_size) + abs(y - (enemy->y + cell_size / 2)/cell_size)) {
        if (you->hp + you->hp_reg <= enemy->dmg) {
            d += 1000;
        }
    }
    return d - rand_uns(0, 1000);
}

std::pair<int,int> where_go(int x, int y, Unit* you) {
    int x_op = 0;
    int y_op = 0;
    int d_op = 1000000;
    for (int i = 0;i <= you->moves_left;++i) {
        for (int j = 0;j <= you->moves_left;++j) {
            int d = 0;
            for (int k = 0;k < your_units.size();++k) {
                if (x + i >= 0&&y + j  >= 0&& y + j < 10 && x + i < 15)
                    d += danger(your_units[k].second, you, x + i, y + j);

            }
            if (x + i >= 0&&y + j  >= 0&& y + j < 10 && x + i < 15)
            if (d < d_op && map[x + i][y + j].second == NULL) {
                x_op = x + i;
                y_op = y + j;
                d_op = d;
            }
            d = 0;
            for (int k = 0;k < your_units.size();++k) {
                if (x - i >= 0&&y + j  >= 0&& y + j < 10 && x - i < 15)
                d += danger(your_units[k].second, you, x - i, y + j);
            }
            if (x - i >= 0&&y + j  >= 0&& y + j < 10 && x - i < 15)
            if (d < d_op && map[x - i][y + j].second == NULL) {
                x_op = x - i;
                y_op = y + j;
                d_op = d;
            }
            d = 0;
            for (int k = 0;k < your_units.size();++k) {
                if (x - i >= 0 &&y - j  >= 0&& y - j < 10 && x - i < 15)
                d += danger(your_units[k].second, you, x - i, y - j);
            }
            if (x - i >= 0 &&y - j  >= 0&& y - j < 10 && x - i < 15)
            if (d < d_op && map[x - i][y - j].second == NULL) {
                x_op = x - i;
                y_op = y - j;
                d_op = d;
            }
            d = 0;
            for (int k = 0;k < your_units.size();++k) {
                if (x + i >= 0&&y - j  >= 0&& y - j < 10 && x + i < 15)
                d += danger(your_units[k].second, you, x + i, y - j);
            }
            if (x + i >= 0&&y - j  >= 0&& y - j < 10 && x + i < 15)
            if (d < d_op && map[x + i][y - j].second == NULL) {
                x_op = x + i;
                y_op = y - j;
                d_op = d;
            }
        }
    }
    return std::pair<int, int>(x_op, y_op);
}

struct action {
    int x;
    int y;
    int type;
    action(int x_s, int y_s, int type_s) {
        x = x_s;
        y = y_s;
        type = type_s;
    }
};


int attack_prior(Unit* you) {
    int min_hp = 10000;
    int a = 0;
    if ((you->y + cell_size / 2) / cell_size + 1 < 10)
    if (map[you->x / cell_size][(you->y + cell_size / 2) / cell_size + 1].second != NULL) {
        if (map[you->x / cell_size][(you->y + cell_size / 2) / cell_size + 1].first != "tree")
        if (!dynamic_cast<Unit*>(map[you->x / cell_size][(you->y + cell_size / 2) / cell_size + 1].second)->enemy)
        if (dynamic_cast<Unit*>(map[you->x / cell_size][(you->y + cell_size / 2) / cell_size + 1].second)->hp < min_hp) {
            min_hp = dynamic_cast<Unit*>(map[you->x / cell_size][(you->y + cell_size / 2) / cell_size + 1].second)->hp;
            a = 1;
        }
    }
    if ((you->y + cell_size / 2) / cell_size - 1 >= 0)
    if (map[you->x / cell_size][(you->y + cell_size / 2) / cell_size - 1].second != NULL) {
        if (map[you->x / cell_size][(you->y + cell_size / 2) / cell_size - 1].first != "tree")
        if (!dynamic_cast<Unit*>(map[you->x / cell_size][(you->y + cell_size / 2) / cell_size - 1].second)->enemy)
        if (dynamic_cast<Unit*>(map[you->x / cell_size][(you->y + cell_size / 2) / cell_size - 1].second)->hp < min_hp) {
            min_hp = dynamic_cast<Unit*>(map[you->x / cell_size][(you->y + cell_size / 2) / cell_size - 1].second)->hp;
            a = 2;
        }
    }
    if ((you->x) / cell_size - 1 >= 0)
    if (map[you->x / cell_size - 1][(you->y + cell_size / 2) / cell_size].second != NULL) {
        if (map[you->x / cell_size - 1][(you->y + cell_size / 2) / cell_size].first != "tree")
        if (!dynamic_cast<Unit*>(map[you->x / cell_size - 1][(you->y + cell_size / 2) / cell_size].second)->enemy)
        if (dynamic_cast<Unit*>(map[you->x / cell_size - 1][(you->y + cell_size / 2) / cell_size].second)->hp < min_hp) {
            min_hp = dynamic_cast<Unit*>(map[you->x / cell_size - 1][(you->y + cell_size / 2) / cell_size].second)->hp;
            a = 3;
        }
    }
    if ((you->x) / cell_size + 1 < 15)
    if (map[you->x / cell_size + 1][(you->y + cell_size / 2) / cell_size].second != NULL) {
        if (map[you->x / cell_size + 1][(you->y + cell_size / 2) / cell_size].first != "tree")
        if (!dynamic_cast<Unit*>(map[you->x / cell_size + 1][(you->y + cell_size / 2) / cell_size].second)->enemy)
        if (dynamic_cast<Unit*>(map[you->x / cell_size + 1][(you->y + cell_size / 2) / cell_size].second)->hp < min_hp) {
            min_hp = dynamic_cast<Unit*>(map[you->x / cell_size + 1][(you->y + cell_size / 2) / cell_size].second)->hp;
            a = 4;
        }
    }
    return a;
}

action what_to_do() {
    return action(0,0,1);
}

void bots_move() {
    if (!your_units.empty()) {
        for (int i = 0;i < enemies.size();++i) {
            std::pair<int,int>move = where_go(enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2) / cell_size, enemies[i].second);
            move_unit_for_bot((enemies[i].second->x) / cell_size, (enemies[i].second->y + cell_size / 2) / cell_size, move.first, move.second);
            Sleep(500);
            if(attack_prior(enemies[i].second) == 1) {
                attack_for_bot(enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size + 1, enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size);
                Sleep(1000);
                use_first_skill_for_bot(enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size + 1, enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size);
            }
            if(attack_prior(enemies[i].second) == 2) {
                attack_for_bot(enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size - 1, enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size);
                Sleep(1000);
                use_first_skill_for_bot(enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size - 1, enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size);

            }
            if(attack_prior(enemies[i].second) == 3) {
                attack_for_bot(enemies[i].second->x / cell_size - 1, (enemies[i].second->y + cell_size / 2)/ cell_size, enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size);
                Sleep(1000);
                use_first_skill_for_bot(enemies[i].second->x / cell_size - 1, (enemies[i].second->y + cell_size / 2)/ cell_size, enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size);
            }
            if(attack_prior(enemies[i].second) == 4) {
                attack_for_bot(enemies[i].second->x / cell_size + 1, (enemies[i].second->y + cell_size / 2)/ cell_size, enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size);
                Sleep(1000);
                use_first_skill_for_bot(enemies[i].second->x / cell_size + 1, (enemies[i].second->y + cell_size / 2)/ cell_size, enemies[i].second->x / cell_size, (enemies[i].second->y + cell_size / 2)/ cell_size);
            }
            Sleep(100);
            if (your_units.empty()) {
                game_runs = false;



                while (!lights.empty()) {
                    lights.pop_back();
                    Sleep(200);

                }
                game_end = true;
            }
        }
    } else {
        game_runs = false;
        while (!lights.empty()) {
            lights.pop_back();
            Sleep(200);

        }
        game_end = true;
    }
    it_is_your_turn = true;
}


Widget::~Widget()
{
    delete ui;
}

void one_turn_tic() {
    for (int i = 0;i < 15;++i)
        for (int j = 0;j < 10;++j) {
            if (map[i][j].second != NULL) {
                if (map[i][j].first != "tree") {
                dynamic_cast<Unit*>(map[i][j].second)->one_turn_tic();
                }
            }
        }
}

void Widget::on_pushButton_clicked()
{
    std::thread t(bots_move);
    ui->pushButton->hide();
    it_is_your_turn = false;
    one_turn_tic();
    t.detach();

}

void Widget::on_pushButton_2_clicked()
{
    menu = false;
    start_game();
}
