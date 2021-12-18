#include "scrollview.h"

ScrollView::ScrollView(QWidget *parent){
    m_animationGroup = new QParallelAnimationGroup(this);
    setAttribute(Qt::WA_StyledBackground, true);
    setAcceptDrops(true);
    initAnimation();
    setFixedSize(size());
    setupAnimation();
    setupAppearance();
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

void ScrollView::setupAppearance(int borderWidth, int radius, QColor borderColor, QColor backgroundColor){
    m_borderWidth = fmax(fmin(borderWidth, 3), 0);
    radius = fmax(radius, 0);
    QString background, border;
    QString styleSheet = ".ScrollView {border: " + QString::number(m_borderWidth) + "px solid transparent; background-color: " + utils::color2QString(backgroundColor, background) + "; border-radius: " + QString::number(radius) + "px;}" +
                         ".ScrollView:hover {border: " + QString::number(m_borderWidth) + "px solid " + utils::color2QString(borderColor, border) + ";}";
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
    widget->setParent(this);
    widget->setVisible(false);
    if (widget != nullptr){
        widget->setFixedSize(size() - QSize(m_borderWidth * 2, m_borderWidth * 2));
        if (widget->size() + QSize(m_borderWidth * 2, m_borderWidth * 2) == size()){
            widget->setMinimumSize(QSize(0, 0));
            m_widgetList << widget;
            if (m_widgetList.count() == 1)exhibit();
            return true;
        }
    }
    return false;
}

bool ScrollView::append(QList<QWidget*>* widget_list){
    if (widget_list != nullptr){
        for (auto w : *widget_list){
            append(w);
        }
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

void ScrollView::bind(QWidget* widget){
    connect(widget, SIGNAL(signalDrop(const QPoint&, QWidget*)), this, SLOT(onCaptureWidget(const QPoint&, QWidget*)));
    connect(widget, SIGNAL(signalDrag(const QPoint&)), this, SLOT(onDragWidget(const QPoint&)));
    connect(this, SIGNAL(signalSwitchExhibitState(const ExhibitState, const QPoint&, const QPoint&)),
            widget, SLOT(switchExhibitState(const ExhibitState, const QPoint&, const QPoint&)));
}

void ScrollView::bind(QList<QWidget*>* widget_list){
    if (widget_list != nullptr){
        for (auto w : *widget_list){
            bind(w);
        }
    }
}

void ScrollView::wheelEvent(QWheelEvent *event){
    if (m_animationGroup->state() == QAnimationGroup::Running) {
        return;
    }

    if (event->angleDelta().y() <= -100 || (m_scrollType ==SCROLL_HORIZONTAL && event->angleDelta().x() <= -100)) {
        scrollNext();
    } else if (event->angleDelta().y() >= 100 || (m_scrollType ==SCROLL_HORIZONTAL && event->angleDelta().x() >= 100)) {
        scrollPrev();
    }
}

QPoint ScrollView::getPos(ScrollView::PosType type)
{
    if (m_scrollType == SCROLL_VERTICAL){
        return QPoint(m_borderWidth, m_borderWidth + height() * type);
    } else {
        return QPoint(m_borderWidth + width() * type, m_borderWidth);
    }
}

QWidget *ScrollView::getWidget(ScrollView::PosType type)
{
    int index = (m_currentIndex + m_widgetList.count() + type) % m_widgetList.count();
    return m_widgetList.at(index);
}

void ScrollView::exhibit()
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

void ScrollView::scrollPrev()
{
    if (m_widgetList.count() < 2)return;
    for (int i = 0; i < 2; i++) {
        auto w = getWidget(static_cast<PosType>(POS_SHOW - i));
        w->setVisible(true);

        QPoint p_start = getPos(static_cast<PosType>(POS_SHOW - i));
        QPoint p_end = getPos(static_cast<PosType>(POS_NEXT - i));

        QRect r_start(p_start, w->maximumSize());
        QRect r_inter = m_scrollType == SCROLL_VERTICAL ?
                    QRect((p_start + p_end) / 2 + QPoint(w->width() * (1 - m_zoomRate) / 2, 0), w->maximumSize() * m_zoomRate) :
                    QRect((p_start + p_end) / 2 + QPoint(0, w->height() * (1 - m_zoomRate) / 2), w->maximumSize() * m_zoomRate);
        QRect r_end(p_end, w->maximumSize());

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

        QRect r_start(p_start, w->maximumSize());
        QRect r_inter = m_scrollType == SCROLL_VERTICAL ?
                    QRect((p_start + p_end) / 2 + QPoint(w->width() * (1 - m_zoomRate) / 2, 0), w->maximumSize() * m_zoomRate) :
                    QRect((p_start + p_end) / 2 + QPoint(0, w->height() * (1 - m_zoomRate) / 2), w->maximumSize() * m_zoomRate);
        QRect r_end(p_end, w->maximumSize());

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

void ScrollView::scrollIn(){
    if (m_widgetList.count() < 1)return;
    auto w = getWidget(static_cast<PosType>(POS_SHOW));
    w->setVisible(true);

    QPoint p_start = getPos(static_cast<PosType>(POS_NEXT));
    QPoint p_end = getPos(static_cast<PosType>(POS_SHOW));

    QRect r_start(p_start, w->maximumSize());
    QRect r_inter = m_scrollType == SCROLL_VERTICAL ?
                QRect((p_start + p_end) / 2 + QPoint(w->width() * (1 - m_zoomRate) / 2, 0), w->maximumSize() * m_zoomRate) :
                QRect((p_start + p_end) / 2 + QPoint(0, w->height() * (1 - m_zoomRate) / 2), w->maximumSize() * m_zoomRate);
    QRect r_end(p_end, w->maximumSize());

    QSequentialAnimationGroup* resizeGroup = static_cast<QSequentialAnimationGroup *>(m_animationGroup->animationAt(1));
    QPropertyAnimation* a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(0));
    a->setStartValue(r_start);
    a->setEndValue(r_inter);
    a->setTargetObject(w);
    a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(1));
    a->setStartValue(r_inter);
    a->setEndValue(r_end);
    a->setTargetObject(w);

    resizeGroup->start();
}

void ScrollView::scrollOut(){
    if (m_widgetList.count() < 1)return;
    auto w = getWidget(static_cast<PosType>(POS_SHOW));
    w->setVisible(true);

    QPoint p_start = getPos(static_cast<PosType>(POS_SHOW));
    QPoint p_end = getPos(static_cast<PosType>(POS_NEXT));

    QRect r_start(p_start, w->maximumSize());
    QRect r_inter = m_scrollType == SCROLL_VERTICAL ?
                QRect((p_start + p_end) / 2 + QPoint(w->width() * (1 - m_zoomRate) / 2, 0), w->maximumSize() * m_zoomRate) :
                QRect((p_start + p_end) / 2 + QPoint(0, w->height() * (1 - m_zoomRate) / 2), w->maximumSize() * m_zoomRate);
    QRect r_end(p_end, w->maximumSize());

    QSequentialAnimationGroup* resizeGroup = static_cast<QSequentialAnimationGroup *>(m_animationGroup->animationAt(0));
    QPropertyAnimation* a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(0));
    a->setStartValue(r_start);
    a->setEndValue(r_inter);
    a->setTargetObject(w);
    a = static_cast<QPropertyAnimation *>(resizeGroup->animationAt(1));
    a->setStartValue(r_inter);
    a->setEndValue(r_end);
    a->setTargetObject(w);
    resizeGroup->start();
}

void ScrollView::onDragWidget(const QPoint& center_point){
    QRect rect(pos(), size());
    if (!m_add_mode && rect.contains(center_point)){
        this->scrollOut();
        m_add_mode = true;
    } else if (m_add_mode && !rect.contains(center_point)){
        this->scrollIn();
        m_add_mode = false;
    }
}

void ScrollView::onCaptureWidget(const QPoint& center_point, QWidget* widget){
    QRect rect(pos(), size());
    if (rect.contains(center_point)){
        append(widget);
        m_add_mode = false;
        m_currentIndex = m_widgetList.count() - 1;
        exhibit();
        emit signalSwitchExhibitState(EX_LARGE, center_point - pos(), QPoint(size().width() / 2 + m_borderWidth, size().height() / 2 + m_borderWidth));
    }
}
