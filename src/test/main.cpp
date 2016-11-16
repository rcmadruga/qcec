#include "qceckeyboardmanager.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char** argv)
{
    qDebug() << "Logging the instantiation of the CEC plugin";
    QCoreApplication app(argc, argv);
    app.setApplicationName("Fuzzy Bear Must Die");
    QCECKeyboardManager *pop = new QCECKeyboardManager(nullptr, true);
    return app.exec();
}
