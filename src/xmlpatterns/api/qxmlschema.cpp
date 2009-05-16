/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qxmlschema.h"
#include "qxmlschema_p.h"

#include <QtCore/QIODevice>
#include <QtCore/QUrl>

/*!
  \class QXmlSchema

  \brief The QXmlSchema class provides loading and validation of a W3C XML Schema.

  \reentrant
  \since 4.X
  \ingroup xml-tools

  The QXmlSchema class loads, compiles and validates W3C XML Schema files
  that can be used further for validation of XML instance documents via
  \l{QXmlSchemaValidator}.
*/

/*!
  Constructs an invalid, empty schema that cannot be used until
  setSchema() is called.
 */
QXmlSchema::QXmlSchema()
    : d(new QXmlSchemaPrivate(QXmlNamePool()))
{
}

/*!
  Constructs a QXmlSchema that is a copy of \a other. The new
  instance will share resources with the existing schema
  to the extent possible.
 */
QXmlSchema::QXmlSchema(const QXmlSchema &other)
    : d(other.d)
{
}

/*!
  Destroys this QXmlSchema.
 */
QXmlSchema::~QXmlSchema()
{
}

/*!
  Sets this QXmlSchema to a schema loaded from the \a source
  URI.
 */
void QXmlSchema::load(const QUrl &source)
{
    d->load(source, QString());
}

/*!
  Sets this QXmlSchema to a schema read from the \a source
  device. The device must have been opened with at least
  QIODevice::ReadOnly.

  \a documentUri represents the schema obtained from the \a source
  device. It is the base URI of the schema, that is used
  internally to resolve relative URIs that appear in the schema, and
  for message reporting.

  If \a source is \c null or not readable, or if \a documentUri is not
  a valid URI, behavior is undefined.
  \sa isValid()
 */
void QXmlSchema::load(QIODevice *source, const QUrl &documentUri)
{
    d->load(source, documentUri, QString());
}

/*!
  Sets this QXmlSchema to a schema read from the \a data

  \a documentUri represents the schema obtained from the \a data.
  It is the base URI of the schema, that is used internally to
  resolve relative URIs that appear in the schema, and
  for message reporting.

  If \a documentUri is not a valid URI, behavior is undefined.
  \sa isValid()
 */
void QXmlSchema::load(const QByteArray &data, const QUrl &documentUri)
{
    d->load(data, documentUri, QString());
}

/*!
  Returns true if this schema is valid. Examples of invalid schemas
  are ones that contain syntax errors or that do not conform the
  W3C XML Schema specification.
 */
bool QXmlSchema::isValid() const
{
    return d->isValid();
}

/*!
  Returns the name pool used by this QXmlSchema for constructing \l
  {QXmlName} {names}. There is no setter for the name pool, because
  mixing name pools causes errors due to name confusion.
 */
QXmlNamePool QXmlSchema::namePool() const
{
    return d->namePool();
}

/*!
  Returns the document URI of the schema or an empty URI if no
  schema has been set.
 */
QUrl QXmlSchema::documentUri() const
{
    return d->documentUri();
}

/*!
  Changes the \l {QAbstractMessageHandler}{message handler} for this
  QXmlSchema to \a handler. The schema sends all compile and
  validation messages to this message handler. QXmlSchema does not take
  ownership of \a handler.

  Normally, the default message handler is sufficient. It writes
  compile and validation messages to \e stderr. The default message
  handler includes color codes if \e stderr can render colors.

  When QXmlSchema calls QAbstractMessageHandler::message(),
  the arguments are as follows:

  \table
  \header
    \o message() argument
    \o Semantics
  \row
    \o QtMsgType type
    \o Only QtWarningMsg and QtFatalMsg are used. The former
       identifies a warning, while the latter identifies an error.
  \row
    \o const QString & description
    \o An XHTML document which is the actual message. It is translated
       into the current language.
  \row
    \o const QUrl &identifier
    \o Identifies the error with a URI, where the fragment is
       the error code, and the rest of the URI is the error namespace.
  \row
    \o const QSourceLocation & sourceLocation
    \o Identifies where the error occurred.
  \endtable

 */
void QXmlSchema::setMessageHandler(QAbstractMessageHandler *handler)
{
    d->setMessageHandler(handler);
}

/*!
    Returns the message handler that handles compile and validation
    messages for this QXmlSchema.
 */
QAbstractMessageHandler *QXmlSchema::messageHandler() const
{
    return d->messageHandler();
}

/*!
  Sets the URI resolver to \a resolver. QXmlSchema does not take
  ownership of \a resolver.

  \sa uriResolver()
 */
void QXmlSchema::setUriResolver(QAbstractUriResolver *resolver)
{
    d->setUriResolver(resolver);
}

/*!
  Returns the schema's URI resolver. If no URI resolver has been set,
  QtXmlPatterns will use the URIs in queries as they are.

  The URI resolver provides a level of abstraction, or \e{polymorphic
  URIs}. A resolver can rewrite \e{logical} URIs to physical ones, or
  it can translate obsolete or invalid URIs to valid ones.

  When QtXmlPatterns calls QAbstractUriResolver::resolve() the
  absolute URI is the URI mandated by the schema specification, and the
  relative URI is the URI specified by the user.

  \sa setUriResolver()
 */
QAbstractUriResolver *QXmlSchema::uriResolver() const
{
    return d->uriResolver();
}

/*!
  Sets the network manager to \a manager.
  QXmlSchema does not take ownership of \a manager.

  \sa networkAccessManager()
 */
void QXmlSchema::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    d->setNetworkAccessManager(manager);
}

/*!
  Returns the network manager, or 0 if it has not been set.

  \sa setNetworkAccessManager()
 */
QNetworkAccessManager *QXmlSchema::networkAccessManager() const
{
    return d->networkAccessManager();
}
