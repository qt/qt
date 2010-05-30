/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_TestAbstractXmlReceiver_h
#define Patternist_TestAbstractXmlReceiver_h

#include <QtXmlPatterns/QAbstractXmlReceiver>

class TestAbstractXmlReceiver : public QAbstractXmlReceiver
{
public:
    using QAbstractXmlReceiver::attribute;
    using QAbstractXmlReceiver::characters;

    virtual void startElement(const QXmlName&);
    virtual void endElement();
    virtual void attribute(const QXmlName&, const QStringRef&);
    virtual void comment(const QString&);
    virtual void characters(const QStringRef&);
    virtual void startDocument();
    virtual void endDocument();
    virtual void processingInstruction(const QXmlName&, const QString&);
    virtual void atomicValue(const QVariant&);
    virtual void namespaceBinding(const QXmlName&);
    virtual void startOfSequence();
    virtual void endOfSequence();

    QString receivedFromCharacters;
    QString receivedFromAttribute;
};

void TestAbstractXmlReceiver::startElement(const QXmlName &name)
{
    Q_UNUSED(name);
}

void TestAbstractXmlReceiver::endElement()
{
}

void TestAbstractXmlReceiver::attribute(const QXmlName &name, const QStringRef &value)
{
    Q_UNUSED(name);
    receivedFromAttribute = value.toString();
}

void TestAbstractXmlReceiver::comment(const QString &value)
{
    Q_UNUSED(value);
}

void TestAbstractXmlReceiver::characters(const QStringRef &value)
{
    receivedFromCharacters = value.toString();
}

void TestAbstractXmlReceiver::startDocument()
{
}

void TestAbstractXmlReceiver::endDocument()
{
}

void TestAbstractXmlReceiver::processingInstruction(const QXmlName &name, const QString &data)
{
    Q_UNUSED(name);
    Q_UNUSED(data);
}

void TestAbstractXmlReceiver::atomicValue(const QVariant &val)
{
    Q_UNUSED(val);
}

void TestAbstractXmlReceiver::namespaceBinding(const QXmlName &name)
{
    Q_UNUSED(name);
}

void TestAbstractXmlReceiver::startOfSequence()
{
}

void TestAbstractXmlReceiver::endOfSequence()
{
}

#endif
