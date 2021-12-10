#include "bidirectscrollview.h"

BidirectScrollView::BidirectScrollView(QWidget *parent){
    m_animationGroup = new QParallelAnimationGroup(this);
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

void BidirectScrollView::setupAppearance(int borderWidth, int radius, QString borderColor, QString backgroundColor){
    borderWidth = fmax(fmin(borderWidth, 3), 0);
    radius = fmax(radius, 0);
    QString styleSheet = ".QWidget {border: 3px solid transparent; background-color: transparent; border-radius: " + QString::number(radius) + "px;}" +
                         ".QWidget:hover {border: " + QString::number(borderWidth) + "px solid " + borderColor + "; " +
                                   "background-color: " + backgroundColor + "; " +
                                   "border-radius: " + QString::number(radius) + "px;}";
    setStyleSheet(styleSheet);
}

bool BidirectScrollView::append(QList<QList<QWidget *>>* widget_list){
    if (widget_list != nullptr){
        for (auto wl : *widget_list){
            for (auto w : wl){
                w->setFixedSize(size() - QSize(6, 6));
                if (w->size() + QSize(6, 6) != size())return false;
                w->setMinimumSize(QSize(0, 0));
            }
        }
        m_widgetList << *widget_list;
        show();
        return true;
    }
    return false;
}

void BidirectScrollView::wheelEvent(QWheelEvent *event)
{
    if (m_animationGroup->state() == QAnimationGroup::Running) {
        return;
    }

    if (event->angleDelta().y() < 0) {
        scrollNext();
    } else {
        scrollPre();
    }
}

void BidirectScrollView::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Shift){
        m_scrollType = SCROLL_HORIZONTAL;
    }
}
void BidirectScrollView::keyReleaseEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Shift){
        m_scrollType = SCROLL_VERTICAL;
    }
}

QPoint BidirectScrollView::getPos(BidirectScrollView::PosType type)
{
    if (m_scrollType == SCROLL_VERTICAL){
        return QPoint(3, 3 + height() * type);
    } else {
        return QPoint(3 + width() * type, 3);
    }
}

#include <QDebug>
QWidget *BidirectScrollView::getWidget(BidirectScrollView::PosType type)
{
    if (m_scrollType == SCROLL_VERTICAL){
        int verticalIndex = (m_verticalIndex + m_widgetList.count() + type) % m_widgetList.count();
        int horizentalIndex = fmin(m_horizontalIndex, m_widgetList.at(verticalIndex).count() - 1);
        return m_widgetList.at(verticalIndex).at(horizentalIndex);
    } else {
        int horizentalIndex = (m_horizontalIndex + m_widgetList.at(m_verticalIndex).count() + type) % m_widgetList.at(m_verticalIndex).count();
        return m_widgetList.at(m_verticalIndex).at(horizentalIndex);
    }
}

void BidirectScrollView::show()
{
    auto w = getWidget(static_cast<PosType>(POS_SHOW));
    w->move(getPos(static_cast<PosType>(POS_SHOW)));
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

void BidirectScrollView::scrollPre()
{
    if ((m_scrollType == SCROLL_VERTICAL && m_widgetList.count() < 2) ||
        (m_scrollType == SCROLL_HORIZONTAL && m_widgetList.at(m_verticalIndex).count() < 2)){
        return;
    }
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
    if (m_scrollType == SCROLL_VERTICAL){
        m_verticalIndex = (m_verticalIndex + m_widgetList.count() - 1) % m_widgetList.count();
        m_horizontalIndex = fmin(m_horizontalIndex, m_widgetList.at(m_verticalIndex).count() - 1);
    } else {
        m_horizontalIndex = (m_horizontalIndex + m_widgetList.at(m_verticalIndex).count() - 1) % m_widgetList.at(m_verticalIndex).count();
    }
}

void BidirectScrollView::scrollNext()
{
    if ((m_scrollType == SCROLL_VERTICAL && m_widgetList.count() < 2) ||
        (m_scrollType == SCROLL_HORIZONTAL && m_widgetList.at(m_verticalIndex).count() < 2)){
        return;
    }
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
    if (m_scrollType == SCROLL_VERTICAL){
        m_verticalIndex = (m_verticalIndex + 1) % m_widgetList.count();
        m_horizontalIndex = fmin(m_horizontalIndex, m_widgetList.at(m_verticalIndex).count() - 1);
    } else {
        m_horizontalIndex = (m_horizontalIndex + 1) % m_widgetList.at(m_verticalIndex).count();
    }
}
