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

#include "qdeclarativecomponent.h"
#include "private/qdeclarativecomponent_p.h"

#include "private/qdeclarativecompiler_p.h"
#include "private/qdeclarativecontext_p.h"
#include "private/qdeclarativecompositetypedata_p.h"
#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativevme_p.h"
#include "qdeclarative.h"
#include "qdeclarativeengine.h"
#include "private/qdeclarativebinding_p.h"
#include "private/qdeclarativebinding_p_p.h"
#include "private/qdeclarativeglobal_p.h"
#include "private/qdeclarativescriptparser_p.h"
#include "private/qdeclarativedebugtrace_p.h"

#include <QStack>
#include <QStringList>
#include <QFileInfo>
#include <QtCore/qdebug.h>
#include <QApplication>

QT_BEGIN_NAMESPACE

class QByteArray;

/*!
    \class QDeclarativeComponent
    \since 4.7
    \brief The QDeclarativeComponent class encapsulates a QML component definition.
    \mainclass

    Components are reusable, encapsulated QML elements with well-defined interfaces.
    They are often defined in \l {qdeclarativedocuments.html}{Component Files}.

    A QDeclarativeComponent instance can be created from a QML file.
    For example, if there is a \c main.qml file like this:

    \qml
    import Qt 4.7

    Item {
        width: 200
        height: 200
    }
    \endqml

    The following code loads this QML file as a component, creates an instance of
    this component using create(), and then queries the \l Item's \l {Item::}{width}
    value:

    \code
    QDeclarativeEngine *engine = new QDeclarativeEngine;
    QDeclarativeComponent component(engine, QUrl::fromLocalFile("main.qml"));

    QObject *myObject = component.create();
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(myObject);
    int width = item->width();  // width = 200
    \endcode

    \sa {Using QML in C++ Applications}, {Integrating QML with existing Qt UI code}
*/

/*!
    \qmlclass Component QDeclarativeComponent
    \since 4.7
    \brief The Component element encapsulates a QML component definition.

    Components are reusable, encapsulated QML elements with well-defined interfaces.

    Components are often defined by \l {qdeclarativedocuments.html}{component files} -
    that is, \c .qml files. The \e Component element allows components to be defined
    within QML items rather than in a separate file. This may be useful for reusing 
    a small component within a QML file, or for defining a component that logically 
    belongs with other QML components within a file.

    For example, here is a component that is used by multiple \l Loader objects.
    It contains a top level \l Rectangle item:

    \snippet doc/src/snippets/declarative/component.qml 0

    Notice that while a \l Rectangle by itself would be automatically 
    rendered and displayed, this is not the case for the above rectangle
    because it is defined inside a \c Component. The component encapsulates the
    QML elements within, as if they were defined in a separate \c .qml
    file, and is not loaded until requested (in this case, by the
    two \l Loader objects).

    A Component cannot contain anything other
    than an \c id and a single top level item. While the \c id is optional,
    the top level item is not; you cannot define an empty component.

    The Component element is commonly used to provide graphical components
    for views. For example, the ListView::delegate property requires a Component
    to specify how each list item is to be displayed.

    Component objects can also be dynamically created using
    \l{QML:Qt::createComponent()}{Qt.createComponent()}.
*/

/*!
    \qmlattachedsignal Component::onCompleted()

    Emitted after component "startup" has completed.  This can be used to
    execute script code at startup, once the full QML environment has been
    established.

    The \c {Component::onCompleted} attached property can be applied to
    any element.  The order of running the \c onCompleted scripts is
    undefined.

    \qml
    Rectangle {
        Component.onCompleted: console.log("Completed Running!")
        Rectangle {
            Component.onCompleted: console.log("Nested Completed Running!")
        }
    }
    \endqml
*/

/*!
    \qmlattachedsignal Component::onDestruction()

    Emitted as the component begins destruction.  This can be used to undo
    work done in the onCompleted signal, or other imperative code in your
    application.

    The \c {Component::onDestruction} attached property can be applied to
    any element.  However, it applies to the destruction of the component as
    a whole, and not the destruction of the specific object.  The order of
    running the \c onDestruction scripts is undefined.

    \qml
    Rectangle {
        Component.onDestruction: console.log("Destruction Beginning!")
        Rectangle {
            Component.onDestruction: console.log("Nested Destruction Beginning!")
        }
    }
    \endqml

    \sa QtDeclarative
*/

