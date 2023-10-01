//
// Created by maicss on 23-9-30.
//

#ifndef QT_DOG_HOVERBUTTON_H
#define QT_DOG_HOVERBUTTON_H

#include <QPushButton>
#include <QMenu>
#include "Structs.h"
#include "QPropertyAnimation"
#define SHADOW_SIZE 10

enum TextType {
    URL,
    TEXT,
    WIFI
};

class HoverButton : public QPushButton{
    Q_OBJECT
public:
    HoverButton(QWidget *parent=nullptr);
    ~HoverButton();
    static HoverButton* instance();
    static void pop(QPoint position, const CodeItem& code);
    static void hide();
protected:
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
//    void mouse
protected slots:
    void buttonClicked() const;
private:
    static HoverButton* _instance;
    CodeItem m_code;
    TextType m_type;
    QMenu *m_menu;
    QPropertyAnimation *m_propertyAnimation;
};


#endif //QT_DOG_HOVERBUTTON_H
