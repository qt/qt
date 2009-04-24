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

#include <e32base.h>
#include <e32uid.h>
#include "qcore_symbian_p.h"

QT_BEGIN_NAMESPACE

/*
    Helper function for calling into Symbian classes that expect a TDes&.
    This function converts a QString to a TDes by allocating memory that
    must be deleted by the caller.
*/

Q_CORE_EXPORT HBufC* qt_QString2HBufCNewL(const QString& aString)
{
    HBufC *buffer;
#ifdef QT_NO_UNICODE
    TPtrC8 ptr(reinterpret_cast<const TUint8*>(aString.toLocal8Bit().constData()));
    buffer = HBufC8::NewL(ptr.Length());
    buffer->Des().Copy(ptr);
#else
    TPtrC16 ptr(reinterpret_cast<const TUint16*>(aString.utf16()));
    buffer = HBufC16::NewL(ptr.Length());
    buffer->Des().Copy(ptr);
#endif
    return buffer;
}

Q_CORE_EXPORT QString qt_TDesC2QStringL(const TDesC& aDescriptor)
{
#ifdef QT_NO_UNICODE
    return QString::fromLocal8Bit(aDescriptor.Ptr(), aDescriptor.Length());
#else
    return QString::fromUtf16(aDescriptor.Ptr(), aDescriptor.Length());
#endif
}

QHBufC::QHBufC()
    : m_hBufC(0)
{
}

QHBufC::QHBufC(const QHBufC &src)
{
    m_hBufC = src.m_hBufC->AllocL();
}

/*!
  \internal
  Constructs a QHBufC from an HBufC. Note that the QHBufC instance takes
  ownership of the HBufC.
*/
QHBufC::QHBufC(HBufC *src)
    : m_hBufC(src)
{
}

QHBufC::QHBufC(const QString &src)
{
    m_hBufC = qt_QString2HBufCNewL(src);
}

QHBufC::~QHBufC()
{
    if (m_hBufC)
        delete m_hBufC;
}

class QS60PluginResolver
{
public:
    QS60PluginResolver()
        : initTried(false) {}

    ~QS60PluginResolver() {
        lib.Close();
    }

    TLibraryFunction resolve(int ordinal) {
        if (!initTried) {
            init();
            initTried = true;
        }

        if (lib.Handle())
            return lib.Lookup(ordinal);
        else
            return reinterpret_cast<TLibraryFunction>(NULL);
    }

private:
    void init() {
        _LIT(KLibName_3_1, "qts60plugin_3_1.dll");
        _LIT(KLibName_3_2, "qts60plugin_3_2.dll");
        _LIT(KLibName_5_0, "qts60plugin_5_0.dll");
        TPtrC libName;
        TInt uidValue;
        switch (QSysInfo::s60Version()) {
        case QSysInfo::SV_S60_3_1:
            libName.Set(KLibName_3_1);
            uidValue = 0x2001E620;
            break;
        case QSysInfo::SV_S60_3_2:
            libName.Set(KLibName_3_2);
            uidValue = 0x2001E621;
            break;
        case QSysInfo::SV_S60_5_0: // Fall through to default
        default:
            // Default to 5.0 version, as any unknown platform is likely to be newer than that
            libName.Set(KLibName_5_0);
            uidValue = 0x2001E622;
            break;
        }

        TUidType libUid(KDynamicLibraryUid, KSharedLibraryUid, TUid::Uid(uidValue));
        lib.Load(libName, libUid);
    }

    RLibrary lib;
    bool initTried;
};

Q_GLOBAL_STATIC(QS60PluginResolver, qt_s60_plugin_resolver);

/*!
  \internal
  Resolves a platform version specific function from S60 plugin.
  If plugin is missing or resolving fails for another reason, NULL is returned.
*/
Q_CORE_EXPORT TLibraryFunction qt_resolveS60PluginFunc(int ordinal)
{
    return qt_s60_plugin_resolver()->resolve(ordinal);
}

QT_END_NAMESPACE
