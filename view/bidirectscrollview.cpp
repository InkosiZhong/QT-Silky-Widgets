#include "bidirectscrollview.h"

BidirectScrollView::BidirectScrollView(QWidget *parent){
    m_animationGroup = new QParallelAnimationGroup(this);
    setAttribute(Qt::WA_StyledBackground, true);
    initAnimation();
    setFixedSize(size());
    setupAnimation();
    setupAppearance();
    grabKeyboard();
}

BidirectScrollView::~BidirectScrollView() {}

void BidirectScrollView::setupAnimation(float zoomRate, int duration){
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

void BidirectScrollView::setupAppearance(int borderWidth, int radius, QColor borderColor, QColor backgroundColor){
    borderWidth = fmax(fmin(borderWidth, 3), 0);
    radius = fmax(radius, 0);
    QString tmp;
    QString styleSheet = ".BidirectScrollView {border: 3px solid transparent; background-color: transparent; border-radius: " + QString::number(radius) + "px;}" +
                         ".BidirectScrollView:hover {border: " + QString::number(borderWidth) + "px solid " + utils::color2QString(borderColor, tmp) + "; " +
                                   "background-color: " + utils::color2QString(backgroundColor, tmp) + ";}";
    setStyleSheet(styleSheet);
}

bool BidirectScrollView::get(int v_idx, int h_idx, QWidget *widget){
    if (0 <= v_idx && v_idx < m_widgetList.count() && 0 <= h_idx && h_idx < m_widgetList.at(v_idx).count()){
        widget = m_widgetList.at(v_idx).at(h_idx);
        return true;
    }
    return false;
}

bool BidirectScrollView::append(int idx, QWidget *widget){
    if (0 <= idx && widget != nullptr){
        if (idx > m_widgetList.count())return false;
        widget->setVisible(false);
        widget->setParent(this);
        widget->setFixedSize(size() - QSize(6, 6));
        if (widget->size() + QSize(6, 6) != size())return false;
        widget->setMinimumSize(QSize(0, 0));
        if (idx == m_widgetList.count()){
            QList<QWidget *>widget_list;
            widget_list << widget;
            m_widgetList << widget_list;
        } else {
            m_widgetList[idx] << widget;
        }
        exhibit();
        return true;
    }
    return false;
}

bool BidirectScrollView::appendHere(QWidget *widget){
    return append(m_verticalIndex, widget);
}

bool BidirectScrollView::append(int idx, QList<QWidget *>* widget_list){
    if (0 <= idx && widget_list != nullptr){
        if (idx > m_widgetList.count())return false;
        if (idx == m_widgetList.count()){
            return append(widget_list);
        } else {
            for (auto w : *widget_list) {
                w->setVisible(false);
                w->setParent(this);
                w->setFixedSize(size() - QSize(6, 6));
                if (w->size() + QSize(6, 6) != size())return false;
                w->setMinimumSize(QSize(0, 0));
            }
            m_widgetList[idx] << *widget_list;
        }
        exhibit();
        return true;
    }
    return false;
}

bool BidirectScrollView::appendHere(QList<QWidget *>* widget_list){
return append(m_verticalIndex, widget_list);
}

bool BidirectScrollView::append(QList<QWidget *>* widget_list){
    if (widget_list != nullptr){
        for (auto w : *widget_list){
            w->setVisible(false);
            w->setParent(this);
            w->setFixedSize(size() - QSize(6, 6));
            if (w->size() + QSize(6, 6) != size())return false;
            w->setMinimumSize(QSize(0, 0));
        }
        m_widgetList << *widget_list;
        exhibit();
        return true;
    }
    return false;
}

bool BidirectScrollView::append(QList<QList<QWidget *>>* widget_list){
    if (widget_list != nullptr){
        for (auto wl : *widget_list){
            for (auto w : wl){
                w->setVisible(false);
                w->setParent(this);
                w->setFixedSize(size() - QSize(6, 6));
                if (w->size() + QSize(6, 6) != size())return false;
                w->setMinimumSize(QSize(0, 0));
            }
        }
        m_widgetList << *widget_list;
        exhibit();
        return true;
    }
    return false;
}

bool BidirectScrollView::remove(int v_idx, int h_idx){
    if (0 <= v_idx && v_idx < m_widgetList.count() && 0 <= h_idx && h_idx < m_widgetList.at(v_idx).count()){
        m_widgetList[v_idx].removeAt(h_idx);
        if (m_verticalIndex == v_idx){
            m_horizontalIndex = fmin(m_horizontalIndex, m_widgetList.at(v_idx).count() - 1);
        }
        return true;
    }
    return false;
}

bool BidirectScrollView::removeCurrent(){
    return remove(m_verticalIndex, m_horizontalIndex);
}

void BidirectScrollView::clear(){
    m_widgetList.clear();
}

void BidirectScrollView::wheelEvent(QWheelEvent *event)
{
    if (m_animationGroup->state() == QAnimationGroup::Running) {
        return;
    }

    if (event->angleDelta().y() <= -100) {
        scrollNext(m_shiftDown ? SCROLL_HORIZONTAL : SCROLL_VERTICAL);
    } else if (event->angleDelta().y() >= 100) {
        scrollPre(m_shiftDown ? SCROLL_HORIZONTAL : SCROLL_VERTICAL);
    } else if (event->angleDelta().x() <= -100) {
        scrollNext(SCROLL_HORIZONTAL);
    } else if (event->angleDelta().x() >= 100) {
        scrollPre(SCROLL_HORIZONTAL);
    }
}

void BidirectScrollView::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Shift){
        //type = SCROLL_HORIZONTAL;
        m_shiftDown = true;
    }
}
void BidirectScrollView::keyReleaseEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Shift){
        //type = SCROLL_VERTICAL;
        m_shiftDown = false;
    }
}

