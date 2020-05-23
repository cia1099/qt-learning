#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(SHAPE_LIB)
#  define SHAPE_EXPORT Q_DECL_EXPORT
# else
#  define SHAPE_EXPORT Q_DECL_IMPORT
# endif
#else
# define SHAPE_EXPORT
#endif