/*!
    \enum QDeclarativeComponent::Status
    
    Specifies the loading status of the QDeclarativeComponent.

    \value Null This QDeclarativeComponent has no data.  Call loadUrl() or setData() to add QML content.
    \value Ready This QDeclarativeComponent is ready and create() may be called.
    \value Loading This QDeclarativeComponent is loading network data.
    \value Error An error has occurred.  Call errors() to retrieve a list of \{QDeclarativeError}{errors}.
*/

void QDeclarativeComponentPrivate::typeDataReady()
{
    Q_Q(QDeclarativeComponent);

    Q_ASSERT(typeData);

    fromTypeData(typeData);
    typeData = 0;

    emit q->statusChanged(q->status());
}

void QDeclarativeComponentPrivate::updateProgress(qreal p)
{
    Q_Q(QDeclarativeComponent);

    progress = p;
    emit q->progressChanged(p);
}

void QDeclarativeComponentPrivate::fromTypeData(QDeclarativeCompositeTypeData *data)
{
    url = data->imports.baseUrl();
    QDeclarativeCompiledData *c = data->toCompiledComponent(engine);

    if (!c) {
        Q_ASSERT(data->status == QDeclarativeCompositeTypeData::Error);

        state.errors = data->errors;

    } else {

        cc = c;

    }

    data->release();
}

void QDeclarativeComponentPrivate::clear()
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
QDeclarativeComponent::QDeclarativeComponent(QObject *parent)
    : QObject(*(new QDeclarativeComponentPrivate), parent)
{
}

/*!
    Destruct the QDeclarativeComponent.
*/
QDeclarativeComponent::~QDeclarativeComponent()
{
    Q_D(QDeclarativeComponent);

    if (d->state.completePending) {
        qWarning("QDeclarativeComponent: Component destroyed while completion pending");
        d->completeCreate();
    }

    if (d->typeData) {
        d->typeData->remWaiter(d);
        d->typeData->release();
    }
    if (d->cc)
        d->cc->release();
}

/*!
    \qmlproperty enumeration Component::status
    This property holds the status of component loading.  It can be one of:
    \list
    \o Component.Null - no data is available for the component
    \o Component.Ready - the component has been loaded, and can be used to create instances.
    \o Component.Loading - the component is currently being loaded
    \o Component.Error - an error occurred while loading the component.
               Calling errorString() will provide a human-readable description of any errors.
    \endlist
 */

/*!
    \property QDeclarativeComponent::status
    The component's current \l{QDeclarativeComponent::Status} {status}.
 */
QDeclarativeComponent::Status QDeclarativeComponent::status() const
{
    Q_D(const QDeclarativeComponent);

    if (d->typeData)
        return Loading;
    else if (!d->state.errors.isEmpty())
        return Error;
    else if (d->engine && d->cc)
        return Ready;
    else
        return Null;
}

/*!
    Returns true if status() == QDeclarativeComponent::Null.
*/
bool QDeclarativeComponent::isNull() const
{
    return status() == Null;
}

/*!
    Returns true if status() == QDeclarativeComponent::Ready.
*/
bool QDeclarativeComponent::isReady() const
{
    return status() == Ready;
}

/*!
    Returns true if status() == QDeclarativeComponent::Error.
*/
bool QDeclarativeComponent::isError() const
{
    return status() == Error;
}

/*!
    Returns true if status() == QDeclarativeComponent::Loading.
*/
bool QDeclarativeComponent::isLoading() const
{
    return status() == Loading;
}

/*!
    \qmlproperty real Component::progress
    The progress of loading the component, from 0.0 (nothing loaded)
    to 1.0 (finished).
*/

/*!
    \property QDeclarativeComponent::progress
    The progress of loading the component, from 0.0 (nothing loaded)
    to 1.0 (finished).
*/
qreal QDeclarativeComponent::progress() const
{
    Q_D(const QDeclarativeComponent);
    return d->progress;
}

/*!
    \fn void QDeclarativeComponent::progressChanged(qreal progress)

    Emitted whenever the component's loading progress changes.  \a progress will be the
    current progress between 0.0 (nothing loaded) and 1.0 (finished).
*/

/*!
    \fn void QDeclarativeComponent::statusChanged(QDeclarativeComponent::Status status)

    Emitted whenever the component's status changes.  \a status will be the
    new status.
*/

/*!
    Create a QDeclarativeComponent with no data and give it the specified
    \a engine and \a parent. Set the data with setData().
*/
QDeclarativeComponent::QDeclarativeComponent(QDeclarativeEngine *engine, QObject *parent)
    : QObject(*(new QDeclarativeComponentPrivate), parent)
{
    Q_D(QDeclarativeComponent);
    d->engine = engine;
}

