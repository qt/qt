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

#include "qmlerror.h"

#include <QtCore/qlist.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QString;
class QByteArray;
class QmlDomObject;
class QmlDomList;
class QmlDomValue;
class QmlEngine;
class QmlDomComponent;
class QmlDomImport;
class QIODevice;

class QmlDomDocumentPrivate;

class Q_DECLARATIVE_EXPORT QmlDomDocument
{
public:
    QmlDomDocument();
    QmlDomDocument(const QmlDomDocument &);
    ~QmlDomDocument();
    QmlDomDocument &operator=(const QmlDomDocument &);

    QList<QmlDomImport> imports() const;

    QList<QmlError> errors() const;
    bool load(QmlEngine *, const QByteArray &, const QUrl & = QUrl());

    QmlDomObject rootObject() const;

private:
    QSharedDataPointer<QmlDomDocumentPrivate> d;
};

class QmlDomPropertyPrivate;
class Q_DECLARATIVE_EXPORT QmlDomProperty
{
public:
    QmlDomProperty();
    QmlDomProperty(const QmlDomProperty &);
    ~QmlDomProperty();
    QmlDomProperty &operator=(const QmlDomProperty &);

    bool isValid() const;

    QByteArray propertyName() const;
    QList<QByteArray> propertyNameParts() const;

    bool isDefaultProperty() const;

    QmlDomValue value() const;

    int position() const;
    int length() const;

private:
    friend class QmlDomObject;
    friend class QmlDomDynamicProperty;
    QSharedDataPointer<QmlDomPropertyPrivate> d;
};

class QmlDomDynamicPropertyPrivate;
class Q_DECLARATIVE_EXPORT QmlDomDynamicProperty
{
public:
    QmlDomDynamicProperty();
    QmlDomDynamicProperty(const QmlDomDynamicProperty &);
    ~QmlDomDynamicProperty();
    QmlDomDynamicProperty &operator=(const QmlDomDynamicProperty &);

    bool isValid() const;

    QByteArray propertyName() const;
    int propertyType() const;
    QByteArray propertyTypeName() const;

    bool isDefaultProperty() const;
    QmlDomProperty defaultValue() const;

    bool isAlias() const;

    int position() const;
    int length() const;

private:
    friend class QmlDomObject;
    QSharedDataPointer<QmlDomDynamicPropertyPrivate> d;
};

class QmlDomObjectPrivate;
class Q_DECLARATIVE_EXPORT QmlDomObject
{
public:
    QmlDomObject();
    QmlDomObject(const QmlDomObject &);
    ~QmlDomObject();
    QmlDomObject &operator=(const QmlDomObject &);

    bool isValid() const;

    QByteArray objectType() const;
    QByteArray objectClassName() const;

    int objectTypeMajorVersion() const;
    int objectTypeMinorVersion() const;

    QString objectId() const;

    QList<QmlDomProperty> properties() const;
    QmlDomProperty property(const QByteArray &) const;

    QList<QmlDomDynamicProperty> dynamicProperties() const;
    QmlDomDynamicProperty dynamicProperty(const QByteArray &) const;

    bool isCustomType() const;
    QByteArray customTypeData() const;

    bool isComponent() const;
    QmlDomComponent toComponent() const;

    int position() const;
    int length() const;

    QUrl url() const;
private:
    friend class QmlDomDocument;
    friend class QmlDomComponent;
    friend class QmlDomValue;
    friend class QmlDomValueValueSource;
    friend class QmlDomValueValueInterceptor;
    QSharedDataPointer<QmlDomObjectPrivate> d;
};

class QmlDomValuePrivate;
class QmlDomBasicValuePrivate;
class Q_DECLARATIVE_EXPORT QmlDomValueLiteral
{
public:
    QmlDomValueLiteral();
    QmlDomValueLiteral(const QmlDomValueLiteral &);
    ~QmlDomValueLiteral();
    QmlDomValueLiteral &operator=(const QmlDomValueLiteral &);

