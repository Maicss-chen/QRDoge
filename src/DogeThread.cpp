#include "DogeThread.h"
#include "QCursor"
#include "QApplication"
#include "QPixmap"
#include "zxing/ZXingQtReader.h"

using namespace ZXingQt;

DogeThread::DogeThread(QObject *parent)
        : QThread(parent) {

}
void DogeThread::run() {
    qint64 count = 0; // 事件循环计数
    qint32 CursorWaitCount = 0; // 鼠标静止计数
    QPoint lastCursorPoint; // 上一次鼠标位置，用于判断鼠标静止
    while (true) {
        if (count % 1000 == 0) {
            auto res = scan();
            for (const auto &item: res) {
                int index = m_map.indexOf(item);
                if (index == -1) {
                    emit codeFound(item);
                }
            }
            for (const auto &item: m_map) {
                int index = res.indexOf(item);
                if (index == -1) {
                    emit codeLost(item);
                }
            }
            m_map = res;
        }
        QPoint cursorPoint = QCursor::pos();
        if (cursorPoint == lastCursorPoint) {
            CursorWaitCount++;
        } else {
            CursorWaitCount = 0;
            emit gazeCodeLost();
        }
        if (CursorWaitCount == 500) {
            CodeItem code;
            if (gaze(cursorPoint, code)) {
                emit gazeCode(code);
            }
        }
        lastCursorPoint = cursorPoint;
        count++;
        QThread::msleep(1);
    }
}

bool DogeThread::gaze(QPoint point, CodeItem &code) {
    for (const auto &item: m_map) {
        QPolygon polygon;
        polygon.append(item.position.topLeft);
        polygon.append(item.position.topRight);
        polygon.append(item.position.bottomRight);
        polygon.append(item.position.bottomLeft);

        if (polygon.containsPoint(point, Qt::FillRule::WindingFill)) {
            code = item;
            return true;
        }
    }
    return false;
}

QList<CodeItem> DogeThread::scan() {
    QList<CodeItem> res;
    auto screens = QApplication::screens();
    if (screens.empty()) {
        qDebug()<<"No screens were found!";
        return res;
    }

    auto hints = DecodeHints()
            .setFormats(BarcodeFormat::Any)
            .setTryRotate(true)
            .setMaxNumberOfSymbols(20)
            .setTryHarder(true);

    for (auto screen : screens) {
        QImage screen_img = screen->grabWindow(0).toImage();

        if (screen_img.isNull()) {
            qDebug() << "Could not load the filename as an image:" ;
        }

        auto results = ReadBarcodes(screen_img, hints);

        for (const auto& item:results) {
            CodeItem c_item;
            c_item.text = item.text();
            switch (item.contentType()) {
                case ContentType::Text:
                    c_item.type = CodeType::Text;
                    break;
                case ContentType::Binary:
                    c_item.type = CodeType::Binary;
                    break;
                case ContentType::Mixed:
                    c_item.type = CodeType::Mixed;
                    break;
                case ContentType::GS1:
                    c_item.type = CodeType::GS1;
                    break;
                case ContentType::ISO15434:
                    c_item.type = CodeType::ISO15434;
                    break;
                case ContentType::UnknownECI:
                    c_item.type = CodeType::UnknownECI;
                    break;
            }
            c_item.position.topLeft = QPoint(item.position().topLeft().x(), item.position().topLeft().y());
            c_item.position.topRight = QPoint(item.position().topRight().x(), item.position().topRight().y());
            c_item.position.bottomLeft = QPoint(item.position().bottomLeft().x(), item.position().bottomLeft().y());
            c_item.position.bottomRight = QPoint(item.position().bottomRight().x(), item.position().bottomRight().y());
            c_item.screen = screen;
            res.append(c_item);
        }
    }
    return res;
}

void DogeThread::listen() {
    this->start();
}

