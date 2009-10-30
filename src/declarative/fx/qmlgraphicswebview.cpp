/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qml.h"
#include "qmlengine.h"
#include <private/qmlstate_p.h>
#include <private/qlistmodelinterface_p.h>

#include "qmlgraphicswebview_p.h"
#include <private/qmlgraphicspainteditem_p_p.h>

QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,WebView,QmlGraphicsWebView)
QML_DEFINE_NOCREATE_TYPE(QAction)

static const int MAX_DOUBLECLICK_TIME=500; // XXX need better gesture system

class QmlGraphicsWebSettings : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString standardFontFamily READ standardFontFamily WRITE setStandardFontFamily)
    Q_PROPERTY(QString fixedFontFamily READ fixedFontFamily WRITE setFixedFontFamily)
    Q_PROPERTY(QString serifFontFamily READ serifFontFamily WRITE setSerifFontFamily)
    Q_PROPERTY(QString sansSerifFontFamily READ sansSerifFontFamily WRITE setSansSerifFontFamily)
    Q_PROPERTY(QString cursiveFontFamily READ cursiveFontFamily WRITE setCursiveFontFamily)
    Q_PROPERTY(QString fantasyFontFamily READ fantasyFontFamily WRITE setFantasyFontFamily)

    Q_PROPERTY(int minimumFontSize READ minimumFontSize WRITE setMinimumFontSize)
    Q_PROPERTY(int minimumLogicalFontSize READ minimumLogicalFontSize WRITE setMinimumLogicalFontSize)
    Q_PROPERTY(int defaultFontSize READ defaultFontSize WRITE setDefaultFontSize)
    Q_PROPERTY(int defaultFixedFontSize READ defaultFixedFontSize WRITE setDefaultFixedFontSize)

    Q_PROPERTY(bool autoLoadImages READ autoLoadImages WRITE setAutoLoadImages)
    Q_PROPERTY(bool javascriptEnabled READ javascriptEnabled WRITE setJavascriptEnabled)
    Q_PROPERTY(bool javaEnabled READ javaEnabled WRITE setJavaEnabled)
    Q_PROPERTY(bool pluginsEnabled READ pluginsEnabled WRITE setPluginsEnabled)
    Q_PROPERTY(bool privateBrowsingEnabled READ privateBrowsingEnabled WRITE setPrivateBrowsingEnabled)
    Q_PROPERTY(bool javascriptCanOpenWindows READ javascriptCanOpenWindows WRITE setJavascriptCanOpenWindows)
    Q_PROPERTY(bool javascriptCanAccessClipboard READ javascriptCanAccessClipboard WRITE setJavascriptCanAccessClipboard)
    Q_PROPERTY(bool developerExtrasEnabled READ developerExtrasEnabled WRITE setDeveloperExtrasEnabled)
    Q_PROPERTY(bool linksIncludedInFocusChain READ linksIncludedInFocusChain WRITE setLinksIncludedInFocusChain)
    Q_PROPERTY(bool zoomTextOnly READ zoomTextOnly WRITE setZoomTextOnly)
    Q_PROPERTY(bool printElementBackgrounds READ printElementBackgrounds WRITE setPrintElementBackgrounds)
    Q_PROPERTY(bool offlineStorageDatabaseEnabled READ offlineStorageDatabaseEnabled WRITE setOfflineStorageDatabaseEnabled)
    Q_PROPERTY(bool offlineWebApplicationCacheEnabled READ offlineWebApplicationCacheEnabled WRITE setOfflineWebApplicationCacheEnabled)
    Q_PROPERTY(bool localStorageDatabaseEnabled READ localStorageDatabaseEnabled WRITE setLocalStorageDatabaseEnabled)
    Q_PROPERTY(bool localContentCanAccessRemoteUrls READ localContentCanAccessRemoteUrls WRITE setLocalContentCanAccessRemoteUrls)

