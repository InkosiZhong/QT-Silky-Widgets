#include "cardframe.h"

CardFrame::CardFrame(QWidget *parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    QString styleSheet = ".CardFrame {background-color: #ffffff; border-radius: 15px;} .QFrame:hover {background-color: #eeeeee;}";
    setStyleSheet(styleSheet);
    QHBoxLayout* mainLayout = new QHBoxLayout();
    QVBoxLayout* vctLayout = new QVBoxLayout();
    m_attrWidget = new QWidget();
    QHBoxLayout* attrLayout = new QHBoxLayout();
    attrLayout->setMargin(0);
    QLabel* label = new QLabel();
    QSpacerItem* spacer = new QSpacerItem(25, 0, QSizePolicy::Maximum);
    mainLayout->addSpacerItem(spacer);
    label->setStyleSheet("image:url(:/default)");
    label->setMaximumSize(50, 50);
    label->setMinimumSize(20, 20);
    mainLayout->addWidget(label);
    spacer = new QSpacerItem(25, 0, QSizePolicy::Maximum);
    mainLayout->addSpacerItem(spacer);
    vctLayout->addStretch(1);
    label = new QLabel("Card");
    label->setFont(QFont("Roboto", 14, 75));
    label->setStyleSheet("color: black");
    vctLayout->addWidget(label);
    m_attrWidget->setLayout(attrLayout);
    vctLayout->addWidget(m_attrWidget);
    vctLayout->addStretch(1);
    mainLayout->addLayout(vctLayout);
    setLayout(mainLayout);
    initAnimation();
}

void CardFrame::setup(QString type, QString icon, QColor color, QMap<QString, QPair<QString, QString>>* attributes, QSize size, QSize minSize){
    m_type = QPair<QString, QString>(type, icon);
    m_color = color;
    m_attributes.clear();
    m_size = size;
    m_minSize = minSize == QSize(0, 0) ? this->size() : minSize;
    if (attributes != nullptr){
        m_attributes = *attributes;
    }
    QString hex_color, hover_color;
    QString styleSheet = ".CardFrame {background-color: " + utils::color2QString(m_color, hex_color) + "; border-radius: 15px;}"; // .CardFrame:hover {background-color: " + utils::color2QString(m_color.darker(120), hover_color) + ";}";
    setStyleSheet(styleSheet);
    QLabel* label = qobject_cast<QLabel *>(layout()->itemAt(1)->widget());
    label->setStyleSheet("image:url(" + icon + ")");
    label = qobject_cast<QLabel *>(layout()->itemAt(3)->layout()->itemAt(1)->widget());
    label->setText(type);
    label->setStyleSheet("color: #F9F9F9;");
    if (!m_attributes.isEmpty()){
        QString styleSheet = ".QWidget {background-color: transparent; border: 0px}";
        m_attrWidget->setStyleSheet(styleSheet);
        QHBoxLayout* attrLayout = qobject_cast<QHBoxLayout *>(m_attrWidget->layout());
        QLayoutItem *child;
        while ((child = attrLayout->takeAt(0))) {
            if(child->widget())child->widget()->setParent(NULL);
            delete child;
        }
        for (auto attr : m_attributes){
            label = new QLabel();
            label->setStyleSheet("image:url(" + attr.second + ")");
            label->setFixedSize(15, 15);
            attrLayout->addWidget(label);
            label = new QLabel(attr.first);
            label->setFont(QFont("Roboto", 10, 75));
            label->setStyleSheet("color: #F9F9F9");
            attrLayout->addWidget(label);
        }
    }
}


void CardFrame::switchExhibitState(const ExhibitState state, const QPoint& p_ref){
    if (state != m_state){
        if (state == EX_SMALL){
            QPropertyAnimation* a = static_cast<QPropertyAnimation *>(m_shrinkAnimation->animationAt(0));
            a->setStartValue(QRect(this->pos(), this->size()));
            a->setEndValue(QRect(p_ref, m_minSize));
            a->setTargetObject(this);
            if (!m_attributes.isEmpty()){
                m_attrWidget->hide();
            }
            m_shrinkAnimation->start();
        } else {
            QPropertyAnimation* a = static_cast<QPropertyAnimation *>(m_enlargeAnimation->animationAt(0));
            a->setStartValue(QRect(this->pos(), m_minSize));
            a->setEndValue(QRect(p_ref, m_size));
            a->setTargetObject(this);
            if (!m_attributes.isEmpty()){
                m_attrWidget->show();
            }
            m_enlargeAnimation->start();
        }
    }
    m_state = state;
}

void CardFrame::mousePressEvent(QMouseEvent* event){
    //this->setParent(parentWidget()->parentWidget());
    if (event->buttons() & Qt::LeftButton){
        m_selected = true;
        QPoint p_ref = QPoint(event->pos().x() + pos().x() - m_minSize.width() / 2, event->pos().y() + pos().y() - m_minSize.height() / 2);
        switchExhibitState(EX_SMALL, p_ref);
    }
}

void CardFrame::mouseMoveEvent(QMouseEvent* event){
    if (m_selected){
        QPoint p_ref = QPoint(event->pos().x() + pos().x() - size().width() / 2, event->pos().y() + pos().y() - size().height() / 2);
        move(p_ref);
    }
}

void CardFrame::mouseReleaseEvent(QMouseEvent* event){
    if (m_selected){
        QPoint p_ref = QPoint(event->pos().x() + pos().x() - m_size.width() / 2, event->pos().y() + pos().y() - m_size.height() / 2);
        //QPoint p_ref = QPoint(3, 3);
        switchExhibitState(EX_LARGE, p_ref);
        emit signalDrop(event->pos() + pos(), this);
        m_selected = false;
    }
}

void CardFrame::initAnimation(){
    m_shrinkAnimation = new QParallelAnimationGroup(this);
    QPropertyAnimation* animation = new QPropertyAnimation(m_shrinkAnimation);
    animation->setDuration(150);
    animation->setPropertyName("geometry");
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    m_shrinkAnimation->addAnimation(animation);

    m_enlargeAnimation = new QParallelAnimationGroup(this);
    animation = new QPropertyAnimation(m_shrinkAnimation);
    animation->setDuration(150);
    animation->setPropertyName("geometry");
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    m_enlargeAnimation->addAnimation(animation);
}