QPoint BidirectScrollView::getPos(BidirectScrollView::PosType pos_type, ScrollType scroll_type)
{
    if (scroll_type == SCROLL_VERTICAL){
        return QPoint(3, 3 + height() * pos_type);
    } else {
        return QPoint(3 + width() * pos_type, 3);
    }
}

#include <QDebug>
QWidget *BidirectScrollView::getWidget(BidirectScrollView::PosType pos_type, ScrollType scroll_type)
{
    if (scroll_type == SCROLL_VERTICAL){
        int verticalIndex = (m_verticalIndex + m_widgetList.count() + pos_type) % m_widgetList.count();
        int horizentalIndex = fmin(m_horizontalIndex, m_widgetList.at(verticalIndex).count() - 1);
        return m_widgetList.at(verticalIndex).at(horizentalIndex);
    } else {
        int horizentalIndex = (m_horizontalIndex + m_widgetList.at(m_verticalIndex).count() + pos_type) % m_widgetList.at(m_verticalIndex).count();
        return m_widgetList.at(m_verticalIndex).at(horizentalIndex);
    }
}

void BidirectScrollView::exhibit()
{
    auto w = getWidget(static_cast<PosType>(POS_SHOW), SCROLL_VERTICAL);
    w->move(getPos(static_cast<PosType>(POS_SHOW), SCROLL_VERTICAL));
    w->setVisible(true);
}

void BidirectScrollView::initAnimation()
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

void BidirectScrollView::scrollPre(ScrollType type)
{
    if ((type == SCROLL_VERTICAL && m_widgetList.count() < 2) ||
        (type == SCROLL_HORIZONTAL && m_widgetList.at(m_verticalIndex).count() < 2)){
        return;
    }
    for (int i = 0; i < 2; i++) {
        auto w = getWidget(static_cast<PosType>(POS_SHOW - i), type);
        w->setVisible(true);

        QPoint p_start = getPos(static_cast<PosType>(POS_SHOW - i), type);
        QPoint p_end = getPos(static_cast<PosType>(POS_NEXT - i), type);

        QRect r_start = QRect(p_start, w->size());
        QRect r_inter = type == SCROLL_VERTICAL ?
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
    if (type == SCROLL_VERTICAL){
        m_verticalIndex = (m_verticalIndex + m_widgetList.count() - 1) % m_widgetList.count();
        m_horizontalIndex = fmin(m_horizontalIndex, m_widgetList.at(m_verticalIndex).count() - 1);
    } else {
        m_horizontalIndex = (m_horizontalIndex + m_widgetList.at(m_verticalIndex).count() - 1) % m_widgetList.at(m_verticalIndex).count();
    }
}

void BidirectScrollView::scrollNext(ScrollType type)
{
    if ((type == SCROLL_VERTICAL && m_widgetList.count() < 2) ||
        (type == SCROLL_HORIZONTAL && m_widgetList.at(m_verticalIndex).count() < 2)){
        return;
    }
    for (int i = 0; i < 2; i++) {
        auto w = getWidget(static_cast<PosType>(POS_SHOW + i), type);
        w->setVisible(true);

        QPoint p_start = getPos(static_cast<PosType>(POS_SHOW + i), type);
        QPoint p_end = getPos(static_cast<PosType>(POS_PREV + i), type);

        QRect r_start = QRect(p_start, w->size());
        QRect r_inter = type == SCROLL_VERTICAL ?
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
    if (type == SCROLL_VERTICAL){
        m_verticalIndex = (m_verticalIndex + 1) % m_widgetList.count();
        m_horizontalIndex = fmin(m_horizontalIndex, m_widgetList.at(m_verticalIndex).count() - 1);
    } else {
        m_horizontalIndex = (m_horizontalIndex + 1) % m_widgetList.at(m_verticalIndex).count();
    }
}