public:
    QmlGraphicsWebSettings() {}

    QString standardFontFamily() const { return s->fontFamily(QWebSettings::StandardFont); }
    void setStandardFontFamily(const QString& f) { s->setFontFamily(QWebSettings::StandardFont,f); }
    QString fixedFontFamily() const { return s->fontFamily(QWebSettings::FixedFont); }
    void setFixedFontFamily(const QString& f) { s->setFontFamily(QWebSettings::FixedFont,f); }
    QString serifFontFamily() const { return s->fontFamily(QWebSettings::SerifFont); }
    void setSerifFontFamily(const QString& f) { s->setFontFamily(QWebSettings::SerifFont,f); }
    QString sansSerifFontFamily() const { return s->fontFamily(QWebSettings::SansSerifFont); }
    void setSansSerifFontFamily(const QString& f) { s->setFontFamily(QWebSettings::SansSerifFont,f); }
    QString cursiveFontFamily() const { return s->fontFamily(QWebSettings::CursiveFont); }
    void setCursiveFontFamily(const QString& f) { s->setFontFamily(QWebSettings::CursiveFont,f); }
    QString fantasyFontFamily() const { return s->fontFamily(QWebSettings::FantasyFont); }
    void setFantasyFontFamily(const QString& f) { s->setFontFamily(QWebSettings::FantasyFont,f); }

    int minimumFontSize() const { return s->fontSize(QWebSettings::MinimumFontSize); }
    void setMinimumFontSize(int size) { s->setFontSize(QWebSettings::MinimumFontSize,size); }
    int minimumLogicalFontSize() const { return s->fontSize(QWebSettings::MinimumLogicalFontSize); }
    void setMinimumLogicalFontSize(int size) { s->setFontSize(QWebSettings::MinimumLogicalFontSize,size); }
    int defaultFontSize() const { return s->fontSize(QWebSettings::DefaultFontSize); }
    void setDefaultFontSize(int size) { s->setFontSize(QWebSettings::DefaultFontSize,size); }
    int defaultFixedFontSize() const { return s->fontSize(QWebSettings::DefaultFixedFontSize); }
    void setDefaultFixedFontSize(int size) { s->setFontSize(QWebSettings::DefaultFixedFontSize,size); }

    bool autoLoadImages() const { return s->testAttribute(QWebSettings::AutoLoadImages); }
    void setAutoLoadImages(bool on) { s->setAttribute(QWebSettings::AutoLoadImages, on); }
    bool javascriptEnabled() const { return s->testAttribute(QWebSettings::JavascriptEnabled); }
    void setJavascriptEnabled(bool on) { s->setAttribute(QWebSettings::JavascriptEnabled, on); }
    bool javaEnabled() const { return s->testAttribute(QWebSettings::JavaEnabled); }
    void setJavaEnabled(bool on) { s->setAttribute(QWebSettings::JavaEnabled, on); }
    bool pluginsEnabled() const { return s->testAttribute(QWebSettings::PluginsEnabled); }
    void setPluginsEnabled(bool on) { s->setAttribute(QWebSettings::PluginsEnabled, on); }
    bool privateBrowsingEnabled() const { return s->testAttribute(QWebSettings::PrivateBrowsingEnabled); }
    void setPrivateBrowsingEnabled(bool on) { s->setAttribute(QWebSettings::PrivateBrowsingEnabled, on); }
    bool javascriptCanOpenWindows() const { return s->testAttribute(QWebSettings::JavascriptCanOpenWindows); }
    void setJavascriptCanOpenWindows(bool on) { s->setAttribute(QWebSettings::JavascriptCanOpenWindows, on); }
    bool javascriptCanAccessClipboard() const { return s->testAttribute(QWebSettings::JavascriptCanAccessClipboard); }
    void setJavascriptCanAccessClipboard(bool on) { s->setAttribute(QWebSettings::JavascriptCanAccessClipboard, on); }
    bool developerExtrasEnabled() const { return s->testAttribute(QWebSettings::DeveloperExtrasEnabled); }
    void setDeveloperExtrasEnabled(bool on) { s->setAttribute(QWebSettings::DeveloperExtrasEnabled, on); }
    bool linksIncludedInFocusChain() const { return s->testAttribute(QWebSettings::LinksIncludedInFocusChain); }
    void setLinksIncludedInFocusChain(bool on) { s->setAttribute(QWebSettings::LinksIncludedInFocusChain, on); }
    bool zoomTextOnly() const { return s->testAttribute(QWebSettings::ZoomTextOnly); }
    void setZoomTextOnly(bool on) { s->setAttribute(QWebSettings::ZoomTextOnly, on); }
    bool printElementBackgrounds() const { return s->testAttribute(QWebSettings::PrintElementBackgrounds); }
    void setPrintElementBackgrounds(bool on) { s->setAttribute(QWebSettings::PrintElementBackgrounds, on); }
    bool offlineStorageDatabaseEnabled() const { return s->testAttribute(QWebSettings::OfflineStorageDatabaseEnabled); }
    void setOfflineStorageDatabaseEnabled(bool on) { s->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, on); }
    bool offlineWebApplicationCacheEnabled() const { return s->testAttribute(QWebSettings::OfflineWebApplicationCacheEnabled); }
    void setOfflineWebApplicationCacheEnabled(bool on) { s->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, on); }
    bool localStorageDatabaseEnabled() const { return s->testAttribute(QWebSettings::LocalStorageDatabaseEnabled); }
    void setLocalStorageDatabaseEnabled(bool on) { s->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, on); }
    bool localContentCanAccessRemoteUrls() const { return s->testAttribute(QWebSettings::LocalContentCanAccessRemoteUrls); }
    void setLocalContentCanAccessRemoteUrls(bool on) { s->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, on); }

    QWebSettings *s;
};

QML_DECLARE_TYPE(QmlGraphicsWebSettings)
QML_DEFINE_NOCREATE_TYPE(QmlGraphicsWebSettings)

