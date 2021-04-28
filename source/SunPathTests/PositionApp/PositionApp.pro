include("../../config.pri")

QT += widgets
LIBS += -L../../SunPathLib
LIBS += -lSunPathLib

#HEADERS += \
#    PositionTest.h

SOURCES += \
    Dialog.cpp \
    main.cpp

FORMS += \
    Dialog.ui

HEADERS += \
    Dialog.h
