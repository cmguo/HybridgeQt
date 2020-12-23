#ifndef HYBRIDGEQT_GLOBAL_H
#define HYBRIDGEQT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(HYBRIDGEQT_LIBRARY)
#  define HYBRIDGEQT_EXPORT Q_DECL_EXPORT
#else
#  define HYBRIDGEQT_EXPORT Q_DECL_IMPORT
#endif

#endif // HYBRIDGEQT_GLOBAL_H