TEMPLATE = app
#TARGET = SunPath # SunPath.exe
#DESTDIR = ../..

#CONFIG += console # show terminal on windows
include(../config.pri)
LIBS += -L../SunPathLib
LIBS += -lSunPathLib

HEADERS += \
    Proteas/tasks.h \
    Seville/FormatSeville.h \
    Seville/tasksSeville.h \
    common.h

SOURCES += \
    Proteas/tasks.cpp \
    Seville/FormatSeville.cpp \
    Seville/tasksSeville.cpp \
    common.cpp \
    main.cpp


bin.files += SunPathApp.exe
bin.path = $$INSTALL_PREFIX/bin
INSTALLS *= bin
