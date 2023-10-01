#include "HoverButton.h"
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QUrl>
#include <QToolTip>
#include <QClipboard>
#include <QSystemTrayIcon>

HoverButton* HoverButton::_instance = nullptr;

HoverButton::HoverButton(QWidget *parent)
    : QPushButton(parent)
    , m_menu(nullptr) {
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(32 + SHADOW_SIZE, 32 + SHADOW_SIZE);
    update();
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#444444"));
    shadow->setBlurRadius(10);
    this->setGraphicsEffect(shadow);
    this->setContextMenuPolicy(Qt::DefaultContextMenu);
    connect(this, &QPushButton::clicked, this, &HoverButton::buttonClicked);
    setWindowOpacity(0);
    m_propertyAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_propertyAnimation->setDuration(200);
    connect(m_propertyAnimation, &QPropertyAnimation::valueChanged, [=](){
        if (windowOpacity()==0){
            static_cast<QPushButton*>(instance())->hide();
        }else {
            static_cast<QPushButton*>(instance())->show();
        }
    });
}

HoverButton *HoverButton::instance() {
    if (!_instance){
        _instance = new HoverButton(nullptr);
        _instance->show();
    }
    return _instance;
}

void HoverButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath painterPath;
    painterPath.addRoundedRect(SHADOW_SIZE, SHADOW_SIZE, width() - 2 * SHADOW_SIZE, height() - 2 * SHADOW_SIZE, width()/2, height()/2);
    switch (m_type) {
        case URL:
            painter.fillPath(painterPath, QColor("#686de0"));
            break;
        case TEXT:
            painter.fillPath(painterPath, QColor("#dff9fb"));
            break;
        case WIFI:
            painter.fillPath(painterPath, QColor("#6ab04c"));
            break;
    }
}

void HoverButton::hide() {
    if (!instance()->m_menu) {
        goto Hide;
    } else if ( instance()->m_menu->isHidden()) {
        goto Hide;
    } else {
        return;
    }
    Hide:
    instance()-> m_propertyAnimation->stop();
    instance()-> m_propertyAnimation->setEndValue(0);
    instance()-> m_propertyAnimation->start();
}

HoverButton::~HoverButton() {
}

bool isBeginAs(const QString& string1, const QString& string2) {
    return string1.left(string2.size()) == string2;
}

void HoverButton::pop(QPoint position, const CodeItem& code) {

    if (instance()->m_menu) {
        return;
    }
    instance()->m_code = code;
    instance()->move(position.x() - instance()->width()/2, position.y() - instance()->height()/2);
    if (isBeginAs(code.text, "http://")
        || isBeginAs(code.text, "https://")){
        instance()->m_type = TextType::URL;
    }else if (isBeginAs(code.text, "WIFI:")){
        instance()->m_type = TextType::WIFI;
    }else {
        instance()->m_type = TextType::TEXT;
    }
    instance()-> m_propertyAnimation->stop();
    instance()-> m_propertyAnimation->setEndValue(1);
    instance()-> m_propertyAnimation->start();
}


void HoverButton::buttonClicked() const {
    switch (m_type) {
        case URL:
#ifdef Q_OS_LINUX
            system(("xdg-open "+m_code.text).toStdString().c_str());
#endif
            break;
        case TEXT:{
#ifdef Q_OS_LINUX
            system(("notify-send 文本已被复制到剪贴板 \"文本内容: " + this->m_code.text+"\"").toStdString().c_str());
#elif
            QSystemTrayIcon systemTrayIcon;
            systemTrayIcon.showMessage("文本已被复制到剪贴板", "文本内容: "+this->m_code.text);
#endif
            QApplication::clipboard() -> setText(this->m_code.text);
            break;
        }
        case WIFI:{
            QString wifi = m_code.text.right(m_code.text.size() - 5);
            QStringList ls = wifi.split(';');
            QMap<QString, QString> args;
            for (const auto& item : ls) {
                QStringList il = item.split(':');
                if (il.size() >= 2)
                    args.insert(il.at(0), il.at(1));
            }
#ifdef Q_OS_LINUX
            system(("nmcli device wifi connect "+args.value("S")+" password "+args.value("P")).toStdString().c_str());
#endif
            break;
        }
    }
    hide();
}

void HoverButton::contextMenuEvent(QContextMenuEvent *event) {

    m_menu = new QMenu(nullptr);
    m_menu->setToolTipsVisible(true);
    m_menu->setToolTipDuration(100);

    auto* action_copy = new QAction("复制文本");
    action_copy->setToolTip(this->m_code.text);
    connect(action_copy, &QAction::triggered, [=](){
#ifdef Q_OS_LINUX
            system(("notify-send 文本已被复制到剪贴板 \"文本内容: " + this->m_code.text+"\"").toStdString().c_str());
#elif
        QSystemTrayIcon systemTrayIcon;
        systemTrayIcon.showMessage("文本已被复制到剪贴板", "文本内容: "+this->m_code.text);
#endif
        QApplication::clipboard() -> setText(this->m_code.text);
    });
    m_menu->addAction(action_copy);

    if (m_type==WIFI){
        QString wifi = m_code.text.right(m_code.text.size() - 5);
        QStringList ls = wifi.split(';');
        QMap<QString, QString> args;
        for (const auto& item : ls) {
            QStringList il = item.split(':');
            if (il.size() >= 2)
                args.insert(il.at(0), il.at(1));
        }
        auto* action_wifi = new QAction("连接到 "+args.value("S"));
        connect(action_wifi, &QAction::triggered, [=](){
            system(("nmcli device wifi connect "+args.value("S")+" password "+args.value("P")).toStdString().c_str());
        });
        m_menu->addAction(action_wifi);
    } else if (m_type==URL) {
        auto* action_url = new QAction("打开链接");
        connect(action_url, &QAction::triggered, [=](){
            system(("xdg-open "+m_code.text).toStdString().c_str());
        });
        m_menu->addAction(action_url);
    }

    m_menu->exec(QCursor::pos());
    m_menu->deleteLater();
    m_menu = nullptr;
}
