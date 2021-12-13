#ifndef CARDFRAME_H
#define CARDFRAME_H

#include <QWidget>
#include <QMap>
#include <QPair>
#include <QMouseEvent>
#include <QAnimationGroup>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include "utils.h"

class CardFrame : public QFrame
{
public:
    CardFrame(QWidget *parent = nullptr);

public:
    void setup(QString type, QString icon, QColor color, QMap<QString, QPair<QString, QString>>* attributes = nullptr, QSize size = QSize(300, 100), QSize minSize = QSize(0, 0));

protected:
    enum ExhibitState{EX_LARGE, EX_SMALL};

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void initAnimation();
    void switchExhibitState(const ExhibitState state, const QPoint& p_ref);

private:
    QWidget* m_attrWidget;
    QPair<QString, QString> m_type; // name-icon pair
    QColor m_color;
    QMap<QString, QPair<QString, QString>> m_attributes; // key: value-icon
    QParallelAnimationGroup *m_shrinkAnimation, *m_enlargeAnimation;
    bool m_selected;
    QSize m_size;
    QSize m_minSize;
    ExhibitState m_state;
};

#endif // CARDFRAME_H
