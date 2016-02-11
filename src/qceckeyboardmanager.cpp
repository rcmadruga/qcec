#include "qceckeyboardmanager.h"

// required by following header
#include <iostream>
#include <libcec/cecloader.h>

#include "bcm_host.h"

#include <qpa/qwindowsysteminterface.h>
#include <QDebug>

#include <QCoreApplication>

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
    cec_config.deviceTypes.Add(CEC::CEC_DEVICE_TYPE_TUNER);
    cec_config.deviceTypes.Add(CEC::CEC_DEVICE_TYPE_PLAYBACK_DEVICE);
    cec_config.deviceTypes.Add(CEC::CEC_DEVICE_TYPE_AUDIO_SYSTEM);

    cec_callbacks.CBCecKeyPress = &handle_keypress;

    cec_adapter = LibCecInitialise(&cec_config);

    if(!cec_adapter) {
        qWarning("Could not create CEC adaptor with current config");
        return;
    }

    CEC::cec_adapter devices[255];
    int devices_found = cec_adapter->FindAdapters(devices, 255, NULL);
    if(devices_found < 1) {
        qWarning("No CEC devices found");
        close();
        return;
    }

    if(!cec_adapter->Open(devices[0].comm)) {
        qWarning("Can't open device 0 (assumed to be TV)");
        close();
        return;
    }
}

QCECKeyboardManager::~QCECKeyboardManager() {
    close();
}

void QCECKeyboardManager::close()
{
    cec_adapter->Close();
    UnloadLibCec(cec_adapter);
    cec_adapter = 0;
}
