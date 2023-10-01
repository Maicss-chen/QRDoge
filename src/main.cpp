#include <QApplication>
#include "HoverButton.h"
#include "QRDoge.h"
#include <QWindow>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qRegisterMetaType<QWindow::Visibility>("QWindow::Visibility");
    qRegisterMetaType<CodeItem>("CodeItem");
    QRDoge doge;
    doge.listen();
    return QApplication::exec();
}
