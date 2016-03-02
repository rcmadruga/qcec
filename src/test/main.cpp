#include "qceckeyboardmanager.h"

#include <QCoreApplication>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Fuzzy Bear Must Die");
    QCECKeyboardManager *pop = new QCECKeyboardManager;
    return app.exec();
}
