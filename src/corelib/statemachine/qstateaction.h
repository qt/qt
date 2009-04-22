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

#ifndef QSTATEACTION_H
#define QSTATEACTION_H

#include <QtCore/qobject.h>

#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QStateActionPrivate;
class Q_CORE_EXPORT QStateAction : public QObject
{
    Q_OBJECT
public:
    ~QStateAction();

protected:
    QStateAction(QObject *parent = 0);

    virtual void execute() = 0;

    bool event(QEvent *e);

protected:
#ifdef QT_STATEMACHINE_SOLUTION
    QStateActionPrivate *d_ptr;
#endif
    QStateAction(QStateActionPrivate &dd, QObject *parent);

private:
    Q_DISABLE_COPY(QStateAction)
    Q_DECLARE_PRIVATE(QStateAction)
};

class QStateSetPropertyActionPrivate;
class Q_CORE_EXPORT QStateSetPropertyAction : public QStateAction
{
    Q_OBJECT
    Q_PROPERTY(QObject* target READ targetObject WRITE setTargetObject)
    Q_PROPERTY(QByteArray propertyName READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    QStateSetPropertyAction(QObject *target, const QByteArray &propertyName,
                            const QVariant &value, QObject *parent = 0);
    QStateSetPropertyAction(QObject *parent = 0);
    ~QStateSetPropertyAction();

    QObject *targetObject() const;
    void setTargetObject(QObject *target);

    QByteArray propertyName() const;
    void setPropertyName(const QByteArray &name);

    QVariant value() const;
    void setValue(const QVariant &value);

protected:
    void execute();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QStateSetPropertyAction)
    Q_DECLARE_PRIVATE(QStateSetPropertyAction)
};

class QStateInvokeMethodActionPrivate;
class Q_CORE_EXPORT QStateInvokeMethodAction : public QStateAction
{
    Q_OBJECT
    Q_PROPERTY(QObject* target READ targetObject WRITE setTargetObject)
    Q_PROPERTY(QByteArray methodName READ methodName WRITE setMethodName)
    Q_PROPERTY(QVariantList arguments READ arguments WRITE setArguments)
public:
    QStateInvokeMethodAction(QObject *target, const QByteArray &methodName,
                             QObject *parent = 0);
    QStateInvokeMethodAction(QObject *target, const QByteArray &methodName,
                             const QList<QVariant> &args, QObject *parent = 0);
    QStateInvokeMethodAction(QObject *parent = 0);
    ~QStateInvokeMethodAction();

    QObject *targetObject() const;
    void setTargetObject(QObject *target);

    QByteArray methodName() const;
    void setMethodName(const QByteArray &methodName);

    QVariantList arguments() const;
    void setArguments(const QVariantList &arguments);

protected:
    void execute();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QStateInvokeMethodAction)
    Q_DECLARE_PRIVATE(QStateInvokeMethodAction)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
