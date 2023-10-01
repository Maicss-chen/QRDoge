//
// Created by maicss on 23-9-30.
//

#ifndef QT_DOG_QRDOGE_H
#define QT_DOG_QRDOGE_H

#include <QObject>
#include <QScreen>
#include <QThread>
#include "DogeThread.h"
#include "Structs.h"
#include "zxing/ZXingQtReader.h"


class QRDoge : public QObject{
    Q_OBJECT
public:
    QRDoge();
    void listen();
protected:

private slots:
    void gazeCode(CodeItem code);
    void gazeCodeLost();
private:
    DogeThread* dogeThread;
};


#endif //QT_DOG_QRDOGE_H
