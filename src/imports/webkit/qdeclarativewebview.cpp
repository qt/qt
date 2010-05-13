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

#include "qdeclarativewebview_p.h"
#include "qdeclarativewebview_p_p.h"

#include <qdeclarative.h>
#include <qdeclarativeengine.h>
#include <qdeclarativecontext.h>

#include <QDebug>
#include <QPen>
#include <QFile>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QBasicTimer>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebSettings>

QT_BEGIN_NAMESPACE

static const int MAX_DOUBLECLICK_TIME=500; // XXX need better gesture system

class QDeclarativeWebViewPrivate
{
public:
    QDeclarativeWebViewPrivate(QDeclarativeWebView* qq)
      : q(qq), page(0), preferredwidth(0), preferredheight(0),
            progress(1.0), status(QDeclarativeWebView::Null), pending(PendingNone),
            newWindowComponent(0), newWindowParent(0),
            pressTime(400),
            rendering(true)
    {
        QObject::connect(q, SIGNAL(focusChanged(bool)), q, SLOT(propagateFocusToWebPage(bool)));
    }

    QDeclarativeWebView *q;

    QUrl url; // page url might be different if it has not loaded yet
    QWebPage *page;
    QGraphicsWebView* view;

    int preferredwidth, preferredheight;
    qreal progress;
    QDeclarativeWebView::Status status;
    QString statusText;
    enum { PendingNone, PendingUrl, PendingHtml, PendingContent } pending;
    QUrl pending_url;
    QString pending_string;
    QByteArray pending_data;
    mutable QDeclarativeWebSettings settings;
    QDeclarativeComponent *newWindowComponent;
    QDeclarativeItem *newWindowParent;

    QBasicTimer pressTimer;
    QPoint pressPoint;
    int pressTime; // milliseconds before it's a "hold"

    static void windowObjects_append(QDeclarativeListProperty<QObject> *prop, QObject *o) {
        static_cast<QDeclarativeWebViewPrivate *>(prop->data)->windowObjects.append(o);
        static_cast<QDeclarativeWebViewPrivate *>(prop->data)->updateWindowObjects();
    }

    void updateWindowObjects();
    QObjectList windowObjects;

    bool rendering;
};

/*!
    \qmlclass WebView QDeclarativeWebView
  \since 4.7
    \brief The WebView item allows you to add web content to a canvas.
    \inherits Item

    A WebView renders web content based on a URL.

    This type is made available by importing the \c org.webkit module:

    \bold{import org.webkit 1.0}

    If the width and height of the item is not set, they will
    dynamically adjust to a size appropriate for the content.
    This width may be large for typical online web pages.

    If the width or height is explictly set, the rendered website
    will be clipped, not scaled, to fit into the set dimensions.

    If the preferredWidth is set, the width will be this amount or larger,
    usually laying out the web content to fit the preferredWidth.

    \qml
    import org.webkit 1.0

    WebView {
        url: "http://www.nokia.com"
        preferredWidth: 490
        preferredHeight: 400
        scale: 0.5
        smooth: false
        smoothCache: true
    }
    \endqml

    \image webview.png

    The item includes no scrolling, scaling,
    toolbars, etc., those must be implemented around WebView. See the WebBrowser example
    for a demonstration of this.
*/

/*!
    \internal
    \class QDeclarativeWebView
    \brief The QDeclarativeWebView class allows you to add web content to a QDeclarativeView.

    A WebView renders web content base on a URL.

    \image webview.png

    The item includes no scrolling, scaling,
    toolbars, etc., those must be implemented around WebView. See the WebBrowser example
    for a demonstration of this.

    A QDeclarativeWebView object can be instantiated in Qml using the tag \l WebView.
*/

QDeclarativeWebView::QDeclarativeWebView(QDeclarativeItem *parent)
  : QDeclarativeItem(parent)
{
    init();
}

QDeclarativeWebView::~QDeclarativeWebView()
{
    delete d->page;
    delete d;
}

void QDeclarativeWebView::init()
{
    d = new QDeclarativeWebViewPrivate(this);

    QWebSettings::enablePersistentStorage();

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    setClip(true);

    d->page = 0;
    d->view = new QGraphicsWebView(this);
    d->view->setResizesToContents(true);
    d->view->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    connect(d->view, SIGNAL(geometryChanged()), this, SLOT(updateDeclarativeWebViewSize()));
    connect(d->view, SIGNAL(scaleChanged()), this, SIGNAL(contentsScaleChanged()));
}

void QDeclarativeWebView::componentComplete()
{
    QDeclarativeItem::componentComplete();
    switch (d->pending) {
        case QDeclarativeWebViewPrivate::PendingUrl:
            setUrl(d->pending_url);
            break;
        case QDeclarativeWebViewPrivate::PendingHtml:
            setHtml(d->pending_string, d->pending_url);
            break;
        case QDeclarativeWebViewPrivate::PendingContent:
            setContent(d->pending_data, d->pending_string, d->pending_url);
            break;
        default:
            break;
    }
    d->pending = QDeclarativeWebViewPrivate::PendingNone;
    d->updateWindowObjects();
}

QDeclarativeWebView::Status QDeclarativeWebView::status() const
{
    return d->status;
}


/*!
    \qmlproperty real WebView::progress
    This property holds the progress of loading the current URL, from 0 to 1.

    If you just want to know when progress gets to 1, use
    WebView::onLoadFinished() or WebView::onLoadFailed() instead.
*/
qreal QDeclarativeWebView::progress() const
{
    return d->progress;
}

void QDeclarativeWebView::doLoadStarted()
{

    if (!d->url.isEmpty()) {
        d->status = Loading;
        emit statusChanged(d->status);
    }
    emit loadStarted();
}

void QDeclarativeWebView::doLoadProgress(int p)
{
    if (d->progress == p/100.0)
        return;
    d->progress = p/100.0;
    emit progressChanged();
}

void QDeclarativeWebView::pageUrlChanged()
{
    updateContentsSize();

    if ((d->url.isEmpty() && page()->mainFrame()->url() != QUrl(QLatin1String("about:blank")))
        || (d->url != page()->mainFrame()->url() && !page()->mainFrame()->url().isEmpty()))
    {
        d->url = page()->mainFrame()->url();
        if (d->url == QUrl(QLatin1String("about:blank")))
            d->url = QUrl();
        emit urlChanged();
    }
}

void QDeclarativeWebView::doLoadFinished(bool ok)
{

    if (title().isEmpty())
        pageUrlChanged(); // XXX bug 232556 - pages with no title never get urlChanged()

    if (ok) {
        d->status = d->url.isEmpty() ? Null : Ready;
        emit loadFinished();
    } else {
        d->status = Error;
        emit loadFailed();
    }
    emit statusChanged(d->status);
}

/*!
    \qmlproperty url WebView::url
    This property holds the URL to the page displayed in this item. It can be set,
    but also can change spontaneously (eg. because of network redirection).

    If the url is empty, the page is blank.

    The url is always absolute (QML will resolve relative URL strings in the context
    of the containing QML document).
*/
QUrl QDeclarativeWebView::url() const
{
    return d->url;
}

void QDeclarativeWebView::setUrl(const QUrl &url)
{
    if (url == d->url)
        return;

    if (isComponentComplete()) {
        d->url = url;
        updateContentsSize();
        QUrl seturl = url;
        if (seturl.isEmpty())
            seturl = QUrl(QLatin1String("about:blank"));

        Q_ASSERT(!seturl.isRelative());

        page()->mainFrame()->load(seturl);

        emit urlChanged();
    } else {
        d->pending = d->PendingUrl;
        d->pending_url = url;
    }
}

/*!
    \qmlproperty int WebView::preferredWidth
    This property holds the ideal width for displaying the current URL.
*/
int QDeclarativeWebView::preferredWidth() const
{
    return d->preferredwidth;
}

void QDeclarativeWebView::setPreferredWidth(int iw)
{
    if (d->preferredwidth == iw) return;
    d->preferredwidth = iw;
    updateContentsSize();
    emit preferredWidthChanged();
}

/*!
    \qmlproperty int WebView::preferredHeight
    This property holds the ideal height for displaying the current URL.
    This only affects the area zoomed by heuristicZoom().
*/
int QDeclarativeWebView::preferredHeight() const
{
    return d->preferredheight;
}

void QDeclarativeWebView::setPreferredHeight(int ih)
{
    if (d->preferredheight == ih) return;
    d->preferredheight = ih;
    updateContentsSize();
    emit preferredHeightChanged();
}

