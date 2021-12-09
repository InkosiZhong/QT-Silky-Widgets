#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QDebug>

class ScrollView : public QWidget
{
public:
    ScrollView(QWidget *parent = nullptr);
    ~ScrollView();

public:
    void setupAnimation(float zoomRate = 0.8, int duration = 300);
    void setupAppearance(int borderWidth = 3, int radius = 15, QString borderColor = "#999999", QString backgroundColor = "#AAAAAA");
    bool append(QWidget*);
    bool append(QList<QWidget*>*);

    QWidget & operator<<(QWidget &w){ append(&w); return *this; }
    QWidget & operator<<(QWidget *w){ append(w); return *this; }
    QWidget & operator<<(QList<QWidget*> &wl){ append(&wl); return *this; }
    QWidget & operator<<(QList<QWidget*> *wl){ append(wl); return *this; }

protected:
    enum PosType { POS_PREV = -1, POS_SHOW, POS_NEXT };
    QPoint getPos(PosType type);
    QWidget* getWidget(PosType type);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void show();
    void initAnimation();
    void scrollPre();
    void scrollNext();

private:
    QList<QWidget *> m_widgetList;
    QAnimationGroup *m_animationGroup;
    int m_currentIndex = 0;
    float m_zoomRate = 0.8;
};

#endif // SCROLLVIEW_H
