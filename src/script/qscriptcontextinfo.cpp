/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptcontextinfo.h"

#include "qscriptengine.h"
#include <QtCore/qdatastream.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.4
  \class QScriptContextInfo

  \brief The QScriptContextInfo class provides additional information about a QScriptContext.

  \ingroup script


  QScriptContextInfo is typically used for debugging purposes. It can
  provide information about the code being executed, such as the type
  of the called function, and the original source code location of the
  current statement.

  If the called function is executing Qt Script code, you can obtain
  the script location with the functions fileName() and lineNumber().

  You can obtain the starting line number and ending line number of a
  Qt Script function definition with functionStartLineNumber() and
  functionEndLineNumber(), respectively.

  For Qt Script functions and Qt methods (e.g. slots), you can call
  functionParameterNames() to get the names of the formal parameters of the
  function.

  For Qt methods and Qt property accessors, you can obtain the index
  of the underlying QMetaMethod or QMetaProperty by calling
  functionMetaIndex().

  \sa QScriptContext, QScriptEngineAgent
*/

/*!
    \enum QScriptContextInfo::FunctionType

    This enum specifies the type of function being called.

    \value ScriptFunction The function is a Qt Script function, i.e. it was defined through a call to QScriptEngine::evaluate().
    \value QtFunction The function is a Qt function (a signal, slot or method).
    \value QtPropertyFunction The function is a Qt property getter or setter.
    \value NativeFunction The function is a built-in Qt Script function, or it was defined through a call to QScriptEngine::newFunction().
*/

class QScriptContextInfoPrivate
{
    Q_DECLARE_PUBLIC(QScriptContextInfo)
public:
    QScriptContextInfoPrivate();
    QScriptContextInfoPrivate(const QScriptContext *context);
    ~QScriptContextInfoPrivate();

    qint64 scriptId;
    int lineNumber;
    int columnNumber;
    QString fileName;

    QString functionName;
    QScriptContextInfo::FunctionType functionType;

    int functionStartLineNumber;
    int functionEndLineNumber;
    int functionMetaIndex;

    QStringList parameterNames;

    QBasicAtomicInt ref;

    QScriptContextInfo *q_ptr;
};

/*!
  \internal
*/
QScriptContextInfoPrivate::QScriptContextInfoPrivate()
{
    ref = 0;
    functionType = QScriptContextInfo::NativeFunction;
    functionMetaIndex = -1;
    functionStartLineNumber = -1;
    functionEndLineNumber = -1;
    scriptId = -1;
    lineNumber = -1;
    columnNumber = -1;
}

/*!
  \internal
*/
QScriptContextInfoPrivate::QScriptContextInfoPrivate(const QScriptContext *context)
{
    Q_ASSERT(context);
    ref = 0;
    functionType = QScriptContextInfo::NativeFunction;
    functionMetaIndex = -1;
    functionStartLineNumber = -1;
    functionEndLineNumber = -1;
    scriptId = -1;
    lineNumber = -1;
    columnNumber = -1;

    Q_UNIMPLEMENTED();
}

/*!
  \internal
*/
QScriptContextInfoPrivate::~QScriptContextInfoPrivate()
{
}

/*!
  Constructs a new QScriptContextInfo from the given \a context.

  The relevant information is extracted from the \a context at
  construction time; i.e. if you continue script execution in the \a
  context, the new state of the context will not be reflected in a
  previously created QScriptContextInfo.
*/
QScriptContextInfo::QScriptContextInfo(const QScriptContext *context)
{
    Q_UNUSED(context);
}

/*!
  Constructs a new QScriptContextInfo from the \a other info.
*/
QScriptContextInfo::QScriptContextInfo(const QScriptContextInfo &other)
{
    Q_UNUSED(other);
}

/*!
  Constructs a null QScriptContextInfo.

  \sa isNull()
*/
QScriptContextInfo::QScriptContextInfo()
{
}

/*!
  Destroys the QScriptContextInfo.
*/
QScriptContextInfo::~QScriptContextInfo()
{
}

/*!
  Assigns the \a other info to this QScriptContextInfo,
  and returns a reference to this QScriptContextInfo.
*/
QScriptContextInfo &QScriptContextInfo::operator=(const QScriptContextInfo &other)
{
    Q_UNUSED(other);
    Q_UNIMPLEMENTED();
    return *this;
}

/*!
  Returns the ID of the script where the code being executed was
  defined, or -1 if the ID is not available (i.e. a native function is
  being executed).

  \sa QScriptEngineAgent::scriptLoad()
*/
qint64 QScriptContextInfo::scriptId() const
{
    Q_UNIMPLEMENTED();
    return 0;
}

/*!
  Returns the name of the file where the code being executed was
  defined, if available; otherwise returns an empty string.

  For Qt Script code, this function returns the fileName argument
  that was passed to QScriptEngine::evaluate().

  \sa lineNumber(), functionName()
*/
QString QScriptContextInfo::fileName() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

/*!
  Returns the line number corresponding to the statement being
  executed, or -1 if the line number is not available.

  The line number is only available if Qt Script code is being
  executed.

  \sa columnNumber(), fileName()
*/
int QScriptContextInfo::lineNumber() const
{
    Q_UNIMPLEMENTED();
    return -1;
}