class QmlGraphicsWebViewPrivate : public QmlGraphicsPaintedItemPrivate
{
    Q_DECLARE_PUBLIC(QmlGraphicsWebView)

public:
    QmlGraphicsWebViewPrivate()
      : QmlGraphicsPaintedItemPrivate(), page(0), preferredwidth(0), pagewidth(0),
            progress(1.0), status(QmlGraphicsWebView::Null), pending(PendingNone),
            newWindowComponent(0), newWindowParent(0),
            windowObjects(this),
            rendering(true)
    {
    }

    QUrl url; // page url might be different if it has not loaded yet
    QWebPage *page;

    int preferredwidth;
    int pagewidth;
    qreal progress;
    QmlGraphicsWebView::Status status;
    QString statusText;
    enum { PendingNone, PendingUrl, PendingHtml, PendingContent } pending;
    QUrl pending_url;
    QString pending_string;
    QByteArray pending_data;
    mutable QmlGraphicsWebSettings settings;
    QmlComponent *newWindowComponent;
    QmlGraphicsItem *newWindowParent;

    void updateWindowObjects();
    class WindowObjectList : public QmlConcreteList<QObject *>
    {
    public:
        WindowObjectList(QmlGraphicsWebViewPrivate *p)
            : priv(p) {}
        virtual void append(QObject *v) {
            QmlConcreteList<QObject *>::append(v);
            priv->updateWindowObjects();
        }
    private:
        QmlGraphicsWebViewPrivate *priv;
    } windowObjects;

    bool rendering;
};

/*!
    \qmlclass WebView QFxWebView
    \brief The WebView item allows you to add web content to a canvas.
    \inherits Item

    A WebView renders web content based on a URL.

    If the width and height of the item is not set, they will
    dynamically adjust to a size appropriate for the content.
    This width may be large for typical online web pages.

    If the preferredWidth is set, the width will be this amount or larger,
    usually laying out the web content to fit the preferredWidth.

    \qml
    WebView {
        url: "http://www.nokia.com"
        width: 490
        height: 400
        scale: 0.5
        smooth: true
    }
    \endqml

    \image webview.png

    The item includes no scrolling, scaling,
    toolbars, etc., those must be implemented around WebView. See the WebBrowser example
    for a demonstration of this.
*/

/*!
    \internal
    \class QmlGraphicsWebView
    \brief The QmlGraphicsWebView class allows you to add web content to a QmlView.

    A WebView renders web content base on a URL.

    \image webview.png

    The item includes no scrolling, scaling,
    toolbars, etc., those must be implemented around WebView. See the WebBrowser example
    for a demonstration of this.

    A QmlGraphicsWebView object can be instantiated in Qml using the tag \l WebView.
*/

QmlGraphicsWebView::QmlGraphicsWebView(QmlGraphicsItem *parent)
  : QmlGraphicsPaintedItem(*(new QmlGraphicsWebViewPrivate), parent)
{
    init();
}

QmlGraphicsWebView::QmlGraphicsWebView(QmlGraphicsWebViewPrivate &dd, QmlGraphicsItem *parent)
  : QmlGraphicsPaintedItem(dd, parent)
{
    init();
}

QmlGraphicsWebView::~QmlGraphicsWebView()
{
    Q_D(QmlGraphicsWebView);
    delete d->page;
}

void QmlGraphicsWebView::init()
{
    Q_D(QmlGraphicsWebView);

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    d->page = 0;
}

void QmlGraphicsWebView::componentComplete()
{
    QmlGraphicsPaintedItem::componentComplete();
    Q_D(QmlGraphicsWebView);
    switch (d->pending) {
        case QmlGraphicsWebViewPrivate::PendingUrl:
            setUrl(d->pending_url);
            break;
        case QmlGraphicsWebViewPrivate::PendingHtml:
            setHtml(d->pending_string, d->pending_url);
            break;
        case QmlGraphicsWebViewPrivate::PendingContent:
            setContent(d->pending_data, d->pending_string, d->pending_url);
            break;
        default:
            break;
    }
    d->pending = QmlGraphicsWebViewPrivate::PendingNone;
    d->updateWindowObjects();
}

QmlGraphicsWebView::Status QmlGraphicsWebView::status() const
{
    Q_D(const QmlGraphicsWebView);
    return d->status;
}


/*!
    \qmlproperty real WebView::progress
    This property holds the progress of loading the current URL, from 0 to 1.

    \sa onLoadFinished() onLoadFailed()
*/
qreal QmlGraphicsWebView::progress() const
{
    Q_D(const QmlGraphicsWebView);
    return d->progress;
}

void QmlGraphicsWebView::doLoadStarted()
{
    Q_D(QmlGraphicsWebView);

    if (!d->url.isEmpty()) {
        d->status = Loading;
        emit statusChanged(d->status);
    }
    emit loadStarted();
}

void QmlGraphicsWebView::doLoadProgress(int p)
{
    Q_D(QmlGraphicsWebView);
    if (d->progress == p/100.0)
        return;
    d->progress = p/100.0;
    emit progressChanged();
}

