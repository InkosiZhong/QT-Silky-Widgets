#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QDebug>
#include "frame/cardframe.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->scrollView->setParent(this);
    ui->bidirectScrollView->setParent(this);
    ui->card->setParent(this);
    ui->card->setRestrict(this);
    *ui->scrollView << ui->card;
    for (int i = 0; i < 8; i++) {
        CardFrame *c = new CardFrame(ui->scrollView);
        QMap<QString, QPair<QString, QString>> map;
        map.insert("time", QPair<QString, QString>("2020", ":/time.svg"));
        map.insert("version", QPair<QString, QString>("5", ":/version.svg"));
        map.insert("hot", QPair<QString, QString>("2321", ":/hot.svg"));
        c->setup("CARD_" + QString::number(i), ":/playstation.svg", "#0080FF", &map, QSize(300, 100), QSize(150, 40));
        c->setRestrict(this);
        *ui->scrollView << c;
        *ui->scrollView += c;
    }
    //ui->scrollView->setupAnimation(0.8, 300, ScrollView::SCROLL_HORIZONTAL);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j <= i; j++){
            QPushButton *btn = new QPushButton(ui->bidirectScrollView);
            btn->setText(QString::number(i) + QString::number(j));
            btn->setVisible(false);
            btn->setStyleSheet(".QPushButton {background-color: #F6F6F6 ;border-radius: 15px;}");
            //bl << btn;
            ui->bidirectScrollView->append(i, btn);
        }
        //*ui->bidirectScrollView << bl;
    }
    QMap<QString, QPair<QString, QString>> map;
    map.insert("time", QPair<QString, QString>("2020", ":/time.svg"));
    map.insert("version", QPair<QString, QString>("5", ":/version.svg"));
    map.insert("hot", QPair<QString, QString>("2321", ":/hot.svg"));
    ui->card->setup("PlayStation", ":/playstation.svg", "#0080FF", &map, QSize(300, 100), QSize(150, 40));
}

MainWindow::~MainWindow()
{
    delete ui;
}

