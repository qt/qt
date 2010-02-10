/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLDOM_P_H
#define QMLDOM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmlparser_p.h"

#include <QtCore/QtGlobal>

QT_BEGIN_NAMESPACE

class QmlDomDocumentPrivate : public QSharedData
{
public:
    QmlDomDocumentPrivate();
    QmlDomDocumentPrivate(const QmlDomDocumentPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QmlDomDocumentPrivate();

    QList<QmlError> errors;
    QList<QmlDomImport> imports;
    QmlParser::Object *root;
    QList<int> automaticSemicolonOffsets;
};

class QmlDomObjectPrivate : public QSharedData
{
public:
    QmlDomObjectPrivate();
    QmlDomObjectPrivate(const QmlDomObjectPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QmlDomObjectPrivate();

    typedef QList<QPair<QmlParser::Property *, QByteArray> > Properties;
    Properties properties() const;
    Properties properties(QmlParser::Property *) const;

    QmlParser::Object *object;
};

class QmlDomPropertyPrivate : public QSharedData
{
public:
    QmlDomPropertyPrivate();
    QmlDomPropertyPrivate(const QmlDomPropertyPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QmlDomPropertyPrivate();

    QByteArray propertyName;
    QmlParser::Property *property;
};

class QmlDomDynamicPropertyPrivate : public QSharedData
{
public:
    QmlDomDynamicPropertyPrivate();
    QmlDomDynamicPropertyPrivate(const QmlDomDynamicPropertyPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QmlDomDynamicPropertyPrivate();

    bool valid;
    QmlParser::Object::DynamicProperty property;
};

class QmlDomValuePrivate : public QSharedData
{
public:
    QmlDomValuePrivate();
    QmlDomValuePrivate(const QmlDomValuePrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QmlDomValuePrivate();

    QmlParser::Property *property;
    QmlParser::Value *value;
};

class QmlDomBasicValuePrivate : public QSharedData
{
public:
    QmlDomBasicValuePrivate();
    QmlDomBasicValuePrivate(const QmlDomBasicValuePrivate &o) 
    : QSharedData(o) { qFatal("Not impl"); }
    ~QmlDomBasicValuePrivate();

    QmlParser::Value *value;
};

class QmlDomImportPrivate : public QSharedData
{
public:
    QmlDomImportPrivate();
    QmlDomImportPrivate(const QmlDomImportPrivate &o) 
    : QSharedData(o) { qFatal("Not impl"); }
    ~QmlDomImportPrivate();

    enum Type { Library, File };

    Type type;
    QString uri;
    QString version;
    QString qualifier;
};

QT_END_NAMESPACE

#endif // QMLDOM_P_H

