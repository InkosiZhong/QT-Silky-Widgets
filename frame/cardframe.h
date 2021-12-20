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
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include "utils.h"

class CardFrame : public QFrame
{
    Q_OBJECT
public:
    CardFrame(QWidget *parent = nullptr);
    CardFrame(const CardFrame& src);

public:
    void init();
    void setup(const QString type, const QString icon, const QColor color, const QMap<QString, QPair<QString, QString>>* attributes = nullptr,
               const QSize size = QSize(300, 100), const QSize minSize = QSize(0, 0), const ExhibitState state = EX_LARGE);
    void setRestrict(QWidget *restrict = nullptr);

signals:
    void signalDrag(const QPoint& center_point, QWidget* frame);
    void signalDrop(const QPoint& center_point, QWidget* frame);

    void signalCopy(QWidget* src, QWidget* frame);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void initAnimation();

public slots:
    void switchExhibitState(const ExhibitState state, const QPoint& p_ctr_start, const QPoint& p_ctr_end);
    void copy(QWidget* widget);

private:
    QWidget* m_restrictWidget;
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
