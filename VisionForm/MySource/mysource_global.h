#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MYSOURCE_LIB)
#  define MYSOURCE_EXPORT Q_DECL_EXPORT
# else
#  define MYSOURCE_EXPORT Q_DECL_IMPORT
# endif
#else
# define MYSOURCE_EXPORT
#endif