/*!
    Create a QDeclarativeComponent from the given \a url and give it the
    specified \a parent and \a engine.

    Ensure that the URL provided is full and correct, in particular, use
    \l QUrl::fromLocalFile() when loading a file from the local filesystem.

    \sa loadUrl()
*/
QDeclarativeComponent::QDeclarativeComponent(QDeclarativeEngine *engine, const QUrl &url, QObject *parent)
: QObject(*(new QDeclarativeComponentPrivate), parent)
{
    Q_D(QDeclarativeComponent);
    d->engine = engine;
    loadUrl(url);
}

/*!
    Create a QDeclarativeComponent from the given \a fileName and give it the specified 
    \a parent and \a engine.

    \sa loadUrl()
*/
QDeclarativeComponent::QDeclarativeComponent(QDeclarativeEngine *engine, const QString &fileName, 
                           QObject *parent)
: QObject(*(new QDeclarativeComponentPrivate), parent)
{
    Q_D(QDeclarativeComponent);
    d->engine = engine;
    loadUrl(d->engine->baseUrl().resolved(QUrl::fromLocalFile(fileName)));
}

/*!
    \internal
*/
QDeclarativeComponent::QDeclarativeComponent(QDeclarativeEngine *engine, QDeclarativeCompiledData *cc, int start, int count, QObject *parent)
    : QObject(*(new QDeclarativeComponentPrivate), parent)
{
    Q_D(QDeclarativeComponent);
    d->engine = engine;
    d->cc = cc;
    cc->addref();
    d->start = start;
    d->count = count;
    d->url = cc->url;
    d->progress = 1.0;
}

