#ifndef QPLATFORMFONTDATABASE_QPA_H
#define QPLATFORMFONTDATABASE_QPA_H

#include <QtCore/qconfig.h>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtGui/QFontDatabase>
#include <QtGui/private/qfont_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QWritingSystemsPrivate;

class Q_GUI_EXPORT QSupportedWritingSystems
{
public:

    QSupportedWritingSystems();
    QSupportedWritingSystems(const QSupportedWritingSystems &other);
    QSupportedWritingSystems &operator=(const QSupportedWritingSystems &other);
    ~QSupportedWritingSystems();

    void setSupported(QFontDatabase::WritingSystem, bool supported = true);
    bool supported(QFontDatabase::WritingSystem) const;

private:
    void detach();

    QWritingSystemsPrivate *d;

    friend Q_GUI_EXPORT bool operator==(const QSupportedWritingSystems &, const QSupportedWritingSystems &);
    friend Q_GUI_EXPORT bool operator!=(const QSupportedWritingSystems &, const QSupportedWritingSystems &);
};

Q_GUI_EXPORT bool operator==(const QSupportedWritingSystems &, const QSupportedWritingSystems &);
Q_GUI_EXPORT bool operator!=(const QSupportedWritingSystems &, const QSupportedWritingSystems &);

class QFontRequestPrivate;

class Q_GUI_EXPORT QPlatformFontDatabase
{
public:
    virtual void populateFontDatabase();
    virtual QFontEngine *fontEngine(const QFontDef &fontDef, QUnicodeTables::Script script, void *handle);
    virtual QStringList fallbacksForFamily(const QString family, const QFont::Style &style, const QUnicodeTables::Script &script) const;
    virtual QStringList addApplicationFont(const QByteArray &fontData, const QString &fileName);
    virtual void releaseHandle(void *handle);

    virtual QString fontDir() const;

    //callback
    static void registerQPF2Font(const QByteArray &dataArray, void *handle);
    static void registerFont(const QString &familyname, const QString &foundryname, int weight,
                             QFont::Style style, int stetch, bool antialiased, bool scalable, int pixelSize,
                             const QSupportedWritingSystems &writingSystems, void *handle);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLATFORMFONTDATABASE_QPA_H
