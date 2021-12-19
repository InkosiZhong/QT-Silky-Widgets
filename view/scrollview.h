#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
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
    void setupAppearance(int borderWidth = 2, int radius = 17, QColor borderColor = "#999999", QColor backgroundColor = "#AAAAAA");
    bool get(int idx, QWidget*);
    bool getCurrent(QWidget*);
    bool getAll(QList<QWidget*>*);
    bool append(QWidget*);
    bool append(QList<QWidget*>*);
    bool remove(int idx);
    bool remove(QWidget*);
    bool removeCurrent();
    void clear();
    void bind(QWidget*);
    void bind(QList<QWidget*>*);
    void sever(QWidget*);
    void sever(QList<QWidget*>*);

    QWidget & operator<<(QWidget &w){ bind(&w); return *this; }
    QWidget & operator<<(QWidget *w){ bind(w); return *this; }
    QWidget & operator<<(QList<QWidget*> &wl){ bind(&wl); return *this; }
    QWidget & operator<<(QList<QWidget*> *wl){ bind(wl); return *this; }

    QWidget & operator>>(QWidget &w){ sever(&w); return *this; }
    QWidget & operator>>(QWidget *w){ sever(w); return *this; }
    QWidget & operator>>(QList<QWidget*> &wl){ sever(&wl); return *this; }
    QWidget & operator>>(QList<QWidget*> *wl){ sever(wl); return *this; }

    QWidget & operator+=(QWidget &w){ append(&w); return *this; }
    QWidget & operator+=(QWidget *w){ append(w); return *this; }
    QWidget & operator+=(QList<QWidget*> &wl){ append(&wl); return *this; }
    QWidget & operator+=(QList<QWidget*> *wl){ append(wl); return *this; }

    QWidget & operator-=(QWidget &w){ remove(&w); return *this; }
    QWidget & operator-=(QWidget *w){ remove(w); return *this; }

signals:
    void signalSwitchExhibitState(const ExhibitState state, const QPoint&, const QPoint&);
    void signalCopyWidget(QWidget* src, QWidget* widget);

public slots:
    void onDragWidget(const QPoint& center_point, QWidget* widget);
    void onDropWidget(const QPoint& center_point, QWidget* widget);
    void onCopyWidget(QWidget* src, QWidget* widget);

protected:
    enum PosType { POS_PREV = -1, POS_SHOW, POS_NEXT };
    QPoint getPos(PosType type);
    QWidget* getWidget(PosType type);
    void exhibit();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void initAnimation();
    void scrollPrev();
    void scrollNext();
    void scrollIn();
    void scrollOut();
    void fadeIn();

private:
    QList<QWidget *> m_widgetList;
    int m_currentIndex = 0;
    int m_borderWidth = 3;
    QAnimationGroup *m_scrollGroup;
    float m_zoomRate = 0.8;
    ScrollType m_scrollType = SCROLL_VERTICAL;
    bool m_add_mode = false;
};

#endif // SCROLLVIEW_H
