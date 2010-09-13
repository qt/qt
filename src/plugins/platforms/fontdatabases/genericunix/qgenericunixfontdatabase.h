#ifndef QGENERICUNIXFONTDATABASE_H
#define QGENERICUNIXFONTDATABASE_H

#ifdef Q_FONTCONFIGDATABASE
#include "qfontconfigdatabase.h"
typedef QFontconfigDatabase QGenericUnixFontDatabase;
#else
#include "qbasicunixfontdatabase.h"
typedef QBasicUnixFontDatabase QGenericUnixFontDatabase;
#endif //Q_FONTCONFIGDATABASE

#endif // QGENERICUNIXFONTDATABASE_H
