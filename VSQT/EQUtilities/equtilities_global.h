#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(EQUTILITIES_LIB)
#  define EQUTILITIES_EXPORT Q_DECL_EXPORT
# else
#  define EQUTILITIES_EXPORT Q_DECL_IMPORT
# endif
#else
# define EQUTILITIES_EXPORT
#endif