void QmlGraphicsWebView::pageUrlChanged()
{
    Q_D(QmlGraphicsWebView);

    // Reset zooming to full
    qreal zf = 1.0;
    if (d->preferredwidth) {
        if (d->pagewidth)
            zf = qreal(d->preferredwidth)/d->pagewidth;
        page()->mainFrame()->setZoomFactor(zf);
        page()->setViewportSize(QSize(d->preferredwidth,-1));
    } else {
        page()->mainFrame()->setZoomFactor(zf);
        page()->setViewportSize(QSize(-1,-1));
    }
    emit zooming(zf,0,0);
    expandToWebPage();

    if ((d->url.isEmpty() && page()->mainFrame()->url() != QUrl(QLatin1String("about:blank")))
        || d->url != page()->mainFrame()->url())
    {
        d->url = page()->mainFrame()->url();
        if (d->url == QUrl(QLatin1String("about:blank")))
            d->url = QUrl();
        emit urlChanged();
    }
}

void QmlGraphicsWebView::doLoadFinished(bool ok)
{
    Q_D(QmlGraphicsWebView);

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
QUrl QmlGraphicsWebView::url() const
{
    Q_D(const QmlGraphicsWebView);
    return d->url;
}

void QmlGraphicsWebView::setUrl(const QUrl &url)
{
    Q_D(QmlGraphicsWebView);
    if (url == d->url)
        return;

    if (isComponentComplete()) {
        d->url = url;
        qreal zf = 1.0;
        if (d->preferredwidth) {
            if (d->pagewidth)
                zf = qreal(d->preferredwidth)/d->pagewidth;
            page()->setViewportSize(QSize(d->preferredwidth,-1));
        } else {
            page()->setViewportSize(QSize(-1,-1));
        }
        page()->mainFrame()->setZoomFactor(zf);
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
int QmlGraphicsWebView::preferredWidth() const
{
    Q_D(const QmlGraphicsWebView);
    return d->preferredwidth;
}

void QmlGraphicsWebView::setPreferredWidth(int iw)
{
    Q_D(QmlGraphicsWebView);
    if (d->preferredwidth == iw) return;
    if (d->pagewidth) {
        if (d->preferredwidth) {
            setZoomFactor(zoomFactor()*iw/d->preferredwidth);
        } else {
            setZoomFactor(qreal(iw)/d->pagewidth);
        }
    }
    d->preferredwidth = iw;
    expandToWebPage();
    emit preferredWidthChanged();
}

/*!
    \qmlproperty int WebView::webPageWidth
    This property holds the page width suggested to the web engine.
*/
int QmlGraphicsWebView::webPageWidth() const
{
    Q_D(const QmlGraphicsWebView);
    return d->pagewidth;
}

void QmlGraphicsWebView::setWebPageWidth(int pw)
{
    Q_D(QmlGraphicsWebView);
    if (d->pagewidth == pw) return;
    d->pagewidth = pw;
    expandToWebPage();
}

/*!
    Evaluates the \a scriptSource JavaScript inside the main frame
    context and returns the result of the last executed statement.
*/
QVariant QmlGraphicsWebView::evaluateJavaScript(const QString &scriptSource)
{
    return this->page()->mainFrame()->evaluateJavaScript(scriptSource);
}

void QmlGraphicsWebView::focusChanged(bool hasFocus)
{
    QFocusEvent e(hasFocus ? QEvent::FocusIn : QEvent::FocusOut);
    page()->event(&e);
    QmlGraphicsItem::focusChanged(hasFocus);
}

void QmlGraphicsWebView::contentsSizeChanged(const QSize&)
{
    expandToWebPage();
}

void QmlGraphicsWebView::expandToWebPage()
{
    Q_D(QmlGraphicsWebView);
    QSize cs = page()->mainFrame()->contentsSize();
    qreal zoom = zoomFactor();
    if (cs.width() < d->preferredwidth*zoom)
        cs.setWidth(d->preferredwidth*zoom);
    if (widthValid())
        cs.setWidth(width());
    if (heightValid())
        cs.setHeight(height());
    if (cs != page()->viewportSize()) {
        page()->setViewportSize(cs);
        clearCache();
        setImplicitWidth(cs.width());
        setImplicitHeight(cs.height());
    }
}

void QmlGraphicsWebView::geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry)
{
    if (newGeometry.size() != oldGeometry.size())
        expandToWebPage();
    QmlGraphicsPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void QmlGraphicsWebView::paintPage(const QRect& r)
{
    Q_D(QmlGraphicsWebView);
    if (d->page->mainFrame()->contentsSize() != contentsSize())
        setContentsSize(d->page->mainFrame()->contentsSize());
    dirtyCache(r);
    update();
}

/*!
  \qmlproperty int WebView::pixelCacheSize

  This property holds the maximum number of pixels of image cache to
  allow. The default is 0.1 megapixels. The cache will not be larger
  than the (unscaled) size of the WebView.
*/
int QmlGraphicsWebView::pixelCacheSize() const
{
    Q_D(const QmlGraphicsWebView);
    return d->max_imagecache_size;
}

void QmlGraphicsWebView::setPixelCacheSize(int pixels)
{
    Q_D(QmlGraphicsWebView);
    if (pixels < d->max_imagecache_size) {
        int cachesize=0;
        for (int i=0; i<d->imagecache.count(); ++i) {
            QRect area = d->imagecache[i]->area;
            cachesize += area.width()*area.height();
        }
        while (d->imagecache.count() && cachesize > pixels) {
            int oldest=-1;
            int age=-1;
            for (int i=0; i<d->imagecache.count(); ++i) {
                int a = d->imagecache[i]->age;
                if (a > age) {
                    oldest = i;
                    age = a;
                }
            }
            cachesize -= d->imagecache[oldest]->area.width()*d->imagecache[oldest]->area.height();
            d->imagecache.removeAt(oldest);
        }
    }
    d->max_imagecache_size = pixels;
}

/*!
    \qmlproperty list<object> WebView::javaScriptWindowObjects

    This property is a list of object that are available from within
    the webview's JavaScript context.

    The \a object will be inserted as a child of the frame's window
    object, under the name given by the attached property \c WebView.windowObjectName.

    \qml
    WebView {
        javaScriptWindowObjects: Object {
            WebView.windowObjectName: "coordinates"
        }
    }
    \endqml

    Properties of the object will be exposed as JavaScript properties and slots as
    JavaScript methods.

    If Javascript is not enabled for this page, then this property does nothing.
*/
QmlList<QObject *> *QmlGraphicsWebView::javaScriptWindowObjects()
{
    Q_D(QmlGraphicsWebView);
    return &d->windowObjects;
}

class QmlGraphicsWebViewAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString windowObjectName READ windowObjectName WRITE setWindowObjectName)
public:
    QmlGraphicsWebViewAttached(QObject *parent)
        : QObject(parent)
    {
    }

    QString windowObjectName() const
    {
        return m_windowObjectName;
    }

    void setWindowObjectName(const QString &n)
    {
        m_windowObjectName = n;
    }

private:
    QString m_windowObjectName;
};

QmlGraphicsWebViewAttached *QmlGraphicsWebView::qmlAttachedProperties(QObject *o)
{
    return new QmlGraphicsWebViewAttached(o);
}

void QmlGraphicsWebViewPrivate::updateWindowObjects()
{
    Q_Q(QmlGraphicsWebView);
    if (!q->isComponentComplete() || !page)
        return;

    for (int ii = 0; ii < windowObjects.count(); ++ii) {
        QObject *object = windowObjects.at(ii);
        QmlGraphicsWebViewAttached *attached = static_cast<QmlGraphicsWebViewAttached *>(qmlAttachedPropertiesObject<QmlGraphicsWebView>(object));
        if (attached && !attached->windowObjectName().isEmpty()) {
            page->mainFrame()->addToJavaScriptWindowObject(attached->windowObjectName(), object);
        }
    }
}

bool QmlGraphicsWebView::renderingEnabled() const
{
    Q_D(const QmlGraphicsWebView);
    return d->rendering;
}

void QmlGraphicsWebView::setRenderingEnabled(bool enabled)
{
    Q_D(QmlGraphicsWebView);
    if (d->rendering == enabled)
        return;
    d->rendering = enabled;
    setCacheFrozen(!enabled);
    if (enabled)
        clearCache();
}


void QmlGraphicsWebView::drawContents(QPainter *p, const QRect &r)
{
    Q_D(QmlGraphicsWebView);
    if (d->rendering)
        page()->mainFrame()->render(p,r);
}

static QMouseEvent *sceneMouseEventToMouseEvent(QGraphicsSceneMouseEvent *e)
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

    QMouseEvent *me = new QMouseEvent(t, e->pos().toPoint(), e->button(), e->buttons(), 0);
    return me;
}

