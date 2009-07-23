/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLBINDING_H
#define QMLBINDING_H

#include <QtCore/QObject>
#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlpropertyvaluesource.h>
#include <QtDeclarative/qmlexpression.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlContext;
class QmlBindingPrivate;
class Q_DECLARATIVE_EXPORT QmlBinding : public QmlExpression
{
Q_OBJECT
public:
    QmlBinding(const QString &, QObject *, QmlContext *, QObject *parent=0);
    QmlBinding(void *, QmlRefCount *, QObject *, QmlContext *, QObject *parent);
    ~QmlBinding();

    void setTarget(const QmlMetaProperty &);
    QmlMetaProperty property() const;

    void init();
    void forceUpdate();

    void setEnabled(bool);
    bool enabled() const;

public Q_SLOTS:
    void update();

protected:
    virtual void valueChanged();

private:
    Q_DECLARE_PRIVATE(QmlBinding)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlBinding);

QT_END_HEADER

#endif // QMLBINDING_H
