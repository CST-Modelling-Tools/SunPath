#pragma once

#include <QtCore/qglobal.h>

#ifdef SUNPATHLIB_EXPORT
    #define SUNPATHLIB Q_DECL_EXPORT
#else
    #define SUNPATHLIB Q_DECL_IMPORT
#endif