static QMouseEvent *sceneHoverMoveEventToMouseEvent(QGraphicsSceneHoverEvent *e)
{
    QEvent::Type t = QEvent::MouseMove;

    QMouseEvent *me = new QMouseEvent(t, e->pos().toPoint(), Qt::NoButton, Qt::NoButton, 0);

    return me;
}


/*!
    \qmlsignal WebView::onDoubleClick(clickx,clicky)

    The WebView does not pass double-click events to the web engine, but rather
    emits this signals.
*/

void QmlGraphicsWebView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);
    emit doubleClick(me->x(),me->y());
    delete me;
}

void QmlGraphicsWebView::heuristicZoom(int clickX, int clickY)
{
    Q_D(QmlGraphicsWebView);
    qreal ozf = zoomFactor();
    QRect showarea = elementAreaAt(clickX, clickY, 1, 1);
    qreal z = qreal(preferredWidth())*ozf/showarea.width()*.95;
    if ((z/ozf > 0.99 && z/ozf <1.01) || z < qreal(d->preferredwidth)/d->pagewidth) {
        // zoom out
        z = qreal(d->preferredwidth)/d->pagewidth;
    }
    QRectF r(showarea.left()/ozf*z, showarea.top()/ozf*z, showarea.width()/ozf*z, showarea.height()/ozf*z);
    emit zooming(z,r.x()+r.width()/2, r.y()+r.height()/2);
}

