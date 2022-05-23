#ifndef WIDGET_H


#include <QWidget>
#include <Qpainter>
#include <queue>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include "unit.h"
#include "object.h"
#include "sceleton.h"
#include "samurai.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void start_game();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void one_tenth();
    void redrawing();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Widget *ui;
    QTimer *one_tenth_sec_tic;
    QTimer *fps;
};





#endif // WIDGET_H