/*!
    \qmlmethod bool WebView::evaluateJavaScript(string)

    Evaluates the \a scriptSource JavaScript inside the context of the
    main web frame, and returns the result of the last executed statement.

    Note that this JavaScript does \e not have any access to QML objects
    except as made available as windowObjects.
*/
QVariant QDeclarativeWebView::evaluateJavaScript(const QString &scriptSource)
{
    return this->page()->mainFrame()->evaluateJavaScript(scriptSource);
}

void QDeclarativeWebView::propagateFocusToWebPage(bool hasFocus)
{
    QFocusEvent e(hasFocus ? QEvent::FocusIn : QEvent::FocusOut);
    page()->event(&e);
}

void QDeclarativeWebView::updateDeclarativeWebViewSize()
{
    QSizeF size = d->view->geometry().size() * contentsScale();
    setImplicitWidth(size.width());
    setImplicitHeight(size.height());
}

void QDeclarativeWebView::initialLayout()
{
    // nothing useful to do at this point
}

void QDeclarativeWebView::updateContentsSize()
{
    if (d->page)
        d->page->setPreferredContentsSize(QSize(
            d->preferredwidth>0 ? d->preferredwidth : width(),
            d->preferredheight>0 ? d->preferredheight : height()));
}

void QDeclarativeWebView::geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry)
{
    if (newGeometry.size() != oldGeometry.size() && d->page) {
        QSize cs = d->page->preferredContentsSize();
        if (widthValid())
            cs.setWidth(width());
        if (heightValid())
            cs.setHeight(height());
        if (cs != d->page->preferredContentsSize())
            d->page->setPreferredContentsSize(cs);
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

/*!
    \qmlproperty list<object> WebView::javaScriptWindowObjects

    A list of QML objects to expose to the web page.

    Each object will be added as a property of the web frame's window object.  The
    property name is controlled by the value of \c WebView.windowObjectName 
    attached property.

    Exposing QML objects to a web page allows JavaScript executing in the web 
    page itself to communicate with QML, by reading and writing properties and
    by calling methods of the exposed QML objects.

    This example shows how to call into a QML method using a window object.

    \qml
    WebView {
        javaScriptWindowObjects: QtObject {
            WebView.windowObjectName: "qml"

            function qmlCall() { 
                console.log("This call is in QML!");
            }
        }

        html: "<script>console.log(\"This is in WebKit!\"); window.qml.qmlCall();</script>"
    }
    \endqml

    The output of the example will be:
    \code
    This is in WebKit!
    This call is in QML!
    \endcode

    If Javascript is not enabled for the page, then this property does nothing.
*/
QDeclarativeListProperty<QObject> QDeclarativeWebView::javaScriptWindowObjects()
{
    return QDeclarativeListProperty<QObject>(this, d, &QDeclarativeWebViewPrivate::windowObjects_append);
}

QDeclarativeWebViewAttached *QDeclarativeWebView::qmlAttachedProperties(QObject *o)
{
    return new QDeclarativeWebViewAttached(o);
}

void QDeclarativeWebViewPrivate::updateWindowObjects()
{
    if (!q->isComponentCompletePublic() || !page)
        return;

    for (int ii = 0; ii < windowObjects.count(); ++ii) {
        QObject *object = windowObjects.at(ii);
        QDeclarativeWebViewAttached *attached = static_cast<QDeclarativeWebViewAttached *>(qmlAttachedPropertiesObject<QDeclarativeWebView>(object));
        if (attached && !attached->windowObjectName().isEmpty()) {
            page->mainFrame()->addToJavaScriptWindowObject(attached->windowObjectName(), object);
        }
    }
}

bool QDeclarativeWebView::renderingEnabled() const
{
    return d->rendering;
}

void QDeclarativeWebView::setRenderingEnabled(bool enabled)
{
    if (d->rendering == enabled)
        return;
    d->rendering = enabled;
    emit renderingEnabledChanged();

    d->view->setTiledBackingStoreFrozen(!enabled);
}

QMouseEvent *QDeclarativeWebView::sceneMouseEventToMouseEvent(QGraphicsSceneMouseEvent *e)
{
    QEvent::Type t;
    switch(e->type()) {
    default:
    case QEvent::GraphicsSceneMousePress:
        t = QEvent::MouseButtonPress;
        break;
    case QEvent::GraphicsSceneMouseRelease:
        t = QEvent::MouseButtonRelease;
        break;
    case QEvent::GraphicsSceneMouseMove:
        t = QEvent::MouseMove;
        break;
    case QGraphicsSceneEvent::GraphicsSceneMouseDoubleClick:
        t = QEvent::MouseButtonDblClick;
        break;
    }

    QMouseEvent *me = new QMouseEvent(t, (e->pos()/contentsScale()).toPoint(), e->button(), e->buttons(), 0);
    return me;
}

QMouseEvent *QDeclarativeWebView::sceneHoverMoveEventToMouseEvent(QGraphicsSceneHoverEvent *e)
{
    QEvent::Type t = QEvent::MouseMove;

    QMouseEvent *me = new QMouseEvent(t, (e->pos()/contentsScale()).toPoint(), Qt::NoButton, Qt::NoButton, 0);

    return me;
}

/*!
    \qmlsignal WebView::onDoubleClick(clickx,clicky)

    The WebView does not pass double-click events to the web engine, but rather
    emits this signals.
*/

void QDeclarativeWebView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);
    emit doubleClick(me->x(),me->y());
    delete me;
}

