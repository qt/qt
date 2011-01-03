#include "qtestlitemime.h"

#include "qtestlitestaticinfo.h"
#include "qtestlitescreen.h"

#include <QtCore/QTextCodec>

QTestLiteMime::QTestLiteMime(QClipboard::Mode mode, QTestLiteClipboard *clipboard)
    : QInternalMimeData(), m_clipboard(clipboard)
{
    switch (mode) {
    case QClipboard::Selection:
        modeAtom = XA_PRIMARY;
        break;

    case QClipboard::Clipboard:
        modeAtom = QTestLiteStaticInfo::atom(QTestLiteStaticInfo::CLIPBOARD);
        break;

    default:
        qWarning("QTestLiteMime: Internal error: Unsupported clipboard mode");
        break;
    }
}

QTestLiteMime::~QTestLiteMime()
{
}

bool QTestLiteMime::empty() const
{
    Window win = XGetSelectionOwner(m_clipboard->screen()->display(), modeAtom);

    return win == XNone;
}

QStringList QTestLiteMime::formats_sys() const
{
    if (empty())
        return QStringList();

    if (!formatList.count()) {
        // get the list of targets from the current clipboard owner - we do this
        // once so that multiple calls to this function don't require multiple
        // server round trips...

        format_atoms = m_clipboard->getDataInFormat(modeAtom,QTestLiteStaticInfo::atom(QTestLiteStaticInfo::TARGETS));

        if (format_atoms.size() > 0) {
            Atom *targets = (Atom *) format_atoms.data();
            int size = format_atoms.size() / sizeof(Atom);

            for (int i = 0; i < size; ++i) {
                if (targets[i] == 0)
                    continue;

                QStringList formatsForAtom = xdndMimeFormatsForAtom(m_clipboard->screen()->display(),targets[i]);
                for (int j = 0; j < formatsForAtom.size(); ++j) {
                    if (!formatList.contains(formatsForAtom.at(j)))
                        formatList.append(formatsForAtom.at(j));
                }
            }
        }
    }

    return formatList;
}

bool QTestLiteMime::hasFormat_sys(const QString &format) const
{
    QStringList list = formats();
    return list.contains(format);
}

QVariant QTestLiteMime::retrieveData_sys(const QString &fmt, QVariant::Type requestedType) const
{
    if (fmt.isEmpty() || empty())
        return QByteArray();

    (void)formats(); // trigger update of format list

    QList<Atom> atoms;
    Atom *targets = (Atom *) format_atoms.data();
    int size = format_atoms.size() / sizeof(Atom);
    for (int i = 0; i < size; ++i)
        atoms.append(targets[i]);

    QByteArray encoding;
    Atom fmtatom = xdndMimeAtomForFormat(m_clipboard->screen()->display(),fmt, requestedType, atoms, &encoding);

    if (fmtatom == 0)
        return QVariant();

    return xdndMimeConvertToFormat(m_clipboard->screen()->display(),fmtatom, m_clipboard->getDataInFormat(modeAtom,fmtatom), fmt, requestedType, encoding);
}


QString QTestLiteMime::xdndMimeAtomToString(Display *display, Atom a)
{
    if (!a) return 0;

    if (a == XA_STRING || a == QTestLiteStatic::atom(QTestLiteStatic::UTF8_STRING)) {
        return "text/plain"; // some Xdnd clients are dumb
    }
    char *atom = XGetAtomName(display, a);
    QString result = QString::fromLatin1(atom);
    XFree(atom);
    return result;
}

Atom QTestLiteMime::xdndMimeStringToAtom(Display *display, const QString &mimeType)
{
    if (mimeType.isEmpty())
        return 0;
    return XInternAtom(display, mimeType.toLatin1().constData(), False);
}

QStringList QTestLiteMime::xdndMimeFormatsForAtom(Display *display, Atom a)
{
    QStringList formats;
    if (a) {
        QString atomName = xdndMimeAtomToString(display, a);
        formats.append(atomName);

        // special cases for string type
        if (a == QTestLiteStatic::atom(QTestLiteStatic::UTF8_STRING)
                || a == XA_STRING
                || a == QTestLiteStatic::atom(QTestLiteStatic::TEXT)
                || a == QTestLiteStatic::atom(QTestLiteStatic::COMPOUND_TEXT))
            formats.append(QLatin1String("text/plain"));

        // special cases for uris
        if (atomName == QLatin1String("text/x-moz-url"))
            formats.append(QLatin1String("text/uri-list"));

        // special case for images
        if (a == XA_PIXMAP)
            formats.append(QLatin1String("image/ppm"));
    }
    return formats;
}

