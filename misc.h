#ifndef MISC_H
#define MISC_H
#include <QString>

struct Rect {
    int x;
    int y;
    int width;
    int height;

    QString toString() const {
        return QString("pos(%1,%2) size(%3,%4)").arg(x).arg(y).arg(width).arg(height);
    }
    bool operator==(const Rect& rhs) {
        return this->x == rhs.x && this->y == rhs.y &&
                this->width == rhs.width && this->height == rhs.height;
    }
    bool operator!=(const Rect& rhs) {
        return !(*this==rhs);
    }

    Rect& operator=(const Rect& rhs) {
        this->x = rhs.x;
        this->y = rhs.y;
        this->width = rhs.width;
        this->height = rhs.height;
        return *this;
    }
};

#endif // MISC_H
