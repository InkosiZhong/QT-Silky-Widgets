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
}

MainWindow::~MainWindow()
{
    delete ui;
}

