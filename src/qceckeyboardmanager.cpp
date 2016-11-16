/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qceckeyboardmanager.h"

// required by cecloader.h
#include <iostream>
#include <libcec/cec.h>
#include <libcec/cecloader.h>

#include "bcm_host.h"

#include <qpa/qwindowsysteminterface.h>
#include <QDebug>

#include <QGuiApplication>

int log_keypress(void* not_used, const CEC::cec_keypress msg)
{
    qDebug() << "Received CEC keycode" << msg.keycode;
}

int handle_keypress(void* not_used, const CEC::cec_keypress msg)
{
    Q_UNUSED(not_used);
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // Mapping is semi arbitrarily based on the behavior of my own TV

    Qt::Key key = Qt::Key(0);
    // nativeKeyCode's manually looked up via xev
    int nativeKeyCode = -1;
    switch(msg.keycode)
    {
    case CEC::CEC_USER_CONTROL_CODE_SELECT:
        key = Qt::Key_Return;
        nativeKeyCode = 36;
        break;
    case CEC::CEC_USER_CONTROL_CODE_UP:
        key = Qt::Key_Up;
        nativeKeyCode = 111;
        break;
    case CEC::CEC_USER_CONTROL_CODE_DOWN:
        key = Qt::Key_Down;
        nativeKeyCode = 116;
        break;
    case CEC::CEC_USER_CONTROL_CODE_LEFT:
        key = Qt::Key_Left;
        nativeKeyCode = 113;
        break;
    case CEC::CEC_USER_CONTROL_CODE_RIGHT:
        key = Qt::Key_Right;
        nativeKeyCode = 114;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER0:
        key = Qt::Key_0;
        nativeKeyCode = 19;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER1:
        key = Qt::Key_1;
        nativeKeyCode = 10;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER2:
        key = Qt::Key_2;
        nativeKeyCode = 11;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER3:
        key = Qt::Key_3;
        nativeKeyCode = 12;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER4:
        key = Qt::Key_4;
        nativeKeyCode = 13;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER5:
        key = Qt::Key_5;
        nativeKeyCode = 14;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER6:
        key = Qt::Key_6;
        nativeKeyCode = 15;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER7:
        key = Qt::Key_7;
        nativeKeyCode = 16;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER8:
        key = Qt::Key_8;
        nativeKeyCode = 17;
        break;
    case CEC::CEC_USER_CONTROL_CODE_NUMBER9:
        key = Qt::Key_9;
        nativeKeyCode = 18;
        break;
    case CEC::CEC_USER_CONTROL_CODE_F1_BLUE:
        key = Qt::Key_F1;
        nativeKeyCode = 67;
        break;
    case CEC::CEC_USER_CONTROL_CODE_F2_RED:
        key = Qt::Key_F2;
        nativeKeyCode = 68;
        break;
    case CEC::CEC_USER_CONTROL_CODE_F3_GREEN:
        key = Qt::Key_F3;
        nativeKeyCode = 69;
        break;
    case CEC::CEC_USER_CONTROL_CODE_F4_YELLOW:
        key = Qt::Key_F4;
        nativeKeyCode = 70;
        break;
    case CEC::CEC_USER_CONTROL_CODE_CHANNEL_UP:
        key = Qt::Key_PageUp;
        nativeKeyCode = 112;
        break;
    case CEC::CEC_USER_CONTROL_CODE_CHANNEL_DOWN:
        key = Qt::Key_PageDown;
        nativeKeyCode = 117;
        break;
    case CEC::CEC_USER_CONTROL_CODE_FAST_FORWARD:
        key = Qt::Key_MediaNext;
        nativeKeyCode = 176;
        break;
    case CEC::CEC_USER_CONTROL_CODE_REWIND:
        key = Qt::Key_MediaPrevious;
        nativeKeyCode = 177;
        break;
    case CEC::CEC_USER_CONTROL_CODE_EXIT:
        key = Qt::Key_Escape;
        nativeKeyCode = 9;
        break;
    case CEC::CEC_USER_CONTROL_CODE_AN_RETURN:
        key = Qt::Key_Backspace;
        nativeKeyCode = 22;
        break;
    default: break;
    };

    if (key) {
        QWindowSystemInterface::handleExtendedKeyEvent(0,
                                                       (msg.duration ? QEvent::KeyRelease : QEvent::KeyPress),
                                                       key,
                                                       0,
                                                       nativeKeyCode,
                                                       0,
                                                       0,
                                                       QString(),
                                                       false);
    } else {
        qDebug() << "CEC keypress" << msg.keycode << "received but currently not handled by the qcec keyboard plugin";
        qDebug() << "Feel free to extend it: I don't trust vendor compliance much";
    }

    return 0;
}

QCECKeyboardManager::QCECKeyboardManager(QObject *p, bool logEvents)
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

    if (logEvents)
        cec_callbacks.CBCecKeyPress = &log_keypress;
    else
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

    qDebug() << "Successfully created Qt CEC input plugin";
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
