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

#include "qmlxmlparser_p.h"
#include "qmlcustomparser.h"
#include <qfxperf.h>
#include <QXmlStreamReader>
#include <QStack>
#include "qmlparser_p.h"
#include <private/qmlparser_p.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE
using namespace QmlParser;

struct QmlXmlParserState {
    QmlXmlParserState() : object(0), property(0) {}
    QmlXmlParserState(Object *o) : object(o), property(0) {}
    QmlXmlParserState(Object *o, Property *p) : object(o), property(p) {}

    Object *object;
    Property *property;
};

struct QmlXmlParserStateStack : public QStack<QmlXmlParserState>
{
    void pushObject(Object *obj)
    {
        push(QmlXmlParserState(obj));
    }

    void pushProperty(const QString &name, int lineNumber)
    {
        const QmlXmlParserState &state = top();
        if(state.property) {
            QmlXmlParserState s(state.property->getValue(), 
                                state.property->getValue()->getProperty(name.toLatin1()));
            s.property->line = lineNumber;
            push(s);
        } else {
            QmlXmlParserState s(state.object, 
                                state.object->getProperty(name.toLatin1()));
            s.property->line = lineNumber;
            push(s);
        }
    }
};

QmlXmlParser::~QmlXmlParser()
{
    if(root)
        root->release();
}

QmlXmlParser::QmlXmlParser()
: root(0)
{
}

static QString flatXml(QXmlStreamReader& reader)
{
    QString result;
    int depth=0;
    QStringRef ns = reader.namespaceUri();
    while (depth>=0) {
        switch (reader.tokenType()) {
          case QXmlStreamReader::StartElement:
            result += QLatin1Char('<');
            result += reader.name();
            if (reader.namespaceUri() != ns || depth==0) {
                result += QLatin1String(" xmlns=\"");
                result += reader.namespaceUri();
                result += QLatin1Char('"');
            }
            foreach(QXmlStreamAttribute attr, reader.attributes()) {
                result += QLatin1Char(' ');
                result += attr.name();
                result += QLatin1String("=\"");
                result += attr.value(); // XXX escape
                result += QLatin1Char('"');
            }
            result += QLatin1Char('>');
            ++depth;
            break;
          case QXmlStreamReader::EndElement:
            result += QLatin1String("</");
            result += reader.name();
            result += QLatin1Char('>');
            --depth;
            break;
          case QXmlStreamReader::Characters:
            result += reader.text();
            break;
          default:
            reader.raiseError(QLatin1String("Only StartElement, EndElement, and Characters permitted"));
            break;
        }
        if (depth>=0)
            reader.readNext();
    }
    return result;
}