/*!
    Sets the QDeclarativeComponent to use the given QML \a data.  If \a url
    is provided, it is used to set the component name and to provide
    a base path for items resolved by this component.
*/
void QDeclarativeComponent::setData(const QByteArray &data, const QUrl &url)
{
    Q_D(QDeclarativeComponent);

    d->clear();

    d->url = url;

    QDeclarativeCompositeTypeData *typeData = 
        QDeclarativeEnginePrivate::get(d->engine)->typeManager.getImmediate(data, url);
    
    if (typeData->status == QDeclarativeCompositeTypeData::Waiting
     || typeData->status == QDeclarativeCompositeTypeData::WaitingResources)
    {
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
Returns the QDeclarativeContext the component was created in.  This is only
valid for components created directly from QML.
*/
QDeclarativeContext *QDeclarativeComponent::creationContext() const
{
    Q_D(const QDeclarativeComponent);
    if(d->creationContext)
        return d->creationContext->asQDeclarativeContext();

    return qmlContext(this);
}

/*!
    Load the QDeclarativeComponent from the provided \a url.

    Ensure that the URL provided is full and correct, in particular, use
    \l QUrl::fromLocalFile() when loading a file from the local filesystem.
*/
void QDeclarativeComponent::loadUrl(const QUrl &url)
{
    Q_D(QDeclarativeComponent);

    d->clear();

    if ((url.isRelative() && !url.isEmpty())
    || url.scheme() == QLatin1String("file")) // Workaround QTBUG-11929
        d->url = d->engine->baseUrl().resolved(url);
    else
        d->url = url;

    if (url.isEmpty()) {
        QDeclarativeError error;
        error.setDescription(tr("Invalid empty URL"));
        d->state.errors << error;
        return;
    }

    QDeclarativeCompositeTypeData *data = 
        QDeclarativeEnginePrivate::get(d->engine)->typeManager.get(d->url);

    if (data->status == QDeclarativeCompositeTypeData::Waiting
     || data->status == QDeclarativeCompositeTypeData::WaitingResources)
    {
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
    Return the list of errors that occurred during the last compile or create
    operation.  An empty list is returned if isError() is not set.
*/
QList<QDeclarativeError> QDeclarativeComponent::errors() const
{
    Q_D(const QDeclarativeComponent);
    if (isError())
        return d->state.errors;
    else
        return QList<QDeclarativeError>();
}

/*!
    \qmlmethod string Component::errorString()

    Returns a human-readable description of any errors.

    The string includes the file, location, and description of each error.
    If multiple errors are present they are separated by a newline character.

    If no errors are present, an empty string is returned.
*/

/*!
    \internal
    errorString is only meant as a way to get the errors in script
*/
QString QDeclarativeComponent::errorString() const
{
    Q_D(const QDeclarativeComponent);
    QString ret;
    if(!isError())
        return ret;
    foreach(const QDeclarativeError &e, d->state.errors) {
        ret += e.url().toString() + QLatin1Char(':') +
               QString::number(e.line()) + QLatin1Char(' ') +
               e.description() + QLatin1Char('\n');
    }
    return ret;
}

/*!
    \qmlproperty url Component::url
    The component URL.  This is the URL that was used to construct the component.
*/

/*!
    \property QDeclarativeComponent::url
    The component URL.  This is the URL passed to either the constructor,
    or the loadUrl() or setData() methods.
*/
QUrl QDeclarativeComponent::url() const
{
    Q_D(const QDeclarativeComponent);
    return d->url;
}

/*!
    \internal
*/
QDeclarativeComponent::QDeclarativeComponent(QDeclarativeComponentPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
    \qmlmethod object Component::createObject(parent)

    Creates and returns an object instance of this component that will have the given 
    \a parent. Returns null if object creation fails.

    The object will be created in the same context as the one in which the component
    was created. This function will always return null when called on components
    which were not created in QML.

    If you wish to create an object without setting a parent, specify \c null for
    the \a parent value. Note that if the returned object is to be displayed, you 
    must provide a valid \a parent value or set the returned object's \l{Item::parent}{parent} 
    property, or else the object will not be visible.

    Dynamically created instances can be deleted with the \c destroy() method.
    See \l {Dynamic Object Management in QML} for more information.
*/

/*!
    \internal
    A version of create which returns a scriptObject, for use in script.
    This function will only work on components created in QML.

    Sets graphics object parent because forgetting to do this is a frequent
    and serious problem.
*/
QScriptValue QDeclarativeComponent::createObject(QObject* parent)
{
    Q_D(QDeclarativeComponent);
    QDeclarativeContext* ctxt = creationContext();
    if(!ctxt && d->engine)
        ctxt = d->engine->rootContext();
    if (!ctxt)
        return QScriptValue(QScriptValue::NullValue);
    QObject* ret = create(ctxt);
    if (!ret)
        return QScriptValue(QScriptValue::NullValue);


    if (parent) {
        ret->setParent(parent);
        QList<QDeclarativePrivate::AutoParentFunction> functions = QDeclarativeMetaType::parentFunctions();

        bool needParent = false;

        for (int ii = 0; ii < functions.count(); ++ii) {
            QDeclarativePrivate::AutoParentResult res = functions.at(ii)(ret, parent);
            if (res == QDeclarativePrivate::Parented) {
                needParent = false;
                break;
            } else if (res == QDeclarativePrivate::IncompatibleParent) {
                needParent = true;
            }
        }

        if (needParent) 
            qWarning("QDeclarativeComponent: Created graphical object was not placed in the graphics scene.");
    }

    QDeclarativeEnginePrivate *priv = QDeclarativeEnginePrivate::get(d->engine);
    QDeclarativeData::get(ret, true)->setImplicitDestructible();
    return priv->objectClass->newQObject(ret, QMetaType::QObjectStar);
}

/*!
    Create an object instance from this component.  Returns 0 if creation
    failed.  \a context specifies the context within which to create the object
    instance.  

    If \a context is 0 (the default), it will create the instance in the
    engine' s \l {QDeclarativeEngine::rootContext()}{root context}.
*/
QObject *QDeclarativeComponent::create(QDeclarativeContext *context)
{
    Q_D(QDeclarativeComponent);

    if (!context)
        context = d->engine->rootContext();

    QObject *rv = beginCreate(context);
    completeCreate();
    return rv;
}

QObject *QDeclarativeComponentPrivate::create(QDeclarativeContextData *context, 
                                              const QBitField &bindings)
{
    if (!context)
        context = QDeclarativeContextData::get(engine->rootContext());

    QObject *rv = beginCreate(context, bindings);
    completeCreate();
    return rv;
}

/*!
    This method provides more advanced control over component instance creation.
    In general, programmers should use QDeclarativeComponent::create() to create a 
    component.

    Create an object instance from this component.  Returns 0 if creation
    failed.  \a context specifies the context within which to create the object
    instance.  

    When QDeclarativeComponent constructs an instance, it occurs in three steps:
    \list 1
    \i The object hierarchy is created, and constant values are assigned.
    \i Property bindings are evaluated for the the first time.
    \i If applicable, QDeclarativeParserStatus::componentComplete() is called on objects.
    \endlist 
    QDeclarativeComponent::beginCreate() differs from QDeclarativeComponent::create() in that it
    only performs step 1.  QDeclarativeComponent::completeCreate() must be called to 
    complete steps 2 and 3.

    This breaking point is sometimes useful when using attached properties to
    communicate information to an instantiated component, as it allows their
    initial values to be configured before property bindings take effect.
*/
QObject *QDeclarativeComponent::beginCreate(QDeclarativeContext *context)
{
    Q_D(QDeclarativeComponent);
    QObject *rv = d->beginCreate(context?QDeclarativeContextData::get(context):0, QBitField());
    if (rv) {
        QDeclarativeData *ddata = QDeclarativeData::get(rv);
        Q_ASSERT(ddata);
        ddata->indestructible = true;
    }
    return rv;
}

QObject *
QDeclarativeComponentPrivate::beginCreate(QDeclarativeContextData *context, const QBitField &bindings)
{
    Q_Q(QDeclarativeComponent);
    if (!context) {
        qWarning("QDeclarativeComponent: Cannot create a component in a null context");
        return 0;
    }

    if (!context->isValid()) {
        qWarning("QDeclarativeComponent: Cannot create a component in an invalid context");
        return 0;
    }

    if (context->engine != engine) {
        qWarning("QDeclarativeComponent: Must create component in context from the same QDeclarativeEngine");
        return 0;
    }

    if (state.completePending) {
        qWarning("QDeclarativeComponent: Cannot create new component instance before completing the previous");
        return 0;
    }

    if (!q->isReady()) {
        qWarning("QDeclarativeComponent: Component is not ready");
        return 0;
    }

    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);

    bool isRoot = !ep->inBeginCreate;
    if (isRoot) 
        QDeclarativeDebugTrace::startRange(QDeclarativeDebugTrace::Creating);
    QDeclarativeDebugTrace::rangeData(QDeclarativeDebugTrace::Creating, cc->url);

    QDeclarativeContextData *ctxt = new QDeclarativeContextData;
    ctxt->isInternal = true;
    ctxt->url = cc->url;
    ctxt->imports = cc->importCache;

    // Nested global imports
    if (creationContext && start != -1) 
        ctxt->importedScripts = creationContext->importedScripts;

    cc->importCache->addref();
    ctxt->setParent(context);

    QObject *rv = begin(ctxt, ep, cc, start, count, &state, bindings);

    if (rv && !context->isInternal && ep->isDebugging)
        context->asQDeclarativeContextPrivate()->instances.append(rv);

    return rv;
}

QObject * QDeclarativeComponentPrivate::begin(QDeclarativeContextData *ctxt, QDeclarativeEnginePrivate *enginePriv,
                                              QDeclarativeCompiledData *component, int start, int count,
                                              ConstructionState *state, const QBitField &bindings)
{
    bool isRoot = !enginePriv->inBeginCreate;
    enginePriv->inBeginCreate = true;

    QDeclarativeVME vme;
    QObject *rv = vme.run(ctxt, component, start, count, bindings);

    if (vme.isError()) 
        state->errors = vme.errors();

    if (isRoot) {
        enginePriv->inBeginCreate = false;

        state->bindValues = enginePriv->bindValues;
        state->parserStatus = enginePriv->parserStatus;
        state->finalizedParserStatus = enginePriv->finalizedParserStatus;
        state->componentAttached = enginePriv->componentAttached;
        if (state->componentAttached)
            state->componentAttached->prev = &state->componentAttached;

        enginePriv->componentAttached = 0;
        enginePriv->bindValues.clear();
        enginePriv->parserStatus.clear();
        enginePriv->finalizedParserStatus.clear();
        state->completePending = true;
        enginePriv->inProgressCreations++;
    }

    return rv;
}

void QDeclarativeComponentPrivate::beginDeferred(QDeclarativeEnginePrivate *enginePriv,
                                                 QObject *object, ConstructionState *state)
{
    bool isRoot = !enginePriv->inBeginCreate;
    enginePriv->inBeginCreate = true;

    QDeclarativeVME vme;
    vme.runDeferred(object);

    if (vme.isError()) 
        state->errors = vme.errors();

    if (isRoot) {
        enginePriv->inBeginCreate = false;

        state->bindValues = enginePriv->bindValues;
        state->parserStatus = enginePriv->parserStatus;
        state->finalizedParserStatus = enginePriv->finalizedParserStatus;
        state->componentAttached = enginePriv->componentAttached;
        if (state->componentAttached)
            state->componentAttached->prev = &state->componentAttached;

        enginePriv->componentAttached = 0;
        enginePriv->bindValues.clear();
        enginePriv->parserStatus.clear();
        enginePriv->finalizedParserStatus.clear();
        state->completePending = true;
        enginePriv->inProgressCreations++;
    }
}

void QDeclarativeComponentPrivate::complete(QDeclarativeEnginePrivate *enginePriv, ConstructionState *state)
{
    if (state->completePending) {

        for (int ii = 0; ii < state->bindValues.count(); ++ii) {
            QDeclarativeEnginePrivate::SimpleList<QDeclarativeAbstractBinding> bv = 
                state->bindValues.at(ii);
            for (int jj = 0; jj < bv.count; ++jj) {
                if(bv.at(jj)) 
                    bv.at(jj)->setEnabled(true, QDeclarativePropertyPrivate::BypassInterceptor | 
                                                QDeclarativePropertyPrivate::DontRemoveBinding);
            }
            QDeclarativeEnginePrivate::clear(bv);
        }

        for (int ii = 0; ii < state->parserStatus.count(); ++ii) {
            QDeclarativeEnginePrivate::SimpleList<QDeclarativeParserStatus> ps = 
                state->parserStatus.at(ii);

            for (int jj = ps.count - 1; jj >= 0; --jj) {
                QDeclarativeParserStatus *status = ps.at(jj);
                if (status && status->d) {
                    status->d = 0;
                    status->componentComplete();
                }
            }
            QDeclarativeEnginePrivate::clear(ps);
        }

        for (int ii = 0; ii < state->finalizedParserStatus.count(); ++ii) {
            QPair<QDeclarativeGuard<QObject>, int> status = state->finalizedParserStatus.at(ii);
            QObject *obj = status.first;
            if (obj) {
                void *args[] = { 0 };
                QMetaObject::metacall(obj, QMetaObject::InvokeMetaMethod,
                                      status.second, args);
            }
        }

        while (state->componentAttached) {
            QDeclarativeComponentAttached *a = state->componentAttached;
            a->rem();
            QDeclarativeData *d = QDeclarativeData::get(a->parent());
            Q_ASSERT(d);
            Q_ASSERT(d->context);
            a->add(&d->context->componentAttached);
            emit a->completed();
        }

        state->bindValues.clear();
        state->parserStatus.clear();
        state->finalizedParserStatus.clear();
        state->completePending = false;

        enginePriv->inProgressCreations--;
        if (0 == enginePriv->inProgressCreations) {
            while (enginePriv->erroredBindings) {
                enginePriv->warning(enginePriv->erroredBindings->error);
                enginePriv->erroredBindings->removeError();
            }
        }

    }
}

/*!
    This method provides more advanced control over component instance creation.
    In general, programmers should use QDeclarativeComponent::create() to create a 
    component.

    Complete a component creation begin with QDeclarativeComponent::beginCreate().
*/
void QDeclarativeComponent::completeCreate()
{
    Q_D(QDeclarativeComponent);
    d->completeCreate();
}

void QDeclarativeComponentPrivate::completeCreate()
{
    if (state.completePending) {
        QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);
        complete(ep, &state);

        QDeclarativeDebugTrace::endRange(QDeclarativeDebugTrace::Creating);
    }
}

QDeclarativeComponentAttached::QDeclarativeComponentAttached(QObject *parent)
: QObject(parent), prev(0), next(0)
{
}

QDeclarativeComponentAttached::~QDeclarativeComponentAttached()
{
    if (prev) *prev = next;
    if (next) next->prev = prev;
    prev = 0;
    next = 0;
}

/*!
    \internal
*/
QDeclarativeComponentAttached *QDeclarativeComponent::qmlAttachedProperties(QObject *obj)
{
    QDeclarativeComponentAttached *a = new QDeclarativeComponentAttached(obj);

    QDeclarativeEngine *engine = qmlEngine(obj);
    if (!engine)
        return a;

    if (QDeclarativeEnginePrivate::get(engine)->inBeginCreate) {
        QDeclarativeEnginePrivate *p = QDeclarativeEnginePrivate::get(engine);
        a->add(&p->componentAttached);
    } else {
        QDeclarativeData *d = QDeclarativeData::get(obj);
        Q_ASSERT(d);
        Q_ASSERT(d->context);
        a->add(&d->context->componentAttached);
    }

    return a;
}

QT_END_NAMESPACE
