#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i = 0; i < 8; i++) {
        QPushButton *btn = new QPushButton(ui->scrollView);
        btn->setText(QString::number(i));
        btn->setVisible(false);
        btn->setStyleSheet(".QPushButton {background-color: #F6F6F6 ;border-radius: 15px;}");
        *ui->scrollView << btn;
    }
    for (int i = 0; i < 8; i++) {
        QPushButton *btn = new QPushButton(ui->scrollView2);
        btn->setText(QString::number(i));
        btn->setVisible(false);
        btn->setStyleSheet(".QPushButton {background-color: #F6F6F6 ;border-radius: 15px;}");
        *ui->scrollView2 << btn;
    }
    //ui->scrollView->setupAnimation(0.8, 300, ScrollView::SCROLL_HORIZONTAL);
    for (int i = 0; i < 8; i++) {
        QList<QWidget *> bl;
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