void QmlGraphicsWebView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus (true);
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);
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
        QmlGraphicsPaintedItem::mousePressEvent(event);
    }
}

void QmlGraphicsWebView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);
    page()->event(me);
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
        QmlGraphicsPaintedItem::mouseReleaseEvent(event);
    }
}

void QmlGraphicsWebView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);
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
    delete me;
    if (!event->isAccepted())
        QmlGraphicsPaintedItem::mouseMoveEvent(event);

}
void QmlGraphicsWebView::hoverMoveEvent (QGraphicsSceneHoverEvent * event)
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
        QmlGraphicsPaintedItem::hoverMoveEvent(event);
}

void QmlGraphicsWebView::keyPressEvent(QKeyEvent* event)
{
    page()->event(event);
    if (!event->isAccepted())
        QmlGraphicsPaintedItem::keyPressEvent(event);
}

void QmlGraphicsWebView::keyReleaseEvent(QKeyEvent* event)
{
    page()->event(event);
    if (!event->isAccepted())
        QmlGraphicsPaintedItem::keyReleaseEvent(event);
}

bool QmlGraphicsWebView::sceneEvent(QEvent *event) 
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
    return QmlGraphicsPaintedItem::sceneEvent(event); 
} 



/*!
    \qmlproperty action WebView::back
    This property holds the action for causing the previous URL in the history to be displayed.
*/
QAction *QmlGraphicsWebView::backAction() const
{
    return page()->action(QWebPage::Back);
}

/*!
    \qmlproperty action WebView::forward
    This property holds the action for causing the next URL in the history to be displayed.
*/
QAction *QmlGraphicsWebView::forwardAction() const
{
    return page()->action(QWebPage::Forward);
}

/*!
    \qmlproperty action WebView::reload
    This property holds the action for reloading with the current URL
*/
QAction *QmlGraphicsWebView::reloadAction() const
{
    return page()->action(QWebPage::Reload);
}

/*!
    \qmlproperty action WebView::stop
    This property holds the action for stopping loading with the current URL
*/
QAction *QmlGraphicsWebView::stopAction() const
{
    return page()->action(QWebPage::Stop);
}

/*!
    \qmlproperty real WebView::title
    This property holds the title of the web page currently viewed

    By default, this property contains an empty string.
*/
QString QmlGraphicsWebView::title() const
{
    return page()->mainFrame()->title();
}



/*!
    \qmlproperty pixmap WebView::icon
    This property holds the icon associated with the web page currently viewed
*/
QPixmap QmlGraphicsWebView::icon() const
{
    return page()->mainFrame()->icon().pixmap(QSize(256,256));
}


/*!
    \qmlproperty real WebView::textSizeMultiplier
    This property holds the multiplier used to scale the text in a Web page
*/
void QmlGraphicsWebView::setTextSizeMultiplier(qreal factor)
{
    page()->mainFrame()->setTextSizeMultiplier(factor);
}

qreal QmlGraphicsWebView::textSizeMultiplier() const
{
    return page()->mainFrame()->textSizeMultiplier();
}

/*!
    \qmlproperty real WebView::zoomFactor
    This property holds the multiplier used to scale the contents of a Web page.
*/
void QmlGraphicsWebView::setZoomFactor(qreal factor)
{
    Q_D(QmlGraphicsWebView);
    if (factor == page()->mainFrame()->zoomFactor())
        return;

    page()->mainFrame()->setZoomFactor(factor);
    page()->setViewportSize(QSize(d->pagewidth*factor,-1));
    expandToWebPage();

    emit zoomFactorChanged();
}

qreal QmlGraphicsWebView::zoomFactor() const
{
    return page()->mainFrame()->zoomFactor();
}

/*!
    \qmlproperty string WebView::statusText

    This property is the current status suggested by the current web page. In a web browser,
    such status is often shown in some kind of status bar.
*/
void QmlGraphicsWebView::setStatusText(const QString& s)
{
    Q_D(QmlGraphicsWebView);
    d->statusText = s;
    emit statusTextChanged();
}

void QmlGraphicsWebView::windowObjectCleared()
{
    Q_D(QmlGraphicsWebView);
    d->updateWindowObjects();
}

QString QmlGraphicsWebView::statusText() const
{
    Q_D(const QmlGraphicsWebView);
    return d->statusText;
}