/*!
    \qmlmethod bool WebView::heuristicZoom(clickX,clickY,maxzoom)

    Finds a zoom that:
    \list
    \i shows a whole item
    \i includes (\a clickX, \a clickY)
    \i fits into the preferredWidth and preferredHeight
    \i zooms by no more than \a maxzoom
    \i is more than 10% above the current zoom
    \endlist

    If such a zoom exists, emits zoomTo(zoom,centerX,centerY) and returns true; otherwise,
    no signal is emitted and returns false.
*/
bool QDeclarativeWebView::heuristicZoom(int clickX, int clickY, qreal maxzoom)
{
    if (contentsScale() >= maxzoom/zoomFactor())
        return false;
    qreal ozf = contentsScale();
    QRect showarea = elementAreaAt(clickX, clickY, d->preferredwidth/maxzoom, d->preferredheight/maxzoom);
    qreal z = qMin(qreal(d->preferredwidth)/showarea.width(),qreal(d->preferredheight)/showarea.height());
    if (z > maxzoom/zoomFactor())
        z = maxzoom/zoomFactor();
    if (z/ozf > 1.2) {
        QRectF r(showarea.left()*z, showarea.top()*z, showarea.width()*z, showarea.height()*z);
        emit zoomTo(z,r.x()+r.width()/2, r.y()+r.height()/2);
        return true;
    } else {
        return false;
    }
}

/*!
    \qmlproperty int WebView::pressGrabTime

    The number of milliseconds the user must press before the WebView
    starts passing move events through to the web engine (rather than
    letting other QML elements such as a Flickable take them).

    Defaults to 400ms. Set to 0 to always grab and pass move events to
    the web engine.
*/
int QDeclarativeWebView::pressGrabTime() const
{
    return d->pressTime;
}

void QDeclarativeWebView::setPressGrabTime(int ms)
{
    if (d->pressTime == ms) 
        return;
    d->pressTime = ms;
    emit pressGrabTimeChanged();
}

void QDeclarativeWebView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus (true);
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);

    d->pressPoint = me->pos();
    if (d->pressTime) {
        d->pressTimer.start(d->pressTime,this);
        setKeepMouseGrab(false);
    } else {
        grabMouse();
        setKeepMouseGrab(true);
    }

    page()->event(me);
    event->setAccepted(
/*
  It is not correct to send the press event upwards, if it is not accepted by WebKit
  e.g. push button does not work, if done so as QGraphicsScene will not send the release event at all to WebKit
  Might be a bug in WebKit, though
  */
#if 1 //QT_VERSION <= 0x040500 // XXX see bug 230835
        true
#else
        me->isAccepted()
#endif
        );
    delete me;
    if (!event->isAccepted()) {
        QDeclarativeItem::mousePressEvent(event);
    }
}

void QDeclarativeWebView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);
    page()->event(me);
    d->pressTimer.stop();
    event->setAccepted(
/*
  It is not correct to send the press event upwards, if it is not accepted by WebKit
  e.g. push button does not work, if done so as QGraphicsScene will not send all the events to WebKit
  */
#if 1 //QT_VERSION <= 0x040500 // XXX see bug 230835
        true
#else
        me->isAccepted()
#endif
        );
    delete me;
    if (!event->isAccepted()) {
        QDeclarativeItem::mouseReleaseEvent(event);
    }
    setKeepMouseGrab(false);
    ungrabMouse();
}

