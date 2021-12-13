#include "scrollview.h"

ScrollView::ScrollView(QWidget *parent){
    m_animationGroup = new QParallelAnimationGroup(this);
    initAnimation();
    setFixedSize(size());
    setupAnimation();
    setupAppearance();
    setAttribute(Qt::WA_AcceptTouchEvents, true);
}

ScrollView::~ScrollView() {}

void ScrollView::setupAnimation(float zoomRate, int duration, ScrollType type){
    m_zoomRate = fmax(fmin(zoomRate, 1), 0);
    duration = fmax(duration, 0);
    m_scrollType = type;
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
    QString styleSheet = ".QWidget {border: 3px solid transparent; background-color: transparent; border-radius: " + QString::number(radius) + "px;}" +
                         ".QWidget:hover {border: " + QString::number(borderWidth) + "px solid " + borderColor + "; " +
                                   "background-color: " + backgroundColor + "; " +
                                   "border-radius: " + QString::number(radius) + "px;}";
    setStyleSheet(styleSheet);
}

bool ScrollView::get(int idx, QWidget* widget){
    if (0 <= idx && idx < m_widgetList.count()){
        widget = m_widgetList.at(idx);
        return true;
    }
    return false;
}

bool ScrollView::getCurrent(QWidget* widget){
    return get(m_currentIndex, widget);
}

bool ScrollView::getAll(QList<QWidget*>* widget_list){
    if (!m_widgetList.isEmpty()){
        widget_list = &m_widgetList;
    }
    return false;
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

bool ScrollView::remove(int idx){
    if (0 <= idx && idx < m_widgetList.count()){
        m_widgetList.removeAt(idx);
        m_currentIndex = fmin(m_currentIndex, m_widgetList.count() - 1);
        return true;
    }
    return false;
}

bool ScrollView::remove(QWidget* widget){
    if (widget != nullptr){
        bool success = m_widgetList.removeAll(widget);
        m_currentIndex = fmin(m_currentIndex, m_widgetList.count() - 1);
        return success;
    }
    return false;
}


bool ScrollView::removeCurrent(){
    return remove(m_currentIndex);
}

void ScrollView::clear(){
    m_widgetList.clear();
    m_currentIndex = 0;
}

void ScrollView::wheelEvent(QWheelEvent *event)
{
    if (m_animationGroup->state() == QAnimationGroup::Running) {
        return;
    }

    if (event->angleDelta().y() <= -100 || (m_scrollType ==SCROLL_HORIZONTAL && event->angleDelta().x() <= -100)) {
        scrollNext();
    } else if (event->angleDelta().y() >= 100 || (m_scrollType ==SCROLL_HORIZONTAL && event->angleDelta().x() >= 100)) {
        scrollPre();
    }
}

QPoint ScrollView::getPos(ScrollView::PosType type)
{
    if (m_scrollType == SCROLL_VERTICAL){
        return QPoint(3, 3 + height() * type);
    } else {
        return QPoint(3 + width() * type, 3);
    }
}

QWidget *ScrollView::getWidget(ScrollView::PosType type)
{
    int index = (m_currentIndex + m_widgetList.count() + type) % m_widgetList.count();
    return m_widgetList.at(index);
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
    if (m_widgetList.count() < 2)return;
    for (int i = 0; i < 2; i++) {
        auto w = getWidget(static_cast<PosType>(POS_SHOW - i));
        w->setVisible(true);

        QPoint p_start = getPos(static_cast<PosType>(POS_SHOW - i));
        QPoint p_end = getPos(static_cast<PosType>(POS_NEXT - i));

        QRect r_start = QRect(p_start, w->size());
        QRect r_inter = m_scrollType == SCROLL_VERTICAL ?
                    QRect((p_start + p_end) / 2 + QPoint(w->width() * (1 - m_zoomRate) / 2, 0), w->size() * m_zoomRate) :
                    QRect((p_start + p_end) / 2 + QPoint(0, w->height() * (1 - m_zoomRate) / 2), w->size() * m_zoomRate);
        QRect r_end = QRect(p_end, w->size());

        QSequentialAnimationGroup* resizeGroup = static_cast<QSequentialAnimationGroup *>(m_animationGroup->animationAt(i));
        QPropertyAnimation* a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(0));
        a->setStartValue(r_start);
        a->setEndValue(r_inter);
        a->setTargetObject(w);
        a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(1));
        a->setStartValue(r_inter);
        a->setEndValue(r_end);
        a->setTargetObject(w);
    }

    m_animationGroup->start();
    m_currentIndex = (m_currentIndex + m_widgetList.count() - 1) % m_widgetList.count();
}

void ScrollView::scrollNext()
{
    if (m_widgetList.count() < 2)return;
    for (int i = 0; i < 2; i++) {
        auto w = getWidget(static_cast<PosType>(POS_SHOW + i));
        w->setVisible(true);

        QPoint p_start = getPos(static_cast<PosType>(POS_SHOW + i));
        QPoint p_end = getPos(static_cast<PosType>(POS_PREV + i));

        QRect r_start = QRect(p_start, w->size());
        QRect r_inter = m_scrollType == SCROLL_VERTICAL ?
                    QRect((p_start + p_end) / 2 + QPoint(w->width() * (1 - m_zoomRate) / 2, 0), w->size() * m_zoomRate) :
                    QRect((p_start + p_end) / 2 + QPoint(0, w->height() * (1 - m_zoomRate) / 2), w->size() * m_zoomRate);
        QRect r_end = QRect(p_end, w->size());

        QSequentialAnimationGroup* resizeGroup = static_cast<QSequentialAnimationGroup *>(m_animationGroup->animationAt(i));
        QPropertyAnimation* a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(0));
        a->setStartValue(r_start);
        a->setEndValue(r_inter);
        a->setTargetObject(w);
        a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(1));
        a->setStartValue(r_inter);
        a->setEndValue(r_end);
        a->setTargetObject(w);
    }

    m_animationGroup->start();
    m_currentIndex = (m_currentIndex + 1) % m_widgetList.count();
}
