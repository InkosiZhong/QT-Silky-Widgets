#ifndef UTILS_H
#define UTILS_H

#include <QColor>

namespace utils {
QString& color2QString(const QColor& color, QString& str);
}

enum ExhibitState{EX_LARGE, EX_SMALL};

#endif // UTILS_H
