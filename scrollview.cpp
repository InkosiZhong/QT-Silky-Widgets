#include "scrollview.h"
#include <QPushButton>
#include <QWheelEvent>

ScrollView::ScrollView(QWidget *parent){
    m_animationGroup = new QParallelAnimationGroup(this);
    initAnimation();
    setFixedSize(size());
    setupAnimation();
    setupAppearance();
}

ScrollView::~ScrollView() {}

void ScrollView::setupAnimation(float zoomRate, int duration){
    m_zoomRate = fmax(fmin(zoomRate, 1), 0);
    duration = fmax(duration, 0);
    for (int i = 0; i < 2; i++) {
        QSequentialAnimationGroup* resizeGroup = static_cast<QSequentialAnimationGroup *>(m_animationGroup->animationAt(i));
        QPropertyAnimation* a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(0));
        a->setDuration(duration / 2);
        a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(1));
        a->setDuration(duration / 2);
    }
}

void ScrollView::setupAppearance(int borderWidth, int radius, QString borderColor, QString backgroundColor){
    borderWidth = fmax(fmin(borderWidth, 3), 0);
    radius = fmax(radius, 0);
    QString styleSheet = ".QWidget {border: " + QString::number(borderWidth) + "px solid " + borderColor + "; " +
                                   "background-color: " + backgroundColor + "; " +
                                   "border-radius: " + QString::number(radius) + "px;}";
    setStyleSheet(styleSheet);
}

bool ScrollView::append(QWidget* widget){
    if (widget != nullptr){
        widget->setFixedSize(size() - QSize(6, 6));
        if (widget->size() + QSize(6, 6) == size()){
            widget->setMinimumSize(QSize(0, 0));
            m_widgetList << widget;
            show();
            return true;
        }
    }
    return false;
}

bool ScrollView::append(QList<QWidget*>* widget_list){
    if (widget_list != nullptr){
        for (auto w : *widget_list){
            w->setFixedSize(size() - QSize(6, 6));
            if (w->size() + QSize(6, 6) != size())return false;
            w->setMinimumSize(QSize(0, 0));
        }
        m_widgetList << *widget_list;
        show();
        return true;
    }
    return false;
}

void ScrollView::wheelEvent(QWheelEvent *event)
{
    if (m_animationGroup->state() == QAnimationGroup::Running) {
        return;
    }

    if (event->delta() < 0) {
        scrollNext();
    } else {
        scrollPre();
    }
}

QPoint ScrollView::getPos(ScrollView::PosType type)
{
    switch (type) {
    case POS_PREV:
        return QPoint(3, 3-height());
    case POS_SHOW:
        return QPoint(3, 3);
    case POS_NEXT:
        return QPoint(3, 3+height());
    }
}

QWidget *ScrollView::getWidget(ScrollView::PosType type)
{
    QWidget *w;
    int index;

    switch (type) {
    case POS_PREV:
        index = (m_currentIndex - 1);
        index = index >= 0 ? index : m_widgetList.count() + index;
        w = m_widgetList.at(index);
        break;
    case POS_SHOW:
        w = m_widgetList.at(m_currentIndex);
        break;
    case POS_NEXT:
        index = (m_currentIndex + 1) % m_widgetList.count();
        w = m_widgetList.at(index);
        break;
    }

    return w;

}

void ScrollView::show()
{
    auto w = getWidget(static_cast<PosType>(POS_SHOW));
    w->move(getPos(static_cast<PosType>(POS_SHOW)));
    w->setVisible(true);
}

void ScrollView::initAnimation()
{
    for (int i = 0; i < 2; i++) {
        QSequentialAnimationGroup* resizeGroup = new QSequentialAnimationGroup(this);
        QPropertyAnimation* animation = new QPropertyAnimation(m_animationGroup);
        animation->setDuration(150);
        animation->setPropertyName("geometry");
        animation->setEasingCurve(QEasingCurve::InQuad);
        resizeGroup->addAnimation(animation);
        animation = new QPropertyAnimation(m_animationGroup);
        animation->setDuration(150);
        animation->setPropertyName("geometry");
        resizeGroup->addAnimation(animation);
        animation->setEasingCurve(QEasingCurve::OutQuad);
        m_animationGroup->addAnimation(resizeGroup);
    }
}

void ScrollView::scrollPre()
{
    for (int i = 0; i < 2; i++) {
        auto w = getWidget(static_cast<PosType>(POS_SHOW - i));
        w->setVisible(true);

        QPoint p_start = getPos(static_cast<PosType>(POS_SHOW - i));
        QPoint p_end = getPos(static_cast<PosType>(POS_NEXT - i));

        QSequentialAnimationGroup* resizeGroup = static_cast<QSequentialAnimationGroup *>(m_animationGroup->animationAt(i));
        QPropertyAnimation* a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(0));
        a->setStartValue(QRect(p_start, w->maximumSize()));
        a->setEndValue(QRect((p_start + p_end) / 2 + QPoint(w->maximumWidth() * (1 - m_zoomRate) / 2, 0), w->maximumSize() * m_zoomRate));
        a->setTargetObject(w);
        a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(1));
        a->setStartValue(QRect((p_start + p_end) / 2 + QPoint(w->maximumWidth() * (1 - m_zoomRate) / 2, 0), w->maximumSize() * m_zoomRate));
        a->setEndValue(QRect(p_end, w->maximumSize()));
        a->setTargetObject(w);
    }

    m_animationGroup->start();
    m_currentIndex = (m_currentIndex - 1);
    m_currentIndex = m_currentIndex >= 0 ? m_currentIndex : m_widgetList.count() + m_currentIndex;
}

void ScrollView::scrollNext()
{
    for (int i = 0; i < 2; i++) {
        auto w = getWidget(static_cast<PosType>(POS_SHOW + i));
        w->setVisible(true);

        QPoint p_start = getPos(static_cast<PosType>(POS_SHOW + i));
        QPoint p_end = getPos(static_cast<PosType>(POS_PREV + i));

        QSequentialAnimationGroup* resizeGroup = static_cast<QSequentialAnimationGroup *>(m_animationGroup->animationAt(i));
        QPropertyAnimation* a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(0));
        a->setStartValue(QRect(p_start, w->maximumSize()));
        a->setEndValue(QRect((p_start + p_end) / 2 + QPoint(w->maximumWidth() * (1 - m_zoomRate) / 2, 0), w->maximumSize() * m_zoomRate));
        a->setTargetObject(w);
        a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(1));
        a->setStartValue(QRect((p_start + p_end) / 2 + QPoint(w->maximumWidth() * (1 - m_zoomRate) / 2, 0), w->maximumSize() * m_zoomRate));
        a->setEndValue(QRect(p_end, w->maximumSize()));
        a->setTargetObject(w);
    }

    m_animationGroup->start();
    m_currentIndex = (m_currentIndex + 1) % m_widgetList.count();
}
