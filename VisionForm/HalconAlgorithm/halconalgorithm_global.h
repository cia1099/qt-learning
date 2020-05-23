#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(HALCONALGORITHM_LIB)
#  define HALCONALGORITHM_EXPORT Q_DECL_EXPORT
# else
#  define HALCONALGORITHM_EXPORT Q_DECL_IMPORT
# endif
#else
# define HALCONALGORITHM_EXPORT
#endif
