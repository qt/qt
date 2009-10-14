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

#include "qmlcomponent.h"
#include "qmlcomponent_p.h"
#include "qmlcompiler_p.h"
#include "private/qmlcontext_p.h"
#include "private/qmlcompositetypedata_p.h"
#include "private/qmlengine_p.h"
#include "qmlvme_p.h"
#include "qml.h"
#include <QStack>
#include <private/qfxperf_p.h>
#include <QStringList>
#include <qmlengine.h>
#include <QFileInfo>
#include "qmlbinding.h"
#include <QtCore/qdebug.h>
#include <QApplication>

#include "qmlscriptparser_p.h"

QT_BEGIN_NAMESPACE

class QByteArray;
int statusId = qRegisterMetaType<QmlComponent::Status>("QmlComponent::Status");

/*!
    \class QmlComponent
    \brief The QmlComponent class encapsulates a QML component description.
    \mainclass
*/

/*!
    \qmlclass Component QmlComponent
    \brief The Component element encapsulates a QML component description.

    Components are reusable, encapsulated Qml element with a well-defined interface.
    They are often defined in \l {components}{Component Files}.

    The \e Component element allows defining components within a QML file.
    This can be useful for reusing a small component within a single QML
    file, or for defining a component that logically belongs with the
    file containing it.

    \qml
Item {
    Component {
        id: RedSquare
        Rectangle {
            color: "red"
            width: 10
            height: 10
        }
    }
    Loader { sourceComponent: RedSquare }
    Loader { sourceComponent: RedSquare; x: 20 }
}
    \endqml

    \section1 Attached Properties

    \e onCompleted

    Emitted after component "startup" has completed.  This can be used to
    execute script code at startup, once the full QML environment has been 
    established.

    The \c {Component::onCompleted} attached property can be applied to
    any element.  The order of running the \c onCompleted scripts is
    undefined.

    \qml
    Rectangle {
        Component.onCompleted: print("Completed Running!")
        Rectangle {
            Component.onCompleted: print("Nested Completed Running!")
        }
    }
    \endqml
*/
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Component,QmlComponent);

/*!
    \enum QmlComponent::Status
    
    Specifies the loading status of the QmlComponent.

    \value Null This QmlComponent has no data.  Call loadUrl() or setData() to add QML content.
    \value Ready This QmlComponent is ready and create() may be called.
    \value Loading This QmlComponent is loading network data.
    \value Error An error has occured.  Calling errorDescription() to retrieve a description.
*/

void QmlComponentPrivate::typeDataReady()
{
    Q_Q(QmlComponent);

    Q_ASSERT(typeData);

    fromTypeData(typeData);
    typeData = 0;

    emit q->statusChanged(q->status());
}

void QmlComponentPrivate::updateProgress(qreal p)
{
    Q_Q(QmlComponent);

    progress = p;
    emit q->progressChanged(p);
}

void QmlComponentPrivate::fromTypeData(QmlCompositeTypeData *data)
{
    url = data->imports.baseUrl();
    QmlCompiledData *c = data->toCompiledComponent(engine);

    if (!c) {
        Q_ASSERT(data->status == QmlCompositeTypeData::Error);

        errors = data->errors;

    } else {

        cc = c;

    }

    data->release();
}

void QmlComponentPrivate::clear()
{
    if (typeData) {
        typeData->remWaiter(this);
        typeData->release();
        typeData = 0;
    }
        
    if (cc) { 
        cc->release();
        cc = 0;
    }
}

/*!
    \internal
*/
QmlComponent::QmlComponent(QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
}

/*!
    Destruct the QmlComponent.
*/
QmlComponent::~QmlComponent()
{
    Q_D(QmlComponent);
    if (d->typeData) {
        d->typeData->remWaiter(d);
        d->typeData->release();
    }
    if (d->cc)
        d->cc->release();

    for(int ii = 0; ii < d->bindValues.count(); ++ii)
        QmlEnginePrivate::clear(d->bindValues[ii]);
    for(int ii = 0; ii < d->parserStatus.count(); ++ii)
        QmlEnginePrivate::clear(d->parserStatus[ii]);
}

