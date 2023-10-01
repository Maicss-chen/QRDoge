//
// Created by maicss on 23-10-1.
//

#ifndef QT_DOG_DOGETHREAD_H
#define QT_DOG_DOGETHREAD_H
#include <QThread>
#include "Structs.h"

class DogeThread : public QThread{
    Q_OBJECT
public:
    explicit DogeThread(QObject *parent = nullptr);
    void listen();
signals:
    void codeFound(CodeItem code);
    void codeLost(CodeItem code);
    void gazeCode(CodeItem code);
    void gazeCodeLost();
protected:
    void run() override;
private:
    /**
     * 扫描目前所有屏幕中显示的二维码。
     * @return 返回一个Map，Key是QScreen，value是扫描到的二维码解析结果。
     */
    static QList<CodeItem> scan();

    QList<CodeItem> m_map;
    bool gaze(QPoint point, CodeItem& code);
};


#endif //QT_DOG_DOGETHREAD_H
