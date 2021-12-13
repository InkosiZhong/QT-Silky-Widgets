#ifndef BIDIRECTSCROLLVIEW_H
#define BIDIRECTSCROLLVIEW_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPushButton>
#include <QWheelEvent>
#include <QKeyEvent>
#include <cmath>
#include "utils.h"

class BidirectScrollView : public QWidget
{
public:
    BidirectScrollView(QWidget *parent = nullptr);
    ~BidirectScrollView();

public:
    void setupAnimation(float zoomRate = 0.8, int duration = 300);
    void setupAppearance(int borderWidth = 3, int radius = 15, QColor borderColor = "#999999", QColor backgroundColor = "#AAAAAA");
    bool get(int v_idx, int h_idx, QWidget *);
    bool append(int idx, QWidget *);
    bool appendHere(QWidget *);
    bool append(int idx, QList<QWidget *>*);
    bool appendHere(QList<QWidget *>*);
    bool append(QList<QWidget *>*);
    bool append(QList<QList<QWidget *>>*);
    bool remove(int v_idx, int h_idx);
    bool removeCurrent();
    void clear();

    QWidget & operator<<(QList<QWidget *> &wl){ append(&wl); return *this; }
    QWidget & operator<<(QList<QWidget *> *wl){ append(wl); return *this; }
    QWidget & operator<<(QList<QList<QWidget *>> &wl){ append(&wl); return *this; }
    QWidget & operator<<(QList<QList<QWidget *>> *wl){ append(wl); return *this; }

protected:
    enum PosType { POS_PREV = -1, POS_SHOW, POS_NEXT };
    enum ScrollType { SCROLL_HORIZONTAL, SCROLL_VERTICAL };
    QPoint getPos(PosType pos_type, ScrollType scroll_type);
    QWidget* getWidget(PosType pos_type, ScrollType scroll_type);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void show();
    void initAnimation();
    void scrollPre(ScrollType type);
    void scrollNext(ScrollType type);

private:
    QList<QList<QWidget *>> m_widgetList;
    int m_verticalIndex = 0, m_horizontalIndex = 0;
    QAnimationGroup *m_animationGroup;
    float m_zoomRate = 0.8;
    bool m_shiftDown = false;
};

#endif // BIDIRECTSCROLLVIEW_H
