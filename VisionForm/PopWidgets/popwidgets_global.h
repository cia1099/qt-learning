#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(POPWIDGETS_LIB)
#  define POPWIDGETS_EXPORT Q_DECL_EXPORT
# else
#  define POPWIDGETS_EXPORT Q_DECL_IMPORT
# endif
#else
# define POPWIDGETS_EXPORT
#endif
