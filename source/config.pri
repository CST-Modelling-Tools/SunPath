VERSION = 0.1.0.0
INCLUDEPATH += $$PWD

CONFIG += c++11
CONFIG -= debug_and_release # separate folders for debug and release
CONFIG += skip_target_version_ext # do not append the dll names with version number
#CONFIG += silent # for shorter compile messages

CONFIG(eigen) {
    INCLUDEPATH += $$PWD/../dependencies/eigen-3.3.9
}

INSTALL_PREFIX = $$OUT_PWD/../../install
