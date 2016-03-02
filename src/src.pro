include($$PWD/cec.pri)

HEADERS += qceckeyboardmanager.h
SOURCES += main.cpp \
    qceckeyboardmanager.cpp

INCLUDEPATH += .

TARGET = qceckeyboardplugin

PLUGIN_TYPE = generic
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QCECKeyboardPlugin
load(qt_plugin)

DESTDIR=$${TOPLEVEL}/out

OTHER_FILES += \
    ceckeyboard.json
