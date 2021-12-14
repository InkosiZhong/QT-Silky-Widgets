#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPushButton>
#include <QWheelEvent>
#include <cmath>
#include "utils.h"

class ScrollView : public QWidget
{
    Q_OBJECT
public:
    ScrollView(QWidget *parent = nullptr);
    ~ScrollView();

public:
    enum ScrollType { SCROLL_HORIZONTAL, SCROLL_VERTICAL };
    void setupAnimation(float zoomRate = 0.8, int duration = 300, ScrollType type = SCROLL_VERTICAL);
    void setupAppearance(int borderWidth = 3, int radius = 15, QColor borderColor = "#999999", QColor backgroundColor = "#AAAAAA");
    bool get(int idx, QWidget*);
    bool getCurrent(QWidget*);
    bool getAll(QList<QWidget*>*);
    bool append(QWidget*);
    bool append(QList<QWidget*>*);
    bool remove(int idx);
    bool remove(QWidget*);
    bool removeCurrent();
    void clear();

    QWidget & operator<<(QWidget &w){ append(&w); return *this; }
    QWidget & operator<<(QWidget *w){ append(w); return *this; }
    QWidget & operator<<(QList<QWidget*> &wl){ append(&wl); return *this; }
    QWidget & operator<<(QList<QWidget*> *wl){ append(wl); return *this; }

public slots:
    void onCaptureWidget(const QPoint& center_point, QWidget* widget);

protected:
    enum PosType { POS_PREV = -1, POS_SHOW, POS_NEXT };
    QPoint getPos(PosType type);
    QWidget* getWidget(PosType type);
    void exhibit();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void initAnimation();
    void scrollPre();
    void scrollNext();

private:
    QList<QWidget *> m_widgetList;
    int m_currentIndex = 0;
    QAnimationGroup *m_animationGroup;
    float m_zoomRate = 0.8;
    ScrollType m_scrollType = SCROLL_VERTICAL;
};

#endif // SCROLLVIEW_H
