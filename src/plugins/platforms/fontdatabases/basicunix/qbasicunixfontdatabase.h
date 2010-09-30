#ifndef QBASICUNIXFONTDATABASE_H
#define QBASICUNIXFONTDATABASE_H

#include <QPlatformFontDatabase>
#include <QtCore/QByteArray>
#include <QtCore/QString>

struct FontFile
{
    QString fileName;
    int indexValue;
};

class QBasicUnixFontDatabase : public QPlatformFontDatabase
{
public:
    void populateFontDatabase();
    QFontEngine *fontEngine(const QFontDef &fontDef, QUnicodeTables::Script script, void *handle);
    QStringList fallbacksForFamily(const QString family, const QFont::Style &style, const QUnicodeTables::Script &script) const;
    QStringList addApplicationFont(const QByteArray &fontData, const QString &fileName);
    void releaseHandle(void *handle);

    static QStringList addTTFile(const QByteArray &fontData, const QByteArray &file);
};

#endif // QBASICUNIXFONTDATABASE_H
