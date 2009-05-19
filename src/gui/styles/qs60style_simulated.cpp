/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qs60style.h"
#include "qs60style_p.h"
#include "qfile.h"
#include "qhash.h"
#include "qapplication.h"
#include "qpainter.h"
#include "qpicture.h"
#include "qstyleoption.h"
#include "qtransform.h"
#include "qlayout.h"
#include "qpixmapcache.h"
#include "qmetaobject.h"

#if !defined(QT_NO_STYLE_S60) || defined(QT_PLUGIN)

QT_BEGIN_NAMESPACE

class QS60StyleModeSpecifics
{
public:
    static QPixmap skinnedGraphics(QS60StyleEnums::SkinParts stylepart,
        const QSize &size, QS60StylePrivate::SkinElementFlags flags);
    static QHash<QString, QPicture> m_partPictures;
    static QHash<QPair<QString , int>, QColor> m_colors;
};
QHash<QString, QPicture> QS60StyleModeSpecifics::m_partPictures;
QHash<QPair<QString , int>, QColor> QS60StyleModeSpecifics::m_colors;

QS60StylePrivate::QS60StylePrivate()
{
    setCurrentLayout(0);
}

QS60StylePrivate::~QS60StylePrivate()
{
}

short QS60StylePrivate::pixelMetric(int metric)
{
    Q_ASSERT(metric < MAX_PIXELMETRICS);
    const short returnValue = m_pmPointer[metric];
    if (returnValue==-909)
        return -1;
    return returnValue;
}

QColor QS60StylePrivate::s60Color(QS60StyleEnums::ColorLists list,
    int index, const QStyleOption *option)
{
    const QString listKey = QS60Style::colorListKeys().at(list);
    return QS60StylePrivate::stateColor(
        QS60StyleModeSpecifics::m_colors.value(QPair<QString, int>(listKey, index)),
        option
    );
}

QPixmap QS60StylePrivate::part(QS60StyleEnums::SkinParts part, const QSize &size,
                               QS60StylePrivate::SkinElementFlags flags)
{
    const QString partKey = QS60Style::partKeys().at(part);
    const QPicture partPicture = QS60StyleModeSpecifics::m_partPictures.value(partKey);
    QSize partSize(partPicture.boundingRect().size());
    if (flags & (SF_PointEast | SF_PointWest)) {
        const int temp = partSize.width();
        partSize.setWidth(partSize.height());
        partSize.setHeight(temp);
    }
    const qreal scaleX = size.width() / (qreal)partSize.width();
    const qreal scaleY = size.height() / (qreal)partSize.height();

    QImage partImage(size, QImage::Format_ARGB32);
    partImage.fill(Qt::transparent);
    QPainter resultPainter(&partImage);
    QTransform t;

    if (flags & SF_PointEast)
        t.translate(size.width(), 0);
    else if (flags & SF_PointSouth)
        t.translate(size.width(), size.height());
    else if (flags & SF_PointWest)
        t.translate(0, size.height());

    t.scale(scaleX, scaleY);

    if (flags & SF_PointEast)
        t.rotate(90);
    else if (flags & SF_PointSouth)
        t.rotate(180);
    else if (flags & SF_PointWest)
        t.rotate(270);

    resultPainter.setTransform(t, true);
    const_cast<QPicture *>(&partPicture)->play(&resultPainter);
    resultPainter.end();

    QPixmap result = QPixmap::fromImage(partImage);
    if (flags & SF_StateDisabled) {
        // TODO: fix this
        QStyleOption opt;
//        opt.palette = q->standardPalette();
        result = QApplication::style()->generatedIconPixmap(QIcon::Disabled, result, &opt);
    }

    return result;
}

QPixmap QS60StylePrivate::frame(SkinFrameElements frame, const QSize &size,
    SkinElementFlags flags)
{
    const QS60StyleEnums::SkinParts center =        m_frameElementsData[frame].center;
    const QS60StyleEnums::SkinParts topLeft =       QS60StyleEnums::SkinParts(center - 8);
    const QS60StyleEnums::SkinParts topRight =      QS60StyleEnums::SkinParts(center - 7);
    const QS60StyleEnums::SkinParts bottomLeft =    QS60StyleEnums::SkinParts(center - 6);
    const QS60StyleEnums::SkinParts bottomRight =   QS60StyleEnums::SkinParts(center - 5);
    const QS60StyleEnums::SkinParts top =           QS60StyleEnums::SkinParts(center - 4);
    const QS60StyleEnums::SkinParts bottom =        QS60StyleEnums::SkinParts(center - 3);
    const QS60StyleEnums::SkinParts left =          QS60StyleEnums::SkinParts(center - 2);
    const QS60StyleEnums::SkinParts right =         QS60StyleEnums::SkinParts(center - 1);

    // The size of topLeft defines all other sizes
    const QSize cornerSize(partSize(topLeft));
    // if frame is so small that corners would cover it completely, draw only center piece
    const bool drawOnlyCenter =
         2 * cornerSize.width() + 1 >= size.width() || 2 * cornerSize.height() + 1 >= size.height();

    const int cornerWidth = cornerSize.width();
    const int cornerHeight = cornerSize.height();
    const int rectWidth = size.width();
    const int rectHeight = size.height();
    const QRect rect(QPoint(), size);

    const QRect topLeftRect = QRect(rect.topLeft(), cornerSize);
    const QRect topRect = rect.adjusted(cornerWidth, 0, -cornerWidth, -(rectHeight - cornerHeight));
    const QRect topRightRect = topLeftRect.translated(rectWidth - cornerWidth, 0);
    const QRect rightRect = rect.adjusted(rectWidth - cornerWidth, cornerHeight, 0, -cornerHeight);
    const QRect bottomRightRect = topRightRect.translated(0, rectHeight - cornerHeight);
    const QRect bottomRect = topRect.translated(0, rectHeight - cornerHeight);
    const QRect bottomLeftRect = topLeftRect.translated(0, rectHeight - cornerHeight);
    const QRect leftRect = rightRect.translated(cornerWidth - rectWidth, 0);
    const QRect centerRect = drawOnlyCenter ? rect : rect.adjusted(cornerWidth, cornerWidth, -cornerWidth, -cornerWidth);

    QImage result(size, QImage::Format_ARGB32);
    QPainter painter(&result);

#if 0
    painter.save();
    painter.setOpacity(.3);
    painter.fillRect(topLeftRect, Qt::red);
    painter.fillRect(topRect, Qt::green);
    painter.fillRect(topRightRect, Qt::blue);
    painter.fillRect(rightRect, Qt::green);
    painter.fillRect(bottomRightRect, Qt::red);
    painter.fillRect(bottomRect, Qt::blue);
    painter.fillRect(bottomLeftRect, Qt::green);
    painter.fillRect(leftRect, Qt::blue);
    painter.fillRect(centerRect, Qt::red);
    painter.restore();
#else
    drawPart(topLeft, &painter, topLeftRect, flags);
    drawPart(top, &painter, topRect, flags);
    drawPart(topRight, &painter, topRightRect, flags);
    drawPart(right, &painter, rightRect, flags);
    drawPart(bottomRight, &painter, bottomRightRect, flags);
    drawPart(bottom, &painter, bottomRect, flags);
    drawPart(bottomLeft, &painter, bottomLeftRect, flags);
    drawPart(left, &painter, leftRect, flags);
    drawPart(center, &painter, centerRect, flags);
#endif

    return QPixmap::fromImage(result);
}