bool QTestLiteMime::xdndMimeDataForAtom(Display *display, Atom a, QMimeData *mimeData, QByteArray *data, Atom *atomFormat, int *dataFormat)
{
    bool ret = false;
    *atomFormat = a;
    *dataFormat = 8;
    QString atomName = xdndMimeAtomToString(display, a);
    if (QInternalMimeData::hasFormatHelper(atomName, mimeData)) {
        *data = QInternalMimeData::renderDataHelper(atomName, mimeData);
        if (atomName == QLatin1String("application/x-color"))
            *dataFormat = 16;
        ret = true;
    } else {
        if ((a == QTestLiteStatic::atom(QTestLiteStatic::UTF8_STRING)
             || a == XA_STRING
             || a == QTestLiteStatic::atom(QTestLiteStatic::TEXT)
             || a == QTestLiteStatic::atom(QTestLiteStatic::COMPOUND_TEXT))
            && QInternalMimeData::hasFormatHelper(QLatin1String("text/plain"), mimeData)) {
            if (a == QTestLiteStatic::atom(QTestLiteStatic::UTF8_STRING)){
                *data = QInternalMimeData::renderDataHelper(QLatin1String("text/plain"), mimeData);
                ret = true;
            } else if (a == XA_STRING) {
                *data = QString::fromUtf8(QInternalMimeData::renderDataHelper(
                        QLatin1String("text/plain"), mimeData)).toLocal8Bit();
                ret = true;
            } else if (a == QTestLiteStatic::atom(QTestLiteStatic::TEXT)
                       || a == QTestLiteStatic::atom(QTestLiteStatic::COMPOUND_TEXT)) {
                // the ICCCM states that TEXT and COMPOUND_TEXT are in the
                // encoding of choice, so we choose the encoding of the locale
                QByteArray strData = QString::fromUtf8(QInternalMimeData::renderDataHelper(
                                     QLatin1String("text/plain"), mimeData)).toLocal8Bit();
                char *list[] = { strData.data(), NULL };

                XICCEncodingStyle style = (a == QTestLiteStatic::atom(QTestLiteStatic::COMPOUND_TEXT))
                                        ? XCompoundTextStyle : XStdICCTextStyle;
                XTextProperty textprop;
                if (list[0] != NULL
                    && XmbTextListToTextProperty(display, list, 1, style,
                                                 &textprop) == Success) {
                    *atomFormat = textprop.encoding;
                    *dataFormat = textprop.format;
                    *data = QByteArray((const char *) textprop.value, textprop.nitems * textprop.format / 8);
                    ret = true;

                    XFree(textprop.value);
                }
            }
        } else if (atomName == QLatin1String("text/x-moz-url") &&
                   QInternalMimeData::hasFormatHelper(QLatin1String("text/uri-list"), mimeData)) {
            QByteArray uri = QInternalMimeData::renderDataHelper(
                             QLatin1String("text/uri-list"), mimeData).split('\n').first();
            QString mozUri = QString::fromLatin1(uri, uri.size());
            mozUri += QLatin1Char('\n');
            *data = QByteArray(reinterpret_cast<const char *>(mozUri.utf16()), mozUri.length() * 2);
            ret = true;
        } else if ((a == XA_PIXMAP || a == XA_BITMAP) && mimeData->hasImage()) {
            QPixmap pm = qvariant_cast<QPixmap>(mimeData->imageData());
            if (a == XA_BITMAP && pm.depth() != 1) {
                QImage img = pm.toImage();
                img = img.convertToFormat(QImage::Format_MonoLSB);
                pm = QPixmap::fromImage(img);
            }
//            QDragManager *dm = QDragManager::self();
//            if (dm) {
//                Pixmap handle = pm.handle();
//                *data = QByteArray((const char *) &handle, sizeof(Pixmap));
//                dm->xdndMimeTransferedPixmap[dm->xdndMimeTransferedPixmapIndex] = pm;
//                dm->xdndMimeTransferedPixmapIndex =
//                            (dm->xdndMimeTransferedPixmapIndex + 1) % 2;
//                ret = true;
//            }
        }
    }
    return ret && data != 0;
}

QList<Atom> QTestLiteMime::xdndMimeAtomsForFormat(Display *display, const QString &format)
{
    QList<Atom> atoms;
    atoms.append(xdndMimeStringToAtom(display, format));

    // special cases for strings
    if (format == QLatin1String("text/plain")) {
        atoms.append(QTestLiteStatic::atom(QTestLiteStatic::UTF8_STRING));
        atoms.append(XA_STRING);
        atoms.append(QTestLiteStatic::atom(QTestLiteStatic::TEXT));
        atoms.append(QTestLiteStatic::atom(QTestLiteStatic::COMPOUND_TEXT));
    }

    // special cases for uris
    if (format == QLatin1String("text/uri-list")) {
        atoms.append(xdndMimeStringToAtom(display,QLatin1String("text/x-moz-url")));
    }

    //special cases for images
    if (format == QLatin1String("image/ppm"))
        atoms.append(XA_PIXMAP);
    if (format == QLatin1String("image/pbm"))
        atoms.append(XA_BITMAP);

    return atoms;
}