/*!
  \obsolete
*/
int QScriptContextInfo::columnNumber() const
{
    Q_UNIMPLEMENTED();
    return -1;
}

/*!
  Returns the name of the called function, or an empty string if
  the name is not available.

  For script functions of type QtPropertyFunction, this function
  always returns the name of the property; you can use
  QScriptContext::argumentCount() to differentiate between reads and
  writes.

  \sa fileName(), functionType()
*/
QString QScriptContextInfo::functionName() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

/*!
  Returns the type of the called function.

  \sa functionName(), QScriptContext::callee()
*/
QScriptContextInfo::FunctionType QScriptContextInfo::functionType() const
{
    Q_UNIMPLEMENTED();
    return NativeFunction;
}

/*!
  Returns the line number where the definition of the called function
  starts, or -1 if the line number is not available.

  The starting line number is only available if the functionType() is
  ScriptFunction.

  \sa functionEndLineNumber(), fileName()
*/
int QScriptContextInfo::functionStartLineNumber() const
{
    Q_UNIMPLEMENTED();
    return -1;
}

/*!
  Returns the line number where the definition of the called function
  ends, or -1 if the line number is not available.

  The ending line number is only available if the functionType() is
  ScriptFunction.

  \sa functionStartLineNumber()
*/
int QScriptContextInfo::functionEndLineNumber() const
{
    Q_UNIMPLEMENTED();
    return -1;
}

/*!
  Returns the names of the formal parameters of the called function,
  or an empty QStringList if the parameter names are not available.

  \sa QScriptContext::argument()
*/
QStringList QScriptContextInfo::functionParameterNames() const
{
    Q_UNIMPLEMENTED();
    return QStringList();
}

/*!
  Returns the meta index of the called function, or -1 if the meta
  index is not available.

  The meta index is only available if the functionType() is QtFunction
  or QtPropertyFunction. For QtFunction, the meta index can be passed
  to QMetaObject::method() to obtain the corresponding method
  definition; for QtPropertyFunction, the meta index can be passed to
  QMetaObject::property() to obtain the corresponding property
  definition.

  \sa QScriptContext::thisObject()
*/
int QScriptContextInfo::functionMetaIndex() const
{
    Q_UNIMPLEMENTED();
    return -1;
}

/*!
  Returns true if this QScriptContextInfo is null, i.e. does not
  contain any information.
*/
bool QScriptContextInfo::isNull() const
{
    Q_UNIMPLEMENTED();
    return false;
}

/*!
  Returns true if this QScriptContextInfo is equal to the \a other
  info, otherwise returns false.
*/
bool QScriptContextInfo::operator==(const QScriptContextInfo &other) const
{
    Q_UNUSED(other);
    Q_UNIMPLEMENTED();
    return false;
}

/*!
  Returns true if this QScriptContextInfo is not equal to the \a other
  info, otherwise returns false.
*/
bool QScriptContextInfo::operator!=(const QScriptContextInfo &other) const
{
    // FIXME it could be inlined.
    return !(*this == other);
}

#ifndef QT_NO_DATASTREAM
/*!
  \fn QDataStream &operator<<(QDataStream &stream, const QScriptContextInfo &info)
  \since 4.4
  \relates QScriptContextInfo

  Writes the given \a info to the specified \a stream.
*/
QDataStream &operator<<(QDataStream &out, const QScriptContextInfo &info)
{
    out << info.scriptId();
    out << (qint32)info.lineNumber();
    out << (qint32)info.columnNumber();

    out << (quint32)info.functionType();
    out << (qint32)info.functionStartLineNumber();
    out << (qint32)info.functionEndLineNumber();
    out << (qint32)info.functionMetaIndex();

    out << info.fileName();
    out << info.functionName();
    out << info.functionParameterNames();

    return out;
}

/*!
  \fn QDataStream &operator>>(QDataStream &stream, QScriptContextInfo &info)
  \since 4.4
  \relates QScriptContextInfo

  Reads a QScriptContextInfo from the specified \a stream into the
  given \a info.
*/
Q_SCRIPT_EXPORT QDataStream &operator>>(QDataStream &in, QScriptContextInfo &info)
{
    Q_UNUSED(info);
    Q_UNIMPLEMENTED();
    /*
    if (!info.d_ptr) {
        info.d_ptr = new QScriptContextInfoPrivate();
    }

    in >> info.d_ptr->scriptId;

    qint32 line;
    in >> line;
    info.d_ptr->lineNumber = line;

    qint32 column;
    in >> column;
    info.d_ptr->columnNumber = column;

    quint32 ftype;
    in >> ftype;
    info.d_ptr->functionType = QScriptContextInfo::FunctionType(ftype);

    qint32 startLine;
    in >> startLine;
    info.d_ptr->functionStartLineNumber = startLine;

    qint32 endLine;
    in >> endLine;
    info.d_ptr->functionEndLineNumber = endLine;

    qint32 metaIndex;
    in >> metaIndex;
    info.d_ptr->functionMetaIndex = metaIndex;

    in >> info.d_ptr->fileName;
    in >> info.d_ptr->functionName;
    in >> info.d_ptr->parameterNames;

    */
    return in;
}
#endif

QT_END_NAMESPACE
