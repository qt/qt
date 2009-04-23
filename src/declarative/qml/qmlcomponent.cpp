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
#include "private/qmlcontext_p.h"
#include "private/qmlengine_p.h"
#include "qmlvme_p.h"
#include "qml.h"
#include <QStack>
#include <qfxperf.h>
#include <QStringList>
#include <qmlengine.h>
#include <QFileInfo>
#include <qmlbindablevalue.h>
#include "private/qmlxmlparser_p.h"
#include "qmlcompiledcomponent_p.h"
#include <QtCore/qdebug.h>
#include <QApplication>


QT_BEGIN_NAMESPACE
class QByteArray;

/*!
    \class QmlComponent
    \brief The QmlComponent class encapsulates a QML component description.
    \mainclass
*/
QML_DEFINE_TYPE(QmlComponent,Component);

/*!
    \enum QmlComponent::Status
    
    Specifies the loading status of the QmlComponent.

    \o Null This QmlComponent has no data.  Call loadUrl() or setData() to add QML content.
    \o Ready This QmlComponent is ready and create() may be called.
    \o Loading This QmlComponent is loading network data.
    \o Error An error has occured.  Calling errorDescription() to retrieve a description.
*/

void QmlComponentPrivate::typeDataReady()
{
    Q_Q(QmlComponent);

    Q_ASSERT(typeData);

    fromTypeData(typeData);
    typeData = 0;

    emit q->statusChanged(q->status());
}

void QmlComponentPrivate::fromTypeData(QmlCompositeTypeData *data)
{
    url = QUrl(data->url);
    QmlCompiledComponent *c = data->toCompiledComponent(engine);

    if(!c) {
        Q_ASSERT(data->status == QmlCompositeTypeData::Error);

        errorDescription = data->errorDescription;
        qWarning().nospace() << "QmlComponent: "
                             << data->errorDescription.toLatin1().constData();

    } else {

        cc = c;

    }

    data->release();
}

void QmlComponentPrivate::clear()
{
    if(typeData) {
        typeData->remWaiter(this);
        typeData->release();
        typeData = 0;
    }
        
    if(cc) { 
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
}

QmlComponent::Status QmlComponent::status() const
{
    Q_D(const QmlComponent);

    if(d->typeData)
        return Loading;
    else if(d->engine && d->cc)
        return Ready;
    else if(!d->errorDescription.isEmpty())
        return Error;
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

/*!
    \fn void QmlComponent::statusChanged(QmlComponent::Status status)

    Emitted whenever the component's status changes.  \a status will be the
    new status.
*/

/*!
    Create a QmlComponent with no data. Set setData().
*/
QmlComponent::QmlComponent(QmlEngine *engine, QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
}

/*!
    Create a QmlComponent from the given \a url.
*/
QmlComponent::QmlComponent(QmlEngine *engine, const QUrl &url, QObject *parent)
: QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    loadUrl(url);
}

/*!
    Create a QmlComponent from the given XML \a data.  If provided, \a filename
    is used to set the component name, and to provide a base path for items
    resolved by this component.
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
QmlComponent::QmlComponent(QmlEngine *engine, QmlCompiledComponent *cc, int start, int count, QObject *parent)
    : QObject(*(new QmlComponentPrivate), parent)
{
    Q_D(QmlComponent);
    d->engine = engine;
    d->cc = cc;
    cc->addref();
    d->start = start;
    d->count = count;
}

/*!
    Sets the QmlComponent to use the given XML \a data.  If provided, 
    \a filename is used to set the component name, and to provide a base path 
    for items resolved by this component.
*/
void QmlComponent::setData(const QByteArray &data, const QUrl &url)
{
    Q_D(QmlComponent);

    d->clear();

    d->url = url;

    QmlCompositeTypeData *typeData = 
        d->engine->d_func()->typeManager.getImmediate(data, url);
    
    if(typeData->status == QmlCompositeTypeData::Waiting) {

        d->typeData = typeData;
        d->typeData->addWaiter(d);

    } else {

        d->fromTypeData(typeData);

    }

    emit statusChanged(status());
}

/*!
    Load the QmlComponent from the provided \a url.
*/
void QmlComponent::loadUrl(const QUrl &url)
{
    Q_D(QmlComponent);

    d->clear();

    d->url = url;

    QmlCompositeTypeData *data = 
        d->engine->d_func()->typeManager.get(url);

    if(data->status == QmlCompositeTypeData::Waiting) {

        d->typeData = data;
        d->typeData->addWaiter(d);

    } else {

        d->fromTypeData(data);

    }

    emit statusChanged(status());
}

QString QmlComponent::errorDescription() const
{
    Q_D(const QmlComponent);
    if(isError())
        return d->errorDescription;
    else
        return QString();
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

    if(!context)
        context = d->engine->rootContext();

    if(context->engine() != d->engine) {
        qWarning("QmlComponent::create(): Must create component in context from the same QmlEngine");
        return 0;
    }

    QObject *rv = beginCreate(context);
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

    if(!context) {
        qWarning("QmlComponent::beginCreate(): Cannot create a component in a null context");
        return 0;
    }

    if(context->engine() != d->engine) {
        qWarning("QmlComponent::beginCreate(): Must create component in context from the same QmlEngine");
        return 0;
    }

    if (d->completePending) {
        qWarning("QmlComponent: Cannot create new component instance before completing the previous");
        return 0;
    }

    if(!isReady()) {
        qWarning("QmlComponent: Cannot create un-ready component");
        return 0;
    }

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::CreateComponent> perf;
#endif
    if(!d->engine->d_func()->rootComponent)
        d->engine->d_func()->rootComponent = this;

    QmlContext *ctxt = 
        new QmlContext(context, 0);
    static_cast<QmlContextPrivate*>(ctxt->d_ptr)->component = d->cc;
    static_cast<QmlContextPrivate*>(ctxt->d_ptr)->component->addref();
    ctxt->activate();

    QmlVME vme;
    QObject *rv = vme.run(ctxt, d->cc, d->start, d->count);
    if(vme.isError()) {
        qWarning().nospace()
#ifdef QML_VERBOSEERRORS_ENABLED
                             << "QmlComponent: "
#endif
                             << vme.errorDescription().toLatin1().constData() << " @"
                             << d->url.toString().toLatin1().constData() << ":" << vme.errorLine();
    }


    ctxt->deactivate();

    if(rv) {
        QFx_setParent_noEvent(ctxt, rv);
        QmlEnginePrivate *ep = d->engine->d_func();
        if(ep->rootComponent == this) {
            ep->rootComponent = 0;

            d->bindValues = ep->currentBindValues;
            d->parserStatus = ep->currentParserStatus;
            ep->currentBindValues.clear();
            ep->currentParserStatus.clear();
            d->completePending = true;
        }
    } else {
        delete ctxt;
    }

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
    if (d->completePending) {
        {
#ifdef Q_ENABLE_PERFORMANCE_LOG
            QFxPerfTimer<QFxPerf::BindInit> bi;
#endif
            for(int ii = 0; ii < d->bindValues.count(); ++ii)
                d->bindValues.at(ii)->init();
        }
        QSet<QmlParserStatus *> done;
        for(int ii = 0; ii < d->parserStatus.count(); ++ii) {
            QmlParserStatus *ps = d->parserStatus.at(ii);
            if(!done.contains(ps)) {
                done.insert(ps);
                ps->componentComplete();
            }
        }

        d->bindValues.clear();
        d->parserStatus.clear();
        d->completePending = false;
    }
}
QT_END_NAMESPACE
