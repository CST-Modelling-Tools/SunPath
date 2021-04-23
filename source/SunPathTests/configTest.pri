QT += testlib
QT -= gui

include("../config.pri")

LIBS += -L../../SunPathLib
LIBS += -lSunPathLib

LIBS += -L../CommonLib
LIBS += -lCommonLib
INCLUDEPATH += $$PWD
