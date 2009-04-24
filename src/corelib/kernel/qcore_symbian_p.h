/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#ifndef QCORE_SYMBIAN_P_H
#define QCORE_SYMBIAN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <e32std.h>
#include <QtCore/qglobal.h>
#include <qstring.h>
#include <qrect.h>
#include <qhash.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

Q_CORE_EXPORT HBufC* qt_QString2HBufCNewL(const QString& aString);

Q_CORE_EXPORT QString qt_TDesC2QStringL(const TDesC& aDescriptor);
inline QString qt_TDes2QStringL(const TDes& aDescriptor) { return qt_TDesC2QStringL(aDescriptor); }

static inline QSize qt_TSize2QSize(const TSize& ts)
{
    return QSize(ts.iWidth, ts.iHeight);
}

static inline TSize qt_QSize2TSize(const QSize& qs)
{
    return TSize(qs.width(), qs.height());
}

static inline QRect qt_TRect2QRect(const TRect& tr)
{
    return QRect(tr.iTl.iX, tr.iTl.iY, tr.Width(), tr.Height());
}

static inline TRect qt_QRect2TRect(const QRect& qr)
{
    return TRect(TPoint(qr.left(), qr.top()), TSize(qr.width(), qr.height()));
}

// Returned TPtrC is valid as long as the given parameter is valid and unmodified
static inline TPtrC qt_QString2TPtrC( const QString& string )
{
    return reinterpret_cast<const TUint16*>(string.utf16());
}

class Q_CORE_EXPORT QHBufC
{
public:
    QHBufC();
    QHBufC(const QHBufC &src);
    QHBufC(HBufC *src);
    QHBufC(const QString &src);
    ~QHBufC();

    inline operator HBufC *() { return m_hBufC; }
    inline operator const HBufC *() const { return m_hBufC; }
    inline HBufC *data() { return m_hBufC; }
    inline const HBufC *data() const { return m_hBufC; }
    inline HBufC & operator*() { return *m_hBufC; }
    inline const HBufC & operator*() const { return *m_hBufC; }
    inline HBufC * operator->() { return m_hBufC; }
    inline const HBufC * operator->() const { return m_hBufC; }

    inline bool operator==(const QHBufC &param) const { return data() == param.data(); }
    inline bool operator!=(const QHBufC &param) const { return data() != param.data(); }

private:
    HBufC *m_hBufC;
};

inline uint qHash(TUid uid)
{
    return qHash(uid.iUid);
}

// S60 version specific function ordinals that can be resolved
enum S60PluginFuncOrdinals
{
    S60Plugin_TimeFormatL            = 1,
    S60Plugin_GetTimeFormatSpec      = 2,
    S60Plugin_GetLongDateFormatSpec  = 3,
    S60Plugin_GetShortDateFormatSpec = 4,
    S60Plugin_LocalizedDirectoryName = 5
};

Q_CORE_EXPORT TLibraryFunction qt_resolveS60PluginFunc(int ordinal);

QT_END_NAMESPACE

QT_END_HEADER

#endif //QCORE_SYMBIAN_P_H
