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

class BidirectScrollView : public QWidget
{
public:
    BidirectScrollView(QWidget *parent = nullptr);
    ~BidirectScrollView();

public:
    void setupAnimation(float zoomRate = 0.8, int duration = 300);
    void setupAppearance(int borderWidth = 3, int radius = 15, QString borderColor = "#999999", QString backgroundColor = "#AAAAAA");
    bool append(QList<QList<QWidget *>>*);

    QWidget & operator<<(QList<QList<QWidget *>> &wl){ append(&wl); return *this; }
    QWidget & operator<<(QList<QList<QWidget *>> *wl){ append(wl); return *this; }

protected:
    enum PosType { POS_PREV = -1, POS_SHOW, POS_NEXT };
    QPoint getPos(PosType type);
    QWidget* getWidget(PosType type);
    enum ScrollType { SCROLL_HORIZONTAL, SCROLL_VERTICAL };

protected:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void show();
    void initAnimation();
    void scrollPre();
    void scrollNext();

private:
    QList<QList<QWidget *>> m_widgetList;
    int m_verticalIndex = 0, m_horizontalIndex = 0;
    QAnimationGroup *m_animationGroup;
    float m_zoomRate = 0.8;
    ScrollType m_scrollType = SCROLL_VERTICAL;
};

#endif // BIDIRECTSCROLLVIEW_H
