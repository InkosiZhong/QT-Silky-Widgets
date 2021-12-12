#ifndef CARDFRAME_H
#define CARDFRAME_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QPair>

class CardFrame : public QWidget
{
public:
    CardFrame(QWidget *parent = nullptr);

public:
    void setup(QString type, QString icon, QString color, QList<QMap<QString, QPair<QString, QString>>>* attributes = nullptr);

private:
    QPair<QString, QString> m_type; // name-icon pair
    QColor m_color;
    QList<QMap<QString, QPair<QString, QString>>> m_attributes; // key: value-icon
};

#endif // CARDFRAME_H