/*!
  Returns the component's current \l{QmlComponent::Status} {status}.
 */
QmlComponent::Status QmlComponent::status() const
{
    Q_D(const QmlComponent);

    if (d->typeData)
        return Loading;
    else if (!d->errors.isEmpty())
        return Error;
    else if (d->engine && d->cc)
        return Ready;
    else
        return Null;
}

/*!
    Returns true if the component is in the Null state, false otherwise.

    Equivalent to status() == QmlComponent::Null.
*/
bool QmlComponent::isNull() const
{
    return status() == Null;
}

/*!
    Returns true if the component is in the Ready state, false otherwise.

    Equivalent to status() == QmlComponent::Ready.
*/
bool QmlComponent::isReady() const
{
    return status() == Ready;
}

/*!
    Returns true if the component is in the Error state, false otherwise.

    Equivalent to status() == QmlComponent::Error.
*/
bool QmlComponent::isError() const
{
    return status() == Error;
}

/*!
    Returns true if the component is in the Loading state, false otherwise.

    Equivalent to status() == QmlComponent::Loading.
*/
bool QmlComponent::isLoading() const
{
    return status() == Loading;
}

qreal QmlComponent::progress() const
{
    Q_D(const QmlComponent);
    return d->progress;
}

/*!
    \fn void QmlComponent::statusChanged(QmlComponent::Status status)

    Emitted whenever the component's status changes.  \a status will be the
    new status.
*/

/*!
    Create a QmlComponent with no data and give it the specified
    \a engine and \a parent. Set the data with setData().
*/
QmlComponent::QmlComponent(QmlEngine *engine, QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
}

/*!
    Create a QmlComponent from the given \a url and give it the
    specified \a parent and \a engine.

    \sa loadUrl()
*/
QmlComponent::QmlComponent(QmlEngine *engine, const QUrl &url, QObject *parent)
: QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    loadUrl(url);
}

/*!
    Create a QmlComponent from the given \a fileName and give it the specified 
    \a parent and \a engine.

    \sa loadUrl()
*/
QmlComponent::QmlComponent(QmlEngine *engine, const QString &fileName, 
                           QObject *parent)
: QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    loadUrl(QUrl::fromLocalFile(fileName));
}

/*!
    Create a QmlComponent from the given QML \a data and give it the
    specified \a parent and \a engine.  \a url is used to provide a base path
    for items resolved by this component, and may be an empty url if the 
    component contains no items to resolve.

    \sa setData()
*/
QmlComponent::QmlComponent(QmlEngine *engine, const QByteArray &data, const QUrl &url, QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    setData(data,url);
}

/*!
    \internal
*/
QmlComponent::QmlComponent(QmlEngine *engine, QmlCompiledData *cc, int start, int count, QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    d->cc = cc;
    cc->addref();
    d->start = start;
    d->count = count;
    d->url = cc->url;
    d->progress = 1.0;
}

/*!
    Sets the QmlComponent to use the given QML \a data.  If \a url
    is provided, it is used to set the component name and to provide
    a base path for items resolved by this component.
*/
void QmlComponent::setData(const QByteArray &data, const QUrl &url)
{
    Q_D(QmlComponent);

    d->clear();

    d->url = url;

    QmlCompositeTypeData *typeData = 
        QmlEnginePrivate::get(d->engine)->typeManager.getImmediate(data, url);
    
    if (typeData->status == QmlCompositeTypeData::Waiting) {

        d->typeData = typeData;
        d->typeData->addWaiter(d);

    } else {

        d->fromTypeData(typeData);

    }

    d->progress = 1.0;
    emit statusChanged(status());
    emit progressChanged(d->progress);
}

