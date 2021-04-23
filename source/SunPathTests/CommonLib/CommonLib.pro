TEMPLATE = lib
DEFINES += COMMONLIB_EXPORT

QT -= gui
include("../../config.pri")

LIBS += -L../../SunPathLib
LIBS += -lSunPathLib

HEADERS += \
    CommonLib.h \
    common.h

SOURCES += \
    common.cpp