void QDeclarativeWebView::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == d->pressTimer.timerId()) {
        d->pressTimer.stop();
        grabMouse();
        setKeepMouseGrab(true);
    }
}

void QDeclarativeWebView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);
    if (d->pressTimer.isActive()) {
        if ((me->pos() - d->pressPoint).manhattanLength() > QApplication::startDragDistance())  {
            d->pressTimer.stop();
        }
    }
    if (keepMouseGrab()) {
        page()->event(me);
        event->setAccepted(
/*
  It is not correct to send the press event upwards, if it is not accepted by WebKit
  e.g. push button does not work, if done so as QGraphicsScene will not send the release event at all to WebKit
  Might be a bug in WebKit, though
  */
#if 1 // QT_VERSION <= 0x040500 // XXX see bug 230835
            true
#else
            me->isAccepted()
#endif
        );
    }
    delete me;
    if (!event->isAccepted())
        QDeclarativeItem::mouseMoveEvent(event);
}

void QDeclarativeWebView::hoverMoveEvent (QGraphicsSceneHoverEvent * event)
{
    QMouseEvent *me = sceneHoverMoveEventToMouseEvent(event);
    page()->event(me);
    event->setAccepted(
#if QT_VERSION <= 0x040500 // XXX see bug 230835
        true
#else
        me->isAccepted()
#endif
    );
    delete me;
    if (!event->isAccepted())
        QDeclarativeItem::hoverMoveEvent(event);
}

