#include "QRDoge.h"
#include "HoverButton.h"
#include <QApplication>
#include <QThread>

using namespace ZXingQt;

QRDoge::QRDoge()
    : dogeThread(new DogeThread(this)) {
    connect(dogeThread, &DogeThread::gazeCode, this, &QRDoge::gazeCode);
    connect(dogeThread, &DogeThread::gazeCodeLost, this, &QRDoge::gazeCodeLost);
}

void QRDoge::listen() {
    dogeThread->listen();
}

void QRDoge::gazeCode(const CodeItem& code) {
    HoverButton::pop(QCursor::pos(), code);
}

void QRDoge::gazeCodeLost() {
    HoverButton::hide();
}

