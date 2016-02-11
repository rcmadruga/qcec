#ifndef QCECKEYBOARDMANAGER
#define QCECKEYBOARDMANAGER

#include <libcec/cec.h>

#include <QObject>

class QCECKeyboardManager : public QObject {
public:
    QCECKeyboardManager(QObject *p = 0);
    ~QCECKeyboardManager();

    void close();
private:
    CEC::ICECAdapter* cec_adapter;
};
#endif // QCECKEYBOARDMANAGER