QWebPage *QmlGraphicsWebView::page() const
{
    Q_D(const QmlGraphicsWebView);

    if (!d->page) {
        QmlGraphicsWebView *self = const_cast<QmlGraphicsWebView*>(this);
        QWebPage *wp = new QmlGraphicsWebPage(self);

        // QML items don't default to having a background,
        // even though most we pages will set one anyway.
        QPalette pal = QApplication::palette();
        pal.setBrush(QPalette::Base, QColor::fromRgbF(0, 0, 0, 0));
        wp->setPalette(pal);

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
QmlGraphicsWebSettings *QmlGraphicsWebView::settingsObject() const
{
    Q_D(const QmlGraphicsWebView);
    d->settings.s = page()->settings();
    return &d->settings;
}

void QmlGraphicsWebView::setPage(QWebPage *page)
{
    Q_D(QmlGraphicsWebView);
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
    d->page->setViewportSize(QSize(
        d->preferredwidth>0 ? d->preferredwidth : -1, -1));
    d->page->mainFrame()->setScrollBarPolicy(Qt::Horizontal,Qt::ScrollBarAlwaysOff);
    d->page->mainFrame()->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
    connect(d->page,SIGNAL(repaintRequested(QRect)),this,SLOT(paintPage(QRect)));
    connect(d->page->mainFrame(),SIGNAL(urlChanged(QUrl)),this,SLOT(pageUrlChanged()));
    connect(d->page->mainFrame(), SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    connect(d->page->mainFrame(), SIGNAL(iconChanged()), this, SIGNAL(iconChanged()));
    connect(d->page->mainFrame(), SIGNAL(contentsSizeChanged(QSize)), this, SLOT(contentsSizeChanged(QSize)));

    connect(d->page,SIGNAL(loadStarted()),this,SLOT(doLoadStarted()));
    connect(d->page,SIGNAL(loadProgress(int)),this,SLOT(doLoadProgress(int)));
    connect(d->page,SIGNAL(loadFinished(bool)),this,SLOT(doLoadFinished(bool)));
    connect(d->page,SIGNAL(statusBarMessage(QString)),this,SLOT(setStatusText(QString)));

    connect(d->page->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(windowObjectCleared()));
}

/*!
    \qmlsignal WebView::onLoadStarted()

    This handler is called when the web engine begins loading
    a page.

    \sa progress onLoadFinished() onLoadFailed()
*/

/*!
    \qmlsignal WebView::onLoadFinished()

    This handler is called when the web engine finishes loading
    a page, including any component content.

    \sa progress onLoadFailed()
*/

/*!
    \qmlsignal WebView::onLoadFailed()

    This handler is called when the web engine fails loading
    a page or any component content.

    \sa progress onLoadFinished()
*/

void QmlGraphicsWebView::load(const QNetworkRequest &request,
          QNetworkAccessManager::Operation operation,
          const QByteArray &body)
{
    page()->mainFrame()->load(request, operation, body);
}

QString QmlGraphicsWebView::html() const
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
void QmlGraphicsWebView::setHtml(const QString &html, const QUrl &baseUrl)
{
    Q_D(QmlGraphicsWebView);
    page()->setViewportSize(QSize(
        d->preferredwidth>0 ? d->preferredwidth : width(), height()));
    if (isComponentComplete())
        page()->mainFrame()->setHtml(html, baseUrl);
    else {
        d->pending = d->PendingHtml;
        d->pending_url = baseUrl;
        d->pending_string = html;
    }
}

void QmlGraphicsWebView::setContent(const QByteArray &data, const QString &mimeType, const QUrl &baseUrl)
{
    Q_D(QmlGraphicsWebView);
    page()->setViewportSize(QSize(
        d->preferredwidth>0 ? d->preferredwidth : width(), height()));

    if (isComponentComplete())
        page()->mainFrame()->setContent(data,mimeType,qmlContext(this)->resolvedUrl(baseUrl));
    else {
        d->pending = d->PendingContent;
        d->pending_url = baseUrl;
        d->pending_string = mimeType;
        d->pending_data = data;
    }
}

QWebHistory *QmlGraphicsWebView::history() const
{
    return page()->history();
}

QWebSettings *QmlGraphicsWebView::settings() const
{
    return page()->settings();
}

QmlGraphicsWebView *QmlGraphicsWebView::createWindow(QWebPage::WebWindowType type)
{
    Q_D(QmlGraphicsWebView);
    switch (type) {
        case QWebPage::WebBrowserWindow: {
            if (!d->newWindowComponent && d->newWindowParent)
                qWarning("WebView::newWindowComponent not set - WebView::newWindowParent ignored");
            else if (d->newWindowComponent && !d->newWindowParent)
                qWarning("WebView::newWindowParent not set - WebView::newWindowComponent ignored");
            else if (d->newWindowComponent && d->newWindowParent) {
                QmlGraphicsWebView *webview = 0;
                QmlContext *windowContext = new QmlContext(qmlContext(this));

                QObject *nobj = d->newWindowComponent->create(windowContext);
                if (nobj) {
                    windowContext->setParent(nobj);
                    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem *>(nobj);
                    if (!item) {
                        delete nobj;
                    } else {
                        webview = item->findChild<QmlGraphicsWebView*>();
                        if (!webview) {
                            delete item;
                        } else {
                            item->setParent(d->newWindowParent);
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
QmlComponent *QmlGraphicsWebView::newWindowComponent() const
{
    Q_D(const QmlGraphicsWebView);
    return d->newWindowComponent;
}

void QmlGraphicsWebView::setNewWindowComponent(QmlComponent *newWindow)
{
    Q_D(QmlGraphicsWebView);
    delete d->newWindowComponent;
    d->newWindowComponent = newWindow;
}


/*!
    \qmlproperty item WebView::newWindowParent

    The parent item for new windows.

    \sa newWindowComponent
*/
QmlGraphicsItem *QmlGraphicsWebView::newWindowParent() const
{
    Q_D(const QmlGraphicsWebView);
    return d->newWindowParent;
}

void QmlGraphicsWebView::setNewWindowParent(QmlGraphicsItem *parent)
{
    Q_D(QmlGraphicsWebView);
    delete d->newWindowParent;
    d->newWindowParent = parent;
}

/*!
    Returns the area of the largest element at position (\a x,\a y) that is no larger
    than \a maxwidth by \a maxheight pixels.

    May return an area larger in the case when no smaller element is at the position.
*/
QRect QmlGraphicsWebView::elementAreaAt(int x, int y, int maxwidth, int maxheight) const
{
    QWebHitTestResult hit = page()->mainFrame()->hitTestContent(QPoint(x,y));
    QWebElement element = hit.enclosingBlockElement();
    QWebElement parent = element.parent();
    if (maxwidth<=0) maxwidth = INT_MAX;
    if (maxheight<=0) maxheight = INT_MAX;
    while (!parent.isNull() && parent.geometry().width() <= maxwidth && parent.geometry().height() <= maxheight) {
        element = parent;
        parent = element.parent();
    }
    return element.geometry();
}

/*!
    \internal
    \class QmlGraphicsWebPage
    \brief The QmlGraphicsWebPage class is a QWebPage that can create QML plugins.

    \sa QmlGraphicsWebView
*/
QmlGraphicsWebPage::QmlGraphicsWebPage(QmlGraphicsWebView *parent) :
    QWebPage(parent)
{
}

QmlGraphicsWebPage::~QmlGraphicsWebPage()
{
}

/*
    Qt WebKit does not understand non-QWidget plugins, so dummy widgets
    are created, parented to a single dummy tool window.

    The requirements for QML object plugins are input to the Qt WebKit
    non-QWidget plugin support, which will obsolete this kludge.
*/
class QWidget_Dummy_Plugin : public QWidget
{
    Q_OBJECT
public:
    static QWidget *dummy_shared_parent()
    {
        static QWidget *dsp = 0;
        if (!dsp) {
            dsp = new QWidget(0,Qt::Tool);
            dsp->setGeometry(-10000,-10000,0,0);
            dsp->show();
        }
        return dsp;
    }
    QWidget_Dummy_Plugin(const QUrl& url, QmlGraphicsWebView *view, const QStringList &paramNames, const QStringList &paramValues) :
        QWidget(dummy_shared_parent()),
        propertyNames(paramNames),
        propertyValues(paramValues),
        webview(view)
    {
        QmlEngine *engine = qmlEngine(webview);
        component = new QmlComponent(engine, url, this);
        item = 0;
        if (component->isLoading())
            connect(component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(qmlLoaded()));
        else
            qmlLoaded();
    }

public Q_SLOTS:
    void qmlLoaded()
    {
        if (component->isError()) {
            // ### Could instead give these errors to the WebView to handle.
            qWarning() << component->errors();
            return;
        }
        item = qobject_cast<QmlGraphicsItem*>(component->create(qmlContext(webview)));
        item->setParent(webview);
        QString jsObjName;
        for (int i=0; i<propertyNames.count(); ++i) {
            if (propertyNames[i] != QLatin1String("type") && propertyNames[i] != QLatin1String("data")) {
                item->setProperty(propertyNames[i].toUtf8(),propertyValues[i]);
                if (propertyNames[i] == QLatin1String("objectname"))
                    jsObjName = propertyValues[i]; 
            }
        }
        if (!jsObjName.isNull()) { 
            QWebFrame *f = webview->page()->mainFrame(); 
            f->addToJavaScriptWindowObject(jsObjName, item); 
        }
        resizeEvent(0);
        delete component;
        component = 0;
    }
    void resizeEvent(QResizeEvent*)
    {
        if (item) {
            item->setX(x());
            item->setY(y());
            item->setWidth(width());
            item->setHeight(height());
        }
    }

private:
    QmlComponent *component;
    QmlGraphicsItem *item;
    QStringList propertyNames, propertyValues;
    QmlGraphicsWebView *webview;
};

QmlGraphicsWebView *QmlGraphicsWebPage::viewItem()
{
    return static_cast<QmlGraphicsWebView*>(parent());
}

QObject *QmlGraphicsWebPage::createPlugin(const QString &, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues)
{
    QUrl comp = qmlContext(viewItem())->resolvedUrl(url);
    return new QWidget_Dummy_Plugin(comp,viewItem(),paramNames,paramValues);
}

QWebPage *QmlGraphicsWebPage::createWindow(WebWindowType type)
{
    QmlGraphicsWebView *newView = viewItem()->createWindow(type);
    if (newView)
        return newView->page();
    return 0;
}

QT_END_NAMESPACE

#include "qmlgraphicswebview.moc"
