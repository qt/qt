/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_PushBaseliner_h
#define Patternist_PushBaseliner_h

#include <QTextStream>
#include <QtXmlPatterns/QAbstractXmlReceiver>
#include <QtXmlPatterns/QXmlNamePool>
#include <QtXmlPatterns/QXmlNamePool>

class PushBaseliner : public QAbstractXmlReceiver
{
public:
    PushBaseliner(QTextStream &out,
                  const QXmlNamePool &namePool) : m_out(out)
                                                , m_namePool(namePool)
    {
        Q_ASSERT(m_out.codec());
    }

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

private:
    QTextStream &   m_out;
    const QXmlNamePool m_namePool;
};

void PushBaseliner::startElement(const QXmlName &name)
{
    m_out << "startElement(" << name.toClarkName(m_namePool) << ')'<< endl;
}

void PushBaseliner::endElement()
{
    m_out << "endElement()" << endl;
}

void PushBaseliner::attribute(const QXmlName &name, const QStringRef &value)
{
    m_out << "attribute(" << name.toClarkName(m_namePool) << ", " << value.toString() << ')'<< endl;
}

void PushBaseliner::comment(const QString &value)
{
    m_out << "comment(" << value << ')' << endl;
}

void PushBaseliner::characters(const QStringRef &value)
{
    m_out << "characters(" << value.toString() << ')' << endl;
}

void PushBaseliner::startDocument()
{
    m_out << "startDocument()" << endl;
}

void PushBaseliner::endDocument()
{
    m_out << "endDocument()" << endl;
}

void PushBaseliner::processingInstruction(const QXmlName &name, const QString &data)
{
    m_out << "processingInstruction(" << name.toClarkName(m_namePool) << ", " << data << ')' << endl;
}

void PushBaseliner::atomicValue(const QVariant &val)
{
    m_out << "atomicValue(" << val.toString() << ')' << endl;
}

void PushBaseliner::namespaceBinding(const QXmlName &name)
{
    m_out << "namespaceBinding(" << name.toClarkName(m_namePool) << ')' << endl;
}

void PushBaseliner::startOfSequence()
{
    m_out << "startOfSequence()" << endl;
}

void PushBaseliner::endOfSequence()
{
    m_out << "endOfSequence()" << endl;
}

#endif