/*!
    Load the QmlComponent from the provided \a url.
*/
void QmlComponent::loadUrl(const QUrl &url)
{
    Q_D(QmlComponent);

    d->clear();

    if (url.isRelative() && !url.isEmpty())
        d->url = d->engine->baseUrl().resolved(url);
    else
        d->url = url;

    QmlCompositeTypeData *data = 
        QmlEnginePrivate::get(d->engine)->typeManager.get(d->url);

    if (data->status == QmlCompositeTypeData::Waiting) {
        d->typeData = data;
        d->typeData->addWaiter(d);
        d->progress = data->progress;
    } else {
        d->fromTypeData(data);
        d->progress = 1.0;
    }

    emit statusChanged(status());
    emit progressChanged(d->progress);
}

/*!
    Return the list of errors that occured during the last compile or create
    operation.  An empty list is returned if isError() is not set.
*/
QList<QmlError> QmlComponent::errors() const
{
    Q_D(const QmlComponent);
    if (isError())
        return d->errors;
    else
        return QList<QmlError>();
}

/*!
    Return the component URL.  This is the URL passed to either the constructor,
    or the loadUrl() or setData() methods.
*/
QUrl QmlComponent::url() const
{
    Q_D(const QmlComponent);
    return d->url;
}

/*!
    \internal
*/
QmlComponent::QmlComponent(QmlComponentPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}


/*!
    Create an object instance from this component.  Returns 0 if creation
    failed.  \a context specifies the context within which to create the object
    instance.  

    If \a context is 0 (the default), it will create the instance in the
    engine' s \l {QmlEngine::rootContext()}{root context}.
*/
QObject *QmlComponent::create(QmlContext *context)
{
    Q_D(QmlComponent);

    return d->create(context, QBitField());
}

QObject *QmlComponentPrivate::create(QmlContext *context, 
                                     const QBitField &bindings)
{
    QObject *create(QmlContext *context, const QBitField &);
    if (!context)
        context = engine->rootContext();

    if (context->engine() != engine) {
        qWarning("QmlComponent::create(): Must create component in context from the same QmlEngine");
        return 0;
    }

    QObject *rv = beginCreate(context, bindings);
    completeCreate();
    return rv;
}

/*!
    This method provides more advanced control over component instance creation.
    In general, programmers should use QmlComponent::create() to create a 
    component.

    Create an object instance from this component.  Returns 0 if creation
    failed.  \a context specifies the context within which to create the object
    instance.  

    When QmlComponent constructs an instance, it occurs in three steps:
    \list 1
    \i The object hierarchy is created, and constant values are assigned.
    \i Property bindings are evaluated for the the first time.
    \i If applicable, QmlParserStatus::componentComplete() is called on objects.
    \endlist 
    QmlComponent::beginCreate() differs from QmlComponent::create() in that it
    only performs step 1.  QmlComponent::completeCreate() must be called to 
    complete steps 2 and 3.

    This breaking point is sometimes useful when using attached properties to
    communicate information to an instantiated component, as it allows their
    initial values to be configured before property bindings take effect.
*/
QObject *QmlComponent::beginCreate(QmlContext *context)
{
    Q_D(QmlComponent);
    return d->beginCreate(context, QBitField());
}

