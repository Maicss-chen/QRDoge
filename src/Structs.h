//
// Created by maicss on 23-10-1.
//

#ifndef QT_DOG_STRUCTS_H
#define QT_DOG_STRUCTS_H
#include "QString"
#include "QPoint"
#include "QScreen"

enum class CodeType { Text, Binary, Mixed, GS1, ISO15434, UnknownECI };

struct CodePosition {
    QPoint topLeft;
    QPoint topRight;
    QPoint bottomLeft;
    QPoint bottomRight;

    bool operator==(const CodePosition& other) const{
        return topLeft == other.topLeft
               && topRight == other.topRight
               && bottomLeft == other.bottomLeft
               && bottomRight == other.bottomRight;
    }
};

struct CodeItem {
    QString text;
    CodeType type;
    CodePosition position;
    QScreen *screen;
    bool operator==(const CodeItem& other) const{
        return text == other.text
               && type == other.type;
//            && position == other.position;
    }

};

#endif //QT_DOG_STRUCTS_H