bool QDeclarativeWebView::sceneEvent(QEvent *event) 
{ 
    if (event->type() == QEvent::KeyPress) { 
        QKeyEvent *k = static_cast<QKeyEvent *>(event); 
        if (k->key() == Qt::Key_Tab || k->key() == Qt::Key_Backtab) { 
            if (!(k->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) { //### Add MetaModifier? 
                page()->event(event); 
                if (event->isAccepted()) 
                    return true; 
            } 
        } 
    } 
    return QDeclarativeItem::sceneEvent(event);
} 


/*!
    \qmlproperty action WebView::back
    This property holds the action for causing the previous URL in the history to be displayed.
*/
QAction *QDeclarativeWebView::backAction() const
{
    return page()->action(QWebPage::Back);
}

/*!
    \qmlproperty action WebView::forward
    This property holds the action for causing the next URL in the history to be displayed.
*/
QAction *QDeclarativeWebView::forwardAction() const
{
    return page()->action(QWebPage::Forward);
}

/*!
    \qmlproperty action WebView::reload
    This property holds the action for reloading with the current URL
*/
QAction *QDeclarativeWebView::reloadAction() const
{
    return page()->action(QWebPage::Reload);
}

/*!
    \qmlproperty action WebView::stop
    This property holds the action for stopping loading with the current URL
*/
QAction *QDeclarativeWebView::stopAction() const
{
    return page()->action(QWebPage::Stop);
}

/*!
    \qmlproperty real WebView::title
    This property holds the title of the web page currently viewed

    By default, this property contains an empty string.
*/
QString QDeclarativeWebView::title() const
{
    return page()->mainFrame()->title();
}



/*!
    \qmlproperty pixmap WebView::icon
    This property holds the icon associated with the web page currently viewed
*/
QPixmap QDeclarativeWebView::icon() const
{
    return page()->mainFrame()->icon().pixmap(QSize(256,256));
}


/*!
    \qmlproperty real WebView::zoomFactor
    This property holds the multiplier used to scale the contents of a Web page.
*/
void QDeclarativeWebView::setZoomFactor(qreal factor)
{
    if (factor == page()->mainFrame()->zoomFactor())
        return;

    page()->mainFrame()->setZoomFactor(factor);
    updateContentsSize();

    emit zoomFactorChanged();
}

qreal QDeclarativeWebView::zoomFactor() const
{
    return page()->mainFrame()->zoomFactor();
}

/*!
    \qmlproperty string WebView::statusText

    This property is the current status suggested by the current web page. In a web browser,
    such status is often shown in some kind of status bar.
*/
void QDeclarativeWebView::setStatusText(const QString& s)
{
    d->statusText = s;
    emit statusTextChanged();
}

void QDeclarativeWebView::windowObjectCleared()
{
    d->updateWindowObjects();
}

QString QDeclarativeWebView::statusText() const
{
    return d->statusText;
}

QWebPage *QDeclarativeWebView::page() const
{

    if (!d->page) {
        QDeclarativeWebView *self = const_cast<QDeclarativeWebView*>(this);
        QWebPage *wp = new QDeclarativeWebPage(self);

        wp->setNetworkAccessManager(qmlEngine(this)->networkAccessManager());

        self->setPage(wp);

        return wp;
    }

    return d->page;
}


// The QObject interface to settings().
/*!
    \qmlproperty string WebView::settings.standardFontFamily
    \qmlproperty string WebView::settings.fixedFontFamily
    \qmlproperty string WebView::settings.serifFontFamily
    \qmlproperty string WebView::settings.sansSerifFontFamily
    \qmlproperty string WebView::settings.cursiveFontFamily
    \qmlproperty string WebView::settings.fantasyFontFamily

    \qmlproperty int WebView::settings.minimumFontSize
    \qmlproperty int WebView::settings.minimumLogicalFontSize
    \qmlproperty int WebView::settings.defaultFontSize
    \qmlproperty int WebView::settings.defaultFixedFontSize

    \qmlproperty bool WebView::settings.autoLoadImages
    \qmlproperty bool WebView::settings.javascriptEnabled
    \qmlproperty bool WebView::settings.javaEnabled
    \qmlproperty bool WebView::settings.pluginsEnabled
    \qmlproperty bool WebView::settings.privateBrowsingEnabled
    \qmlproperty bool WebView::settings.javascriptCanOpenWindows
    \qmlproperty bool WebView::settings.javascriptCanAccessClipboard
    \qmlproperty bool WebView::settings.developerExtrasEnabled
    \qmlproperty bool WebView::settings.linksIncludedInFocusChain
    \qmlproperty bool WebView::settings.zoomTextOnly
    \qmlproperty bool WebView::settings.printElementBackgrounds
    \qmlproperty bool WebView::settings.offlineStorageDatabaseEnabled
    \qmlproperty bool WebView::settings.offlineWebApplicationCacheEnabled
    \qmlproperty bool WebView::settings.localStorageDatabaseEnabled
    \qmlproperty bool WebView::settings.localContentCanAccessRemoteUrls

    These properties give access to the settings controlling the web view.

    See QWebSettings for details of these properties.

    \qml
        WebView {
            settings.pluginsEnabled: true
            settings.standardFontFamily: "Arial"
            ...
        }
    \endqml
*/
QDeclarativeWebSettings *QDeclarativeWebView::settingsObject() const
{
    d->settings.s = page()->settings();
    return &d->settings;
}

void QDeclarativeWebView::setPage(QWebPage *page)
{
    if (d->page == page)
        return;
    if (d->page) {
        if (d->page->parent() == this) {
            delete d->page;
        } else {
            d->page->disconnect(this);
        }
    }
    d->page = page;
    updateContentsSize();
    d->page->mainFrame()->setScrollBarPolicy(Qt::Horizontal,Qt::ScrollBarAlwaysOff);
    d->page->mainFrame()->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
    connect(d->page->mainFrame(),SIGNAL(urlChanged(QUrl)),this,SLOT(pageUrlChanged()));
    connect(d->page->mainFrame(), SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    connect(d->page->mainFrame(), SIGNAL(titleChanged(QString)), this, SIGNAL(iconChanged()));
    connect(d->page->mainFrame(), SIGNAL(iconChanged()), this, SIGNAL(iconChanged()));
    connect(d->page->mainFrame(), SIGNAL(initialLayoutCompleted()), this, SLOT(initialLayout()));
    connect(d->page->mainFrame(), SIGNAL(contentsSizeChanged(QSize)), this, SIGNAL(contentsSizeChanged(QSize)));

    connect(d->page,SIGNAL(loadStarted()),this,SLOT(doLoadStarted()));
    connect(d->page,SIGNAL(loadProgress(int)),this,SLOT(doLoadProgress(int)));
    connect(d->page,SIGNAL(loadFinished(bool)),this,SLOT(doLoadFinished(bool)));
    connect(d->page,SIGNAL(statusBarMessage(QString)),this,SLOT(setStatusText(QString)));

    connect(d->page->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(windowObjectCleared()));

    d->page->settings()->setAttribute(QWebSettings::TiledBackingStoreEnabled, true);

    d->view->setPage(page);
}

/*!
    \qmlsignal WebView::onLoadStarted()

    This handler is called when the web engine begins loading
    a page. Later, WebView::onLoadFinished() or WebView::onLoadFailed()
    will be emitted.
*/

/*!
    \qmlsignal WebView::onLoadFinished()

    This handler is called when the web engine \e successfully
    finishes loading a page, including any component content
    (WebView::onLoadFailed() will be emitted otherwise).

    \sa progress
*/

/*!
    \qmlsignal WebView::onLoadFailed()

    This handler is called when the web engine fails loading
    a page or any component content
    (WebView::onLoadFinished() will be emitted on success).
*/

void QDeclarativeWebView::load(const QNetworkRequest &request,
          QNetworkAccessManager::Operation operation,
          const QByteArray &body)
{
    page()->mainFrame()->load(request, operation, body);
}

QString QDeclarativeWebView::html() const
{
    return page()->mainFrame()->toHtml();
}

/*!
    \qmlproperty string WebView::html
    This property holds HTML text set directly

    The html property can be set as a string.

    \qml
    WebView {
        html: "<p>This is <b>HTML</b>."
    }
    \endqml
*/
void QDeclarativeWebView::setHtml(const QString &html, const QUrl &baseUrl)
{
    updateContentsSize();
    if (isComponentComplete())
        page()->mainFrame()->setHtml(html, baseUrl);
    else {
        d->pending = d->PendingHtml;
        d->pending_url = baseUrl;
        d->pending_string = html;
    }
    emit htmlChanged();
}

void QDeclarativeWebView::setContent(const QByteArray &data, const QString &mimeType, const QUrl &baseUrl)
{
    updateContentsSize();

    if (isComponentComplete())
        page()->mainFrame()->setContent(data,mimeType,qmlContext(this)->resolvedUrl(baseUrl));
    else {
        d->pending = d->PendingContent;
        d->pending_url = baseUrl;
        d->pending_string = mimeType;
        d->pending_data = data;
    }
}

QWebHistory *QDeclarativeWebView::history() const
{
    return page()->history();
}

QWebSettings *QDeclarativeWebView::settings() const
{
    return page()->settings();
}

QDeclarativeWebView *QDeclarativeWebView::createWindow(QWebPage::WebWindowType type)
{
    switch (type) {
        case QWebPage::WebBrowserWindow: {
            if (!d->newWindowComponent && d->newWindowParent)
                qWarning("WebView::newWindowComponent not set - WebView::newWindowParent ignored");
            else if (d->newWindowComponent && !d->newWindowParent)
                qWarning("WebView::newWindowParent not set - WebView::newWindowComponent ignored");
            else if (d->newWindowComponent && d->newWindowParent) {
                QDeclarativeWebView *webview = 0;
                QDeclarativeContext *windowContext = new QDeclarativeContext(qmlContext(this));

                QObject *nobj = d->newWindowComponent->create(windowContext);
                if (nobj) {
                    windowContext->setParent(nobj);
                    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(nobj);
                    if (!item) {
                        delete nobj;
                    } else {
                        webview = item->findChild<QDeclarativeWebView*>();
                        if (!webview) {
                            delete item;
                        } else {
                            nobj->setParent(d->newWindowParent);
                            static_cast<QGraphicsObject*>(item)->setParentItem(d->newWindowParent);
                        }
                    }
                } else {
                    delete windowContext;
                }

                return webview;
            }
        }
        break;
        case QWebPage::WebModalDialog: {
            // Not supported
        }
    }
    return 0;
}

/*!
    \qmlproperty component WebView::newWindowComponent

    This property holds the component to use for new windows.
    The component must have a WebView somewhere in its structure.

    When the web engine requests a new window, it will be an instance of
    this component.

    The parent of the new window is set by newWindowParent. It must be set.
*/
QDeclarativeComponent *QDeclarativeWebView::newWindowComponent() const
{
    return d->newWindowComponent;
}

void QDeclarativeWebView::setNewWindowComponent(QDeclarativeComponent *newWindow)
{
    if (newWindow == d->newWindowComponent)
        return;
    d->newWindowComponent = newWindow;
    emit newWindowComponentChanged();
}


/*!
    \qmlproperty item WebView::newWindowParent

    The parent item for new windows.

    \sa newWindowComponent
*/
QDeclarativeItem *QDeclarativeWebView::newWindowParent() const
{
    return d->newWindowParent;
}

void QDeclarativeWebView::setNewWindowParent(QDeclarativeItem *parent)
{
    if (parent == d->newWindowParent)
        return;
    if (d->newWindowParent && parent) {
        QList<QGraphicsItem *> children = d->newWindowParent->childItems();
        for (int i = 0; i < children.count(); ++i) {
            children.at(i)->setParentItem(parent);
        }
    }
    d->newWindowParent = parent;
    emit newWindowParentChanged();    
}

QSize QDeclarativeWebView::contentsSize() const
{
    return d->page->mainFrame()->contentsSize() * contentsScale();
}

qreal QDeclarativeWebView::contentsScale() const
{
    return d->view->scale();
}

void QDeclarativeWebView::setContentsScale(qreal scale)
{
    if (scale == d->view->scale())
        return;
    d->view->setScale(scale);
    updateDeclarativeWebViewSize();
    emit contentsScaleChanged();
}

/*!
    Returns the area of the largest element at position (\a x,\a y) that is no larger
    than \a maxwidth by \a maxheight pixels.

    May return an area larger in the case when no smaller element is at the position.
*/
QRect QDeclarativeWebView::elementAreaAt(int x, int y, int maxwidth, int maxheight) const
{
    QWebHitTestResult hit = page()->mainFrame()->hitTestContent(QPoint(x,y));
    QRect rv = hit.boundingRect();
    QWebElement element = hit.enclosingBlockElement();
    if (maxwidth<=0) maxwidth = INT_MAX;
    if (maxheight<=0) maxheight = INT_MAX;
    while (!element.parent().isNull() && element.geometry().width() <= maxwidth && element.geometry().height() <= maxheight) {
        rv = element.geometry();
        element = element.parent();
    }
    return rv;
}

/*!
    \internal
    \class QDeclarativeWebPage
    \brief The QDeclarativeWebPage class is a QWebPage that can create QML plugins.

    \sa QDeclarativeWebView
*/
QDeclarativeWebPage::QDeclarativeWebPage(QDeclarativeWebView *parent) :
    QWebPage(parent)
{
}

QDeclarativeWebPage::~QDeclarativeWebPage()
{
}

void QDeclarativeWebPage::javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
{
    qWarning() << sourceID << ':' << lineNumber << ':' << message;
}

QString QDeclarativeWebPage::chooseFile(QWebFrame *originatingFrame, const QString& oldFile)
{
    // Not supported (it's modal)
    Q_UNUSED(originatingFrame)
    Q_UNUSED(oldFile)
    return oldFile;
}

/*!
    \qmlsignal WebView::alert(message)

    This signal is emitted when the web engine sends a JavaScript alert. The \a message is the text
    to be displayed in the alert to the user.
*/


void QDeclarativeWebPage::javaScriptAlert(QWebFrame *originatingFrame, const QString& msg)
{
    Q_UNUSED(originatingFrame)
    emit viewItem()->alert(msg);
}

bool QDeclarativeWebPage::javaScriptConfirm(QWebFrame *originatingFrame, const QString& msg)
{
    // Not supported (it's modal)
    Q_UNUSED(originatingFrame)
    Q_UNUSED(msg)
    return false;
}

bool QDeclarativeWebPage::javaScriptPrompt(QWebFrame *originatingFrame, const QString& msg, const QString& defaultValue, QString* result)
{
    // Not supported (it's modal)
    Q_UNUSED(originatingFrame)
    Q_UNUSED(msg)
    Q_UNUSED(defaultValue)
    Q_UNUSED(result)
    return false;
}


QDeclarativeWebView *QDeclarativeWebPage::viewItem()
{
    return static_cast<QDeclarativeWebView*>(parent());
}

QWebPage *QDeclarativeWebPage::createWindow(WebWindowType type)
{
    QDeclarativeWebView *newView = viewItem()->createWindow(type);
    if (newView)
        return newView->page();
    return 0;
}

QT_END_NAMESPACE