QObject *
QmlComponentPrivate::beginCreate(QmlContext *context, const QBitField &bindings)
{
    Q_Q(QmlComponent);
    if (!context) {
        qWarning("QmlComponent::beginCreate(): Cannot create a component in a null context");
        return 0;
    }

    if (context->engine() != engine) {
        qWarning("QmlComponent::beginCreate(): Must create component in context from the same QmlEngine");
        return 0;
    }

    if (completePending) {
        qWarning("QmlComponent: Cannot create new component instance before completing the previous");
        return 0;
    }

    if (!q->isReady()) {
        qWarning("QmlComponent: Component is not ready");
        return 0;
    }

    if (!QmlEnginePrivate::get(engine)->rootComponent)
        QmlEnginePrivate::get(engine)->rootComponent = q;

    QmlContextPrivate *contextPriv = 
        static_cast<QmlContextPrivate *>(QObjectPrivate::get(context));
    QmlContext *ctxt = new QmlContext(context, 0, true);
    static_cast<QmlContextPrivate*>(ctxt->d_func())->url = cc->url;
    static_cast<QmlContextPrivate*>(ctxt->d_func())->imports = cc->importCache;
    cc->importCache->addref();

    QmlVME vme;
    QObject *rv = vme.run(ctxt, cc, start, count, bindings);

    if (vme.isError()) 
        errors = vme.errors();

    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
    if (ep->rootComponent == q) {
        ep->rootComponent = 0;

        bindValues = ep->bindValues;
        parserStatus = ep->parserStatus;
        componentAttacheds = ep->componentAttacheds;
        if (componentAttacheds)
            componentAttacheds->prev = &componentAttacheds;

        ep->componentAttacheds = 0;
        ep->bindValues.clear();
        ep->parserStatus.clear();
        completePending = true;
    }

    if (rv) {
        QFx_setParent_noEvent(ctxt, rv);
    } else {
        delete ctxt;
    }

    if (rv && !contextPriv->isInternal && ep->isDebugging)
        contextPriv->instances.append(rv);
    return rv;
}

/*!
    This method provides more advanced control over component instance creation.
    In general, programmers should use QmlComponent::create() to create a 
    component.

    Complete a component creation begin with QmlComponent::beginCreate().
*/
void QmlComponent::completeCreate()
{
    Q_D(QmlComponent);
    d->completeCreate();
}

void QmlComponentPrivate::completeCreate()
{
    if (completePending) {
        {
#ifdef Q_ENABLE_PERFORMANCE_LOG
            QFxPerfTimer<QFxPerf::BindInit> bi;
#endif
            for (int ii = 0; ii < bindValues.count(); ++ii) {
                QmlEnginePrivate::SimpleList<QmlAbstractBinding> bv = 
                    bindValues.at(ii);
                for (int jj = 0; jj < bv.count; ++jj) {
                    if(bv.at(jj)) 
                        bv.at(jj)->setEnabled(true, QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
                }
                QmlEnginePrivate::clear(bv);
            }
        }

        for (int ii = 0; ii < parserStatus.count(); ++ii) {
            QmlEnginePrivate::SimpleList<QmlParserStatus> ps = 
                parserStatus.at(ii);

            for (int jj = 0; jj < ps.count; ++jj) {
                QmlParserStatus *status = ps.at(jj);
                if (status && status->d) {
                    status->d = 0;
                    status->componentComplete();
                }
            }
            QmlEnginePrivate::clear(ps);
        }

        while (componentAttacheds) {
            QmlComponentAttached *a = componentAttacheds;
            if (a->next) a->next->prev = &componentAttacheds;
            componentAttacheds = a->next;
            a->prev = 0; a->next = 0;
            emit a->completed();
        }

        bindValues.clear();
        parserStatus.clear();
        completePending = false;
    }
}

QmlComponentAttached::QmlComponentAttached(QObject *parent)
: QObject(parent), prev(0), next(0)
{
}

QmlComponentAttached::~QmlComponentAttached()
{
    if (prev) *prev = next;
    if (next) next->prev = prev;
    prev = 0;
    next = 0;
}

QmlComponentAttached *QmlComponent::qmlAttachedProperties(QObject *obj)
{
    QmlComponentAttached *a = new QmlComponentAttached(obj);

    QmlEngine *engine = qmlEngine(obj);
    if (!engine || !QmlEnginePrivate::get(engine)->rootComponent)
        return a;

    QmlEnginePrivate *p = QmlEnginePrivate::get(engine);

    a->next = p->componentAttacheds;
    a->prev = &p->componentAttacheds;
    if (a->next) a->next->prev = &a->next;
    p->componentAttacheds = a;

    return a;
}

QT_END_NAMESPACE