    QString literal() const;

private:
    friend class QmlDomValue;
    QSharedDataPointer<QmlDomBasicValuePrivate> d;
};

class Q_DECLARATIVE_EXPORT QmlDomValueBinding
{
public:
    QmlDomValueBinding();
    QmlDomValueBinding(const QmlDomValueBinding &);
    ~QmlDomValueBinding();
    QmlDomValueBinding &operator=(const QmlDomValueBinding &);

    QString binding() const;

private:
    friend class QmlDomValue;
    QSharedDataPointer<QmlDomBasicValuePrivate> d;
};

class Q_DECLARATIVE_EXPORT QmlDomValueValueSource
{
public:
    QmlDomValueValueSource();
    QmlDomValueValueSource(const QmlDomValueValueSource &);
    ~QmlDomValueValueSource();
    QmlDomValueValueSource &operator=(const QmlDomValueValueSource &);

    QmlDomObject object() const;

private:
    friend class QmlDomValue;
    QSharedDataPointer<QmlDomBasicValuePrivate> d;
};

class Q_DECLARATIVE_EXPORT QmlDomValueValueInterceptor
{
public:
    QmlDomValueValueInterceptor();
    QmlDomValueValueInterceptor(const QmlDomValueValueInterceptor &);
    ~QmlDomValueValueInterceptor();
    QmlDomValueValueInterceptor &operator=(const QmlDomValueValueInterceptor &);

    QmlDomObject object() const;

private:
    friend class QmlDomValue;
    QSharedDataPointer<QmlDomBasicValuePrivate> d;
};


class Q_DECLARATIVE_EXPORT QmlDomComponent : public QmlDomObject
{
public:
    QmlDomComponent();
    QmlDomComponent(const QmlDomComponent &);
    ~QmlDomComponent();
    QmlDomComponent &operator=(const QmlDomComponent &);

    QmlDomObject componentRoot() const;
};

class Q_DECLARATIVE_EXPORT QmlDomValue
{
public:
    enum Type { 
        Invalid,
        Literal, 
        PropertyBinding, 
        ValueSource,
        ValueInterceptor,
        Object,
        List 
    };

    QmlDomValue();
    QmlDomValue(const QmlDomValue &);
    ~QmlDomValue();
    QmlDomValue &operator=(const QmlDomValue &);

    Type type() const;

    bool isInvalid() const;
    bool isLiteral() const;
    bool isBinding() const;
    bool isValueSource() const;
    bool isValueInterceptor() const;
    bool isObject() const;
    bool isList() const;

    QmlDomValueLiteral toLiteral() const;
    QmlDomValueBinding toBinding() const;
    QmlDomValueValueSource toValueSource() const;
    QmlDomValueValueInterceptor toValueInterceptor() const;
    QmlDomObject toObject() const;
    QmlDomList toList() const;

    int position() const;
    int length() const;

private:
    friend class QmlDomProperty;
    friend class QmlDomList;
    QSharedDataPointer<QmlDomValuePrivate> d;
};

class Q_DECLARATIVE_EXPORT QmlDomList
{
public:
    QmlDomList();
    QmlDomList(const QmlDomList &);
    ~QmlDomList();
    QmlDomList &operator=(const QmlDomList &);

    QList<QmlDomValue> values() const;

    int position() const;
    int length() const;

    QList<int> commaPositions() const;

private:
    friend class QmlDomValue;
    QSharedDataPointer<QmlDomValuePrivate> d;
};

class QmlDomImportPrivate;
class Q_DECLARATIVE_EXPORT QmlDomImport
{
public:
    enum Type { Library, File };

    QmlDomImport();
    QmlDomImport(const QmlDomImport &);
    ~QmlDomImport();
    QmlDomImport &operator=(const QmlDomImport &);

    Type type() const;
    QString uri() const;
    QString version() const;
    QString qualifier() const;

private:
    friend class QmlDomDocument;
    QSharedDataPointer<QmlDomImportPrivate> d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLDOM_P_H
