#ifndef QFONTCONFIGDATABASE_H
#define QFONTCONFIGDATABASE_H

#include <QPlatformFontDatabase>
#include "qbasicunixfontdatabase.h"

class QFontconfigDatabase : public QBasicUnixFontDatabase
{
public:
    void populateFontDatabase();
    QFontEngine *fontEngine(const QFontDef &fontDef, QUnicodeTables::Script script, void *handle);
    QStringList fallbacksForFamily(const QString family, const QFont::Style &style, const QUnicodeTables::Script &script) const;
};

#endif // QFONTCONFIGDATABASE_H
