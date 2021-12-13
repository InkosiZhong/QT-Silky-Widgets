#ifndef CARDFRAME_H
#define CARDFRAME_H

#include <QWidget>
#include <QMap>
#include <QPair>
#include <QMouseEvent>
#include <QAnimationGroup>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include "utils.h"

class CardFrame : public QWidget
{
    Q_OBJECT
public:
    CardFrame(QWidget *parent = nullptr);

public:
    void setup(QString type, QString icon, QColor color, QMap<QString, QPair<QString, QString>>* attributes = nullptr, QSize minSize = QSize(0, 0));

protected:
    enum ExhibitState{EX_LARGE, EX_SMALL};
signals:
    void signalChangeExhibitState(const ExhibitState state, const QPoint& p_end);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void initAnimation();

private slots:
    void onChangeExhibitState(const ExhibitState state, const QPoint& p_ref);

private:
    QWidget* m_attrWidget;
    QPair<QString, QString> m_type; // name-icon pair
    QColor m_color;
    QMap<QString, QPair<QString, QString>> m_attributes; // key: value-icon
    QParallelAnimationGroup *m_shrinkAnimation, *m_enlargeAnimation;
    bool m_selected;
    QSize m_size;
    QSize m_minSize;
    ExhibitState m_state;
};

#endif // CARDFRAME_H
