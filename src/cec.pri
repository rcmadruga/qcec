LOCALDIR = $${TOPLEVEL}/local
INCLUDEPATH += $${LOCALDIR}/include
# Our plugin assumes it is shipped with libcec in the same path
LIBS += -L$${LOCALDIR}/lib -lcec -lbcm_host -ldl
QT += platformsupport-private
