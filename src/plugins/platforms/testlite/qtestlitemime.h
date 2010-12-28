#ifndef QTESTLITEMIME_H
#define QTESTLITEMIME_H

#include <private/qdnd_p.h>

#include <QtGui/QClipboard>

#include "qtestliteintegration.h"
#include "qtestliteclipboard.h"

class QTestLiteMime : public QInternalMimeData {
    Q_OBJECT
public:
    QTestLiteMime(QClipboard::Mode mode, QTestLiteClipboard *clipboard);
    ~QTestLiteMime();
    bool empty() const;

    static QList<Atom> xdndMimeAtomsForFormat(Display *display, const QString &format);
    static QString xdndMimeAtomToString(Display *display, Atom a);
    static bool xdndMimeDataForAtom(Display *display, Atom a, QMimeData *mimeData, QByteArray *data, Atom *atomFormat, int *dataFormat);
    static QStringList xdndMimeFormatsForAtom(Display *display, Atom a);
    static Atom xdndMimeStringToAtom(Display *display, const QString &mimeType);

    static QVariant xdndMimeConvertToFormat(Display *display, Atom a, const QByteArray &data, const QString &format, QVariant::Type requestedType, const QByteArray &encoding);
    static Atom xdndMimeAtomForFormat(Display *display, const QString &format, QVariant::Type requestedType, const QList<Atom> &atoms, QByteArray *requestedEncoding);


protected:
    virtual bool hasFormat_sys(const QString &mimetype) const;
    virtual QStringList formats_sys() const;

    QVariant retrieveData_sys(const QString &mimetype, QVariant::Type type) const;


private:
    QTestLiteClipboard *m_clipboard;
    Atom modeAtom;
    mutable QStringList formatList;
    mutable QByteArray format_atoms;
};

#endif // QTESTLITEMIME_H
