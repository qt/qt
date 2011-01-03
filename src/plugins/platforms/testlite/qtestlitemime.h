#ifndef QTESTLITEMIME_H
#define QTESTLITEMIME_H

#include <private/qdnd_p.h>

#include <QtGui/QClipboard>

#include "qtestliteintegration.h"
#include "qtestliteclipboard.h"

class QTestLiteMime : public QInternalMimeData {
    Q_OBJECT
public:
    QTestLiteMime();
    ~QTestLiteMime();

    static QList<Atom> mimeAtomsForFormat(Display *display, const QString &format);
    static QString mimeAtomToString(Display *display, Atom a);
    static bool mimeDataForAtom(Display *display, Atom a, QMimeData *mimeData, QByteArray *data, Atom *atomFormat, int *dataFormat);
    static QStringList mimeFormatsForAtom(Display *display, Atom a);
    static Atom mimeStringToAtom(Display *display, const QString &mimeType);
    static QVariant mimeConvertToFormat(Display *display, Atom a, const QByteArray &data, const QString &format, QVariant::Type requestedType, const QByteArray &encoding);
    static Atom mimeAtomForFormat(Display *display, const QString &format, QVariant::Type requestedType, const QList<Atom> &atoms, QByteArray *requestedEncoding);
};

#endif // QTESTLITEMIME_H
