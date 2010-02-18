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

#ifndef QMLCUSTOMPARSER_H
#define QMLCUSTOMPARSER_H

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

#include "qmlmetatype.h"
#include "qmlerror.h"
#include "qmlparser_p.h"

#include <QtCore/qbytearray.h>
#include <QtCore/qxmlstream.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlCustomParserPropertyPrivate;
class Q_DECLARATIVE_EXPORT QmlCustomParserProperty
{
public:
    QmlCustomParserProperty();
    QmlCustomParserProperty(const QmlCustomParserProperty &);
    QmlCustomParserProperty &operator=(const QmlCustomParserProperty &);
    ~QmlCustomParserProperty();

    QByteArray name() const;
    QmlParser::Location location() const;

    bool isList() const;
    // Will be one of QmlParser::Variant, QmlCustomParserProperty or 
    // QmlCustomParserNode
    QList<QVariant> assignedValues() const;

private:
    friend class QmlCustomParserNodePrivate;
    friend class QmlCustomParserPropertyPrivate;
    QmlCustomParserPropertyPrivate *d;
};

class QmlCustomParserNodePrivate;
class Q_DECLARATIVE_EXPORT QmlCustomParserNode
{
public:
    QmlCustomParserNode();
    QmlCustomParserNode(const QmlCustomParserNode &);
    QmlCustomParserNode &operator=(const QmlCustomParserNode &);
    ~QmlCustomParserNode();

    QByteArray name() const;
    QmlParser::Location location() const;

    QList<QmlCustomParserProperty> properties() const;

private:
    friend class QmlCustomParserNodePrivate;
    QmlCustomParserNodePrivate *d;
};

class Q_DECLARATIVE_EXPORT QmlCustomParser
{
public:
    virtual ~QmlCustomParser() {}

    void clearErrors();

    virtual QByteArray compile(const QList<QmlCustomParserProperty> &)=0;
    virtual void setCustomData(QObject *, const QByteArray &)=0;

    QList<QmlError> errors() const { return exceptions; }

protected:
    void error(const QmlCustomParserProperty&, const QString& description);
    void error(const QmlCustomParserNode&, const QString& description);

private:
    QList<QmlError> exceptions;
};

#if defined(Q_OS_SYMBIAN)
# define QML_DEFINE_CUSTOM_TYPE(URI, VERSION_MAJ, VERSION_MIN, NAME, TYPE, CUSTOMTYPE) \
    static int defineCustomType##NAME = qmlRegisterCustomType<TYPE>(#URI, VERSION_MAJ, VERSION_MIN, #NAME, #TYPE, new CUSTOMTYPE);
#else
# define QML_DEFINE_CUSTOM_TYPE(URI, VERSION_MAJ, VERSION_MIN, NAME, TYPE, CUSTOMTYPE) \
    template<> QmlPrivate::InstanceType QmlPrivate::Define<TYPE *,(VERSION_MAJ), (VERSION_MIN)>::instance(qmlRegisterCustomType<TYPE>(#URI, VERSION_MAJ, VERSION_MIN, #NAME, #TYPE, new CUSTOMTYPE));
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QmlCustomParserProperty)
Q_DECLARE_METATYPE(QmlCustomParserNode)

QT_END_HEADER

#endif
