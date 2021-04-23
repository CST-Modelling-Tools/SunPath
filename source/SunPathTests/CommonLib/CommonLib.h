#pragma once

#include <QtCore/qglobal.h>

#ifdef COMMONLIB_EXPORT
    #define COMMONLIB Q_DECL_EXPORT
#else
    #define COMMONLIB Q_DECL_IMPORT
#endif