bool QmlXmlParser::parse(const QByteArray &data, const QUrl &url)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::XmlParsing> pt;
#endif

    QString fileDisplayName;
    if (url.isEmpty()) {
        fileDisplayName = QLatin1String("<unspecified file>");
    } else if (url.scheme() == QLatin1String("file")) {
        fileDisplayName = url.toLocalFile();
    } else {
        fileDisplayName = url.toString();
    }
    if (data.isEmpty()) {
        _error = QLatin1String("No Qml was specified for parsing @") + fileDisplayName;
        return false;
    }

    QmlXmlParserStateStack states;

    QXmlStreamReader reader;
    reader.addData(data);

    while(!reader.atEnd()) {
        switch(reader.readNext()) {
            case QXmlStreamReader::Invalid:
            case QXmlStreamReader::NoToken:
            case QXmlStreamReader::StartDocument:
            case QXmlStreamReader::EndDocument:
                break;

            case QXmlStreamReader::StartElement:
                {
                    QString name = reader.name().toString();
                    QString nameSpace = reader.namespaceUri().toString();
                    int line = reader.lineNumber();
                    bool isType = name.at(0).isUpper() && !name.contains(QLatin1Char('.'));
                    QString qualifiedname;
                    if (!nameSpace.isEmpty()) {
                        qualifiedname = nameSpace;
                        qualifiedname += QLatin1Char('/');
                    }
                    qualifiedname += name;
                    QByteArray qualifiednameL1 = qualifiedname.toLatin1();
                    QXmlStreamAttributes attrs = reader.attributes();

                    if (isType) {
                        // Class
                        int typeId = _typeNames.indexOf(qualifiedname);
                        if(typeId == -1) {
                            typeId = _typeNames.count();
                            _typeNames.append(qualifiedname);
                        }

                        Object *obj = new Object;
                        obj->type = typeId;
                        obj->typeName = qualifiednameL1;
                        obj->line = line;

                        QmlCustomParser *customparser = QmlMetaType::customParser(qualifiednameL1);
                        if (customparser) {
                            bool ok;
                            obj->custom = customparser->compile(reader, &ok);
                            if (reader.tokenType() != QXmlStreamReader::EndElement) {
                                reader.raiseError(QLatin1String("Parser for ") + qualifiedname + QLatin1String(" did not end on end element"));
                                ok = false;
                            }
                            if (!ok) {
                                delete obj;
                                break;
                            }
                        }


                        if(!root) { 
                            root = obj;
                            states.pushObject(obj);
                        } else {
                            const QmlXmlParserState &state = states.top();
                            Value *v = new Value;
                            v->object = obj;
                            v->line = line;
                            if(state.property)
                                state.property->addValue(v);
                            else
                                state.object->getDefaultProperty()->addValue(v);
                            states.pushObject(obj);
                        }
                    } else {
                        // Property
                        if (!root) {
                            reader.raiseError(QLatin1String("Can't have a property with no object"));
                            break;
                        }
                        QStringList str = name.split(QLatin1Char('.'));
                        for(int ii = 0; ii < str.count(); ++ii) {
                            QString s = str.at(ii);
                            states.pushProperty(s, line);
                        }
                        if (!nameSpace.isEmpty()) {
                            // Pass non-QML as flat text property value
                            const QmlXmlParserState &state = states.top();
                            Value *v = new Value;
                            v->primitive = flatXml(reader);
                            v->line = line;
                            state.property->addValue(v);
                        }
                    }

                    // (even custom parsed content gets properties set)
                    foreach(QXmlStreamAttribute attr, attrs) {
                        QStringList str = attr.name().toString().split(QLatin1Char('.'));

                        for(int ii = 0; ii < str.count(); ++ii) {
                            QString s = str.at(ii);
                            states.pushProperty(s, line);
                        }

                        const QmlXmlParserState &state = states.top();
                        Value *v = new Value;
                        v->primitive = attr.value().toString();
                        v->line = reader.lineNumber();
                        state.property->addValue(v);

                        for(int ii = str.count() - 1; ii >= 0; --ii) 
                            states.pop();
                    }
                }

                // Custom parsers and namespaced properties move
                // the reader to the end element, so we handle that
                // BEFORE continuing.
                //
                if (reader.tokenType()!=QXmlStreamReader::EndElement)
                    break;
                // ELSE fallthrough to EndElement...
            case QXmlStreamReader::EndElement:
                {
                    QString name = reader.name().toString();
                    Q_ASSERT(!name.isEmpty());
                    if(name.at(0).isUpper() && !name.contains(QLatin1Char('.'))) {
                        // Class
                        states.pop();
                    } else {
                        // Property
                        QStringList str = name.split(QLatin1Char('.'));
                        for(int ii = 0; ii < str.count(); ++ii) 
                            states.pop();
                    }
                }
                break;
            case QXmlStreamReader::Characters:
                if(!reader.isWhitespace()) {
                    const QmlXmlParserState &state = states.top();
                    Value *v = new Value;
                    v->primitive = reader.text().toString();
                    v->line = reader.lineNumber();
                    if(state.property)
                        state.property->addValue(v);
                    else
                        state.object->getDefaultProperty()->addValue(v);
                }
                break;

            case QXmlStreamReader::Comment:
            case QXmlStreamReader::DTD:
            case QXmlStreamReader::EntityReference:
                break;
            case QXmlStreamReader::ProcessingInstruction:
                if(reader.processingInstructionTarget() == QLatin1String("qtfx")) {
                    QString str = reader.processingInstructionData().toString();
                    QString token, data;
                    int idx = str.indexOf(QLatin1Char(':'));
                    if(-1 != idx) {
                        token = str.left(idx);
                        data = str.mid(idx + 1);
                    } else {
                        token = str;
                    }
                    token = token.trimmed();
                    data = data.trimmed();

                    // <?qtfx namespacepath: namespace=path>

                    if(token == QLatin1String("namespacepath")) {
                        int eq=data.indexOf(QLatin1Char('='));
                        if (eq>=0) {
                            _nameSpacePaths.insertMulti(data.left(eq),data.mid(eq+1));
                        }
                    } else {
                        str = str.trimmed();
                        qWarning().nospace() << "Unknown processing instruction " << str.toLatin1().constData() << " @" << fileDisplayName.toLatin1().constData()  << ":" << reader.lineNumber();
                    }
                }
                break;
        }
    }

    if(reader.hasError()) {
        if (root) {
            root->release();
            root = 0;
        }
        _error = reader.errorString() + QLatin1String(" @") + fileDisplayName +
                 QLatin1String(":") + QString::number(reader.lineNumber());
    }

    return root != 0;
}

QMap<QString,QString> QmlXmlParser::nameSpacePaths() const
{
    return _nameSpacePaths;
}

QStringList QmlXmlParser::types() const
{
    return _typeNames;
}

QmlParser::Object *QmlXmlParser::tree() const
{
    return root;
}

QmlParser::Object *QmlXmlParser::takeTree()
{
    QmlParser::Object *r = root;
    root = 0;
    return r;
}

QString QmlXmlParser::errorDescription() const
{
    return _error;
}

void QmlXmlParser::clear()
{
    if(root) {
        root->release();
        root = 0;
    }
    _nameSpacePaths.clear();
    _typeNames.clear();
    _error.clear();
}

QT_END_NAMESPACE
