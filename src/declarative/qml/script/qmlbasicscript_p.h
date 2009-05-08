/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QMLBASICSCRIPT_P_H
#define QMLBASICSCRIPT_P_H

QT_BEGIN_NAMESPACE

class QObject;
class QmlContextPrivate;
class QDebug;
class QByteArray;

class QmlBasicScriptNodeCache
{
public:
    QObject *object;
    const QMetaObject *metaObject;
    enum { Invalid,
           Core, 
           Attached, 
           Signal,
           SignalProperty, 
           Explicit, 
           Variant, 
           ScriptValue } type;
    union {
        int core;
        QObject *attached;
        QmlContextPrivate *context;
    };
    int coreType;

    bool isValid() const { return type != Invalid; }
    bool isCore() const { return type == Core; }
    bool isExplicit() const { return type == Explicit; }
    void clear();
    QVariant value(const char *) const;
};

QDebug operator<<(QDebug, const QmlBasicScriptNodeCache &);

#endif // QMLBASICSCRIPT_P_H

QT_END_NAMESPACE
