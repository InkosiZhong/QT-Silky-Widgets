 #include "utils.h"

QString& utils::color2QString(const QColor& color, QString& str){
    str = QString().asprintf("#%02X%02X%02X", color.red(), color.green(), color.blue());
    return str;
}
