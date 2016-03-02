include($$PWD/../cec.pri)

TEMPLATE = app
TARGET = cecRuntimeTest
INCLUDEPATH += . ../

# Input
SOURCES += main.cpp ../qceckeyboardmanager.cpp
