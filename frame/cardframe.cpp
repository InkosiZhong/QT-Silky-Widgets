#include "cardframe.h"
#include <QTreeWidgetItem>
#include <QtSvg/QSvgRenderer>
#include <QPainter>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

CardFrame::CardFrame(QWidget *parent) {
    QString styleSheet = ".QWidget {background-color: #ffffff; border-radius: 15px;}";
    setStyleSheet(styleSheet);
    QHBoxLayout * mainLayout = new QHBoxLayout();
    QVBoxLayout * vctLayout = new QVBoxLayout();
    QLabel* label = new QLabel();
    label->setStyleSheet("image:url(:/default)");
    label->setFixedSize(50, 50);
    mainLayout->addWidget(label);
    label = new QLabel("Card");
    label->setFont(QFont("Roboto", 12, 75));
    label->setStyleSheet("color: black");
    vctLayout->addWidget(label);
    label = new QLabel("Attributes");
    label->setFont(QFont("Roboto", 9));
    label->setStyleSheet("color: black");
    vctLayout->addWidget(label);
    mainLayout->addLayout(vctLayout);
    setLayout(mainLayout);
}

void CardFrame::setup(QString type, QString icon, QString color, QList<QMap<QString, QPair<QString, QString>>>* attributes){
    m_type = QPair<QString, QString>(type, icon);
    m_color = color;
    m_attributes.clear();
    if (attributes != nullptr){
        m_attributes = *attributes;
    }
    QString styleSheet = ".QWidget {background-color: " + color + "; border-radius: 15px;}";
    setStyleSheet(styleSheet);
    QLabel* label = qobject_cast<QLabel *>(layout()->itemAt(0)->widget());
    label->setStyleSheet("image:url(" + icon + ")");
    label = qobject_cast<QLabel *>(layout()->itemAt(1)->layout()->itemAt(0)->widget());
    label->setText(type);
    label->setStyleSheet("color: #F9F9F9;");
}