QVariant QTestLiteMime::xdndMimeConvertToFormat(Display *display, Atom a, const QByteArray &data, const QString &format, QVariant::Type requestedType, const QByteArray &encoding)
{
    QString atomName = xdndMimeAtomToString(display,a);
    if (atomName == format)
        return data;

    if (!encoding.isEmpty()
        && atomName == format + QLatin1String(";charset=") + QString::fromLatin1(encoding)) {

        if (requestedType == QVariant::String) {
            QTextCodec *codec = QTextCodec::codecForName(encoding);
            if (codec)
                return codec->toUnicode(data);
        }

        return data;
    }

    // special cases for string types
    if (format == QLatin1String("text/plain")) {
        if (a == QTestLiteStatic::atom(QTestLiteStatic::UTF8_STRING))
            return QString::fromUtf8(data);
        if (a == XA_STRING)
            return QString::fromLatin1(data);
        if (a == QTestLiteStatic::atom(QTestLiteStatic::TEXT)
                || a == QTestLiteStatic::atom(QTestLiteStatic::COMPOUND_TEXT))
            // #### might be wrong for COMPUND_TEXT
            return QString::fromLocal8Bit(data, data.size());
    }

    // special case for uri types
    if (format == QLatin1String("text/uri-list")) {
        if (atomName == QLatin1String("text/x-moz-url")) {
            // we expect this as utf16 <url><space><title>
            // the first part is a url that should only contain ascci char
            // so it should be safe to check that the second char is 0
            // to verify that it is utf16
            if (data.size() > 1 && data.at(1) == 0)
                return QString::fromRawData((const QChar *)data.constData(),
                                data.size() / 2).split(QLatin1Char('\n')).first().toLatin1();
        }
    }

    // special cas for images
    if (format == QLatin1String("image/ppm")) {
        if (a == XA_PIXMAP && data.size() == sizeof(Pixmap)) {
//            Pixmap xpm = *((Pixmap*)data.data());
//            if (!xpm)
//                return QByteArray();
//            QPixmap qpm = QPixmap::fromX11Pixmap(xpm);
//            QImageWriter imageWriter;
//            imageWriter.setFormat("PPMRAW");
//            QImage imageToWrite = qpm.toImage();
//            QBuffer buf;
//            buf.open(QIODevice::WriteOnly);
//            imageWriter.setDevice(&buf);
//            imageWriter.write(imageToWrite);
//            return buf.buffer();
            return QVariant();
        }
    }
    return QVariant();
}

Atom QTestLiteMime::xdndMimeAtomForFormat(Display *display, const QString &format, QVariant::Type requestedType, const QList<Atom> &atoms, QByteArray *requestedEncoding)
{
    requestedEncoding->clear();

    // find matches for string types
    if (format == QLatin1String("text/plain")) {
        if (atoms.contains(QTestLiteStatic::atom(QTestLiteStatic::UTF8_STRING)))
            return QTestLiteStatic::atom(QTestLiteStatic::UTF8_STRING);
        if (atoms.contains(QTestLiteStatic::atom(QTestLiteStatic::COMPOUND_TEXT)))
            return QTestLiteStatic::atom(QTestLiteStatic::COMPOUND_TEXT);
        if (atoms.contains(QTestLiteStatic::atom(QTestLiteStatic::TEXT)))
            return QTestLiteStatic::atom(QTestLiteStatic::TEXT);
        if (atoms.contains(XA_STRING))
            return XA_STRING;
    }

    // find matches for uri types
    if (format == QLatin1String("text/uri-list")) {
        Atom a = xdndMimeStringToAtom(display,format);
        if (a && atoms.contains(a))
            return a;
        a = xdndMimeStringToAtom(display,QLatin1String("text/x-moz-url"));
        if (a && atoms.contains(a))
            return a;
    }

    // find match for image
    if (format == QLatin1String("image/ppm")) {
        if (atoms.contains(XA_PIXMAP))
            return XA_PIXMAP;
    }

    // for string/text requests try to use a format with a well-defined charset
    // first to avoid encoding problems
    if (requestedType == QVariant::String
        && format.startsWith(QLatin1String("text/"))
        && !format.contains(QLatin1String("charset="))) {

        QString formatWithCharset = format;
        formatWithCharset.append(QLatin1String(";charset=utf-8"));

        Atom a = xdndMimeStringToAtom(display,formatWithCharset);
        if (a && atoms.contains(a)) {
            *requestedEncoding = "utf-8";
            return a;
        }
    }

    Atom a = xdndMimeStringToAtom(display,format);
    if (a && atoms.contains(a))
        return a;

    return 0;
}