void QS60StylePrivate::setStyleProperty_specific(const char *name, const QVariant &value)
{
    setStyleProperty(name, value);
}

QVariant QS60StylePrivate::styleProperty_specific(const char *name) const
{
    return styleProperty(name);
}

QPixmap QS60StylePrivate::backgroundTexture()
{
    static QPixmap result;
    // Poor mans caching. + Making sure that there is always only one background image in memory at a time

/*
    TODO: 1) Hold the background QPixmap as pointer in a static class member.
             Also add a deleteBackground() function and call that in ~QS60StylePrivate()
          2) Don't cache the background at all as soon as we have native pixmap support
*/

    if (!m_backgroundValid) {
        result = QPixmap();
        result = part(QS60StyleEnums::SP_QsnBgScreen, QApplication::activeWindow()->size());
        m_backgroundValid = true;
    }
    return result;
}

bool QS60StylePrivate::isTouchSupported()
{
#ifdef QT_KEYPAD_NAVIGATION
    return !QApplication::keypadNavigationEnabled();
#else
    return true;
#endif
}

bool QS60StylePrivate::isToolBarBackground()
{
    return true;
}


QFont QS60StylePrivate::s60Font_specific(QS60StyleEnums::FontCategories fontCategory, int pointSize)
{
    QFont result;
    result.setPointSize(pointSize);
    switch (fontCategory) {
        case QS60StyleEnums::FC_Primary:
            result.setBold(true);
            break;
        case QS60StyleEnums::FC_Secondary:
        case QS60StyleEnums::FC_Title:
        case QS60StyleEnums::FC_PrimarySmall:
        case QS60StyleEnums::FC_Digital:
        case QS60StyleEnums::FC_Undefined:
        default:
            break;
    }
    return result;
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QStringList, enumPartKeys, {
    const int enumIndex = QS60StyleEnums::staticMetaObject.indexOfEnumerator("SkinParts");
    Q_ASSERT(enumIndex >= 0);
    const QMetaEnum metaEnum = QS60StyleEnums::staticMetaObject.enumerator(enumIndex);
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
        const QString enumKey = QString::fromLatin1(metaEnum.key(i));
        QString partKey;
        // Following loop does following conversions: "SP_QgnNoteInfo" to "qgn_note_info"...
        for (int charPosition = 3; charPosition < enumKey.length(); charPosition++) {
            if (charPosition > 3 && enumKey[charPosition].isUpper())
                partKey.append(QChar::fromLatin1('_'));
            partKey.append(enumKey[charPosition].toLower());
        }
        x->append(partKey);
    }
})

QStringList QS60Style::partKeys()
{
    return *enumPartKeys();
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QStringList, enumColorListKeys, {
    const int enumIndex = QS60StyleEnums::staticMetaObject.indexOfEnumerator("ColorLists");
    Q_ASSERT(enumIndex >= 0);
    const QMetaEnum metaEnum = QS60StyleEnums::staticMetaObject.enumerator(enumIndex);
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        const QString enumKey = QString::fromLatin1(metaEnum.key(i));
        // Following line does following conversions: CL_QsnTextColors to "text"...
        x->append(enumKey.mid(6, enumKey.length() - 12).toLower());
    }
})

QStringList QS60Style::colorListKeys()
{
    return *enumColorListKeys();
}

void QS60Style::setS60Theme(const QHash<QString, QPicture> &parts,
    const QHash<QPair<QString , int>, QColor> &colors)
{
    QS60StyleModeSpecifics::m_partPictures = parts;
    QS60StyleModeSpecifics::m_colors = colors;
}

QT_END_NAMESPACE

#endif // QT_NO_STYLE_S60 || QT_PLUGIN
