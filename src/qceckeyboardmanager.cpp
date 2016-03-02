#include "qceckeyboardmanager.h"

// required by cecloader.h
#include <iostream>
#include <libcec/cec.h>
#include <libcec/cecloader.h>

#include "bcm_host.h"

#include <qpa/qwindowsysteminterface.h>
#include <QDebug>

#include <QGuiApplication>

int handle_keypress(void* not_used, const CEC::cec_keypress msg)
{
    Q_UNUSED(not_used);
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // Mapping is semi arbitrarily based on the behavior of my own TV

    Qt::Key key = Qt::Key(0);
    switch(msg.keycode)
    {
    case CEC::CEC_USER_CONTROL_CODE_SELECT:
        key = Qt::Key_Return;
        break;
    case CEC::CEC_USER_CONTROL_CODE_UP:
        key = Qt::Key_Up;
        break;
    case CEC::CEC_USER_CONTROL_CODE_DOWN:
        key = Qt::Key_Down;
        break;
    case CEC::CEC_USER_CONTROL_CODE_LEFT:
        key = Qt::Key_Left;
        break;
    case CEC::CEC_USER_CONTROL_CODE_RIGHT:
        key = Qt::Key_Right;
        break;
    default: break;
    };

    if (key) {
        QWindowSystemInterface::handleExtendedKeyEvent(0,
                                                       (msg.duration ? QEvent::KeyRelease : QEvent::KeyPress),
                                                       key,
                                                       0, 0, 0, 0,
                                                       QString(),
                                                       false);
    }

    return 0;
}

QCECKeyboardManager::QCECKeyboardManager(QObject *p)
    : QObject(p),
      cec_adapter(0)
{
    bcm_host_init();

    CEC::ICECCallbacks cec_callbacks;
    CEC::libcec_configuration cec_config;
    cec_config.Clear();
    cec_callbacks.Clear();

    QString cecAdvertizedName = "QPi";

    if (!qApp->applicationName().isNull())
        cecAdvertizedName = qApp->applicationName();

    const std::string devicename(cecAdvertizedName.toLocal8Bit().constData());
    devicename.copy(cec_config.strDeviceName, 13u);

    cec_config.clientVersion = CEC::LIBCEC_VERSION_CURRENT;
    cec_config.bActivateSource = 0;
    cec_config.callbacks = &cec_callbacks;
    cec_config.deviceTypes.Add(CEC::CEC_DEVICE_TYPE_RECORDING_DEVICE);

    cec_callbacks.CBCecKeyPress = &handle_keypress;

    cec_adapter = LibCecInitialise(&cec_config);

    if(!cec_adapter) {
        qDebug() << "Could not create CEC adaptor with current config";
    }

    CEC::cec_adapter devices[10];
    int devices_found = cec_adapter->FindAdapters(devices, 10, NULL);
    if(devices_found < 1) {
        qDebug() << "No CEC devices found";
        close();
    }

    if(!cec_adapter->Open(devices[0].comm)) {
        qDebug() << "Can't open device 0 (assumed to be TV)";
        close();
    }
}

QCECKeyboardManager::~QCECKeyboardManager() {
    close();
}

void QCECKeyboardManager::close()
{
    qDebug() << "Closing the CEC device";
    cec_adapter->Close();
    UnloadLibCec(cec_adapter);
    cec_adapter = 0;
}
