/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLMODULEPLUGIN_H
#define QMLMODULEPLUGIN_H

#include <QtCore/qplugin.h>
#include <QtCore/qfactoryinterface.h>
#include <QtCore/qlist.h>
#include <QtCore/qset.h>
#include <QtCore/qbytearray.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

struct Q_DECLARATIVE_EXPORT QmlModuleFactoryInterface : public QFactoryInterface
{
    virtual void defineModuleOnce(const QString& uri) = 0;
};

#define QmlModuleFactoryInterface_iid "com.nokia.Qt.QmlModuleFactoryInterface"

Q_DECLARE_INTERFACE(QmlModuleFactoryInterface, QmlModuleFactoryInterface_iid)


class Q_DECLARATIVE_EXPORT QmlModulePlugin : public QObject, public QmlModuleFactoryInterface
{
    Q_OBJECT
    Q_INTERFACES(QmlModuleFactoryInterface:QFactoryInterface)
public:
    explicit QmlModulePlugin(QObject *parent = 0);
    ~QmlModulePlugin();

    virtual void defineModule(const QString& uri) = 0;

private:
    void defineModuleOnce(const QString& uri);
    QSet<QString> defined;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLMODULEPLUGIN_H
