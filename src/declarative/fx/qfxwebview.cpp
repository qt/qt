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

#include "qml.h"
#include "qmlengine.h"
#include "qmlstate.h"
#include "qlistmodelinterface.h"

#include "qfxwebview.h"
#include <private/qfxpainteditem_p.h>

QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,WebView,QFxWebView)

static const int MAX_DOUBLECLICK_TIME=500; // XXX need better gesture system

class QFxWebSettings : public QObject {
    Q_OBJECT
    /*
        StandardFont,
        FixedFont,
        SerifFont,
        SansSerifFont,
        CursiveFont,
        FantasyFont

        MinimumFontSize,
        MinimumLogicalFontSize,
        DefaultFontSize,
        DefaultFixedFontSize
    */

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

public:
    QFxWebSettings() {}

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

    QWebSettings *s;
};

QML_DECLARE_TYPE(QFxWebSettings)
QML_DEFINE_NOCREATE_TYPE(QFxWebSettings)

class QFxWebViewPrivate : public QFxPaintedItemPrivate
{
    Q_DECLARE_PUBLIC(QFxWebView)

public:
    QFxWebViewPrivate()
      : QFxPaintedItemPrivate(), page(0), idealwidth(0), idealheight(0), interactive(true), lastPress(0), lastRelease(0), mouseX(0), mouseY(0),
            progress(1.0), pending(PendingNone), windowObjects(this)
    {
    }

    QWebPage *page;

    int idealwidth;
    int idealheight;
    bool interactive;
    QMouseEvent *lastPress, *lastRelease;
    int mouseX, mouseY;
    qreal progress;
    QBasicTimer dcTimer;
    QString statusBarMessage;
    enum { PendingNone, PendingUrl, PendingHtml, PendingContent } pending;
    QUrl pending_url;
    QString pending_string;
    QByteArray pending_data;
    mutable QFxWebSettings settings;

    void updateWindowObjects();
    class WindowObjectList : public QmlConcreteList<QObject *>
    {
    public:
        WindowObjectList(QFxWebViewPrivate *p)
            : priv(p) {}
        virtual void append(QObject *v) {
            QmlConcreteList<QObject *>::append(v);
            priv->updateWindowObjects();
        }
    private:
        QFxWebViewPrivate *priv;
    } windowObjects;
};

/*!
    \qmlclass WebView
    \brief The WebView item allows you to add web content to a canvas.
    \inherits Item

    A WebView renders web content based on a URL.

    If the width and height of the item is not set, they will
    dynamically adjust to a size appropriate for the content.
    This width may be large (eg. 980) for typical online web pages.

    If the idealWidth is set, the width will be this amount or larger,
    usually laying out the web content to fit the idealWidth.

    If the idealHeight is set, the height will be this amount or larger.
    Due to WebKit limitations, the height may be more than necessary
    if the idealHeight is changed after the content is loaded.

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
    \class QFxWebView
    \brief The QFxWebView class allows you to add web content to a QFxView.

    A WebView renders web content base on a URL.

    \image webview.png

    The item includes no scrolling, scaling,
    toolbars, etc., those must be implemented around WebView. See the WebBrowser example
    for a demonstration of this.

    A QFxWebView object can be instantiated in Qml using the tag \l WebView.
*/

QFxWebView::QFxWebView(QFxItem *parent)
  : QFxPaintedItem(*(new QFxWebViewPrivate), parent)
{
    init();
}

QFxWebView::QFxWebView(QFxWebViewPrivate &dd, QFxItem *parent)
  : QFxPaintedItem(dd, parent)
{
    init();
}

QFxWebView::~QFxWebView()
{
    Q_D(QFxWebView);
    delete d->page;
}

void QFxWebView::init()
{
    Q_D(QFxWebView);

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    d->page = 0;
}

void QFxWebView::componentComplete()
{
    QFxPaintedItem::componentComplete();
    Q_D(QFxWebView);
    switch (d->pending) {
        case QFxWebViewPrivate::PendingUrl:
            setUrl(d->pending_url);
            break;
        case QFxWebViewPrivate::PendingHtml:
            setHtml(d->pending_string, d->pending_url);
            break;
        case QFxWebViewPrivate::PendingContent:
            setContent(d->pending_data, d->pending_string, d->pending_url);
            break;
        default:
            break;
    }
    d->pending = QFxWebViewPrivate::PendingNone;
    d->updateWindowObjects();
}

/*!
    \qmlproperty real WebView::progress
    This property holds the progress of loading the current URL, from 0 to 1.
*/
qreal QFxWebView::progress() const
{
    Q_D(const QFxWebView);
    return d->progress;
}

void QFxWebView::doLoadProgress(int p)
{
    Q_D(QFxWebView);
    if (d->progress == p/100.0)
        return;
    d->progress = p/100.0;
    expandToWebPage();
    emit progressChanged();
}

void QFxWebView::doLoadFinished(bool ok)
{
    // XXX bug 232556 - pages with no title never get this signal
    if (title().isEmpty())
        emit urlChanged();

    if (ok) {
        emit loadFinished();
    } else {
        emit loadFailed();
    }
}

/*!
    \qmlproperty url WebView::url
    This property holds the URL to the page displayed in this item.

    Note that after this property is set, it may take some time
    before the change is notified, as this only happens when
    loading of the URL successfully starts.
*/
QUrl QFxWebView::url() const
{
    return page()->mainFrame()->url();
}

void QFxWebView::setUrl(const QUrl &url)
{
    Q_D(QFxWebView);
    if (url == page()->mainFrame()->url())
        return;

    page()->setViewportSize(QSize(
        d->idealwidth>0 ? d->idealwidth : width(),
        d->idealheight>0 ? d->idealheight : height()));

    Q_ASSERT(!url.isRelative());

    if (isComponentComplete())
        page()->mainFrame()->load(url);
    else {
        d->pending = d->PendingUrl;
        d->pending_url = url;
    }

    // emit urlChanged() - not until actually loaded
}

/*!
    \qmlproperty int WebView::idealWidth
    This property holds the ideal width for displaying the current URL.
*/
int QFxWebView::idealWidth() const
{
    Q_D(const QFxWebView);
    return d->idealwidth;
}

void QFxWebView::setIdealWidth(int iw)
{
    Q_D(QFxWebView);
    if (d->idealwidth == iw) return;
    d->idealwidth = iw;
    expandToWebPage();
    emit idealWidthChanged();
}

/*!
    \qmlproperty int WebView::idealHeight
    This property holds the ideal height for displaying the current URL.
*/
int QFxWebView::idealHeight() const
{
    Q_D(const QFxWebView);
    return d->idealheight;
}

void QFxWebView::setIdealHeight(int ih)
{
    Q_D(QFxWebView);
    if (d->idealheight == ih) return;
    d->idealheight = ih;
    expandToWebPage();
    emit idealHeightChanged();
}

/*!
  \qmlproperty bool WebView::interactive

  This property controls whether the item responds to mouse and
  key events.
*/
bool QFxWebView::interactive() const
{
    Q_D(const QFxWebView);
    return d->interactive;
}

void QFxWebView::setInteractive(bool i)
{
    Q_D(QFxWebView);
    if (d->interactive == i) return;
    d->interactive = i;
    emit interactiveChanged();
}

/*!
    Evaluates the \a scriptSource JavaScript inside the main frame
    context and returns the result of the last executed statement.
*/
QVariant QFxWebView::evaluateJavaScript(const QString &scriptSource)
{
    return this->page()->mainFrame()->evaluateJavaScript(scriptSource);
}

void QFxWebView::focusChanged(bool hasFocus)
{
    QFocusEvent e(hasFocus ? QEvent::FocusIn : QEvent::FocusOut);
    page()->event(&e);
    QFxItem::focusChanged(hasFocus);
}

void QFxWebView::expandToWebPage()
{
    Q_D(QFxWebView);
    QSize cs = page()->mainFrame()->contentsSize();
    if (cs.width() < d->idealwidth)
        cs.setWidth(d->idealwidth);
    if (cs.height() < d->idealheight)
        cs.setHeight(d->idealheight);
    if (widthValid() && cs.width() < width())
        cs.setWidth(width());
    if (heightValid() && cs.height() < height())
        cs.setHeight(height());
    if (cs != page()->viewportSize()) {
        page()->setViewportSize(cs);
        clearCache();
        setImplicitWidth(cs.width());
        setImplicitHeight(cs.height());
    }
}

void QFxWebView::geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry)
{
    if (newGeometry.size() != oldGeometry.size())
        expandToWebPage();
    QFxPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void QFxWebView::paintPage(const QRect& r)
{
    Q_D(QFxWebView);
    if (d->page->mainFrame()->contentsSize() != contentsSize())
        setContentsSize(d->page->mainFrame()->contentsSize());
    dirtyCache(r);
    update();
}

/*!
  \qmlproperty int WebView::cacheSize

  This property holds the maximum number of pixels of image cache to
  allow. The default is 0.1 megapixels. The cache will not be larger
  than the (unscaled) size of the WebView.
*/
int QFxWebView::cacheSize() const
{
    Q_D(const QFxWebView);
    return d->max_imagecache_size;
}

void QFxWebView::setCacheSize(int pixels)
{
    Q_D(QFxWebView);
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
QmlList<QObject *> *QFxWebView::javaScriptWindowObjects()
{
    Q_D(QFxWebView);
    return &d->windowObjects;
}

class QFxWebViewAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString windowObjectName READ windowObjectName WRITE setWindowObjectName)
public:
    QFxWebViewAttached(QObject *parent)
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

QFxWebViewAttached *QFxWebView::qmlAttachedProperties(QObject *o)
{
    return new QFxWebViewAttached(o);
}

void QFxWebViewPrivate::updateWindowObjects()
{
    Q_Q(QFxWebView);
    if (!q->isComponentComplete() || !page)
        return;

    for (int ii = 0; ii < windowObjects.count(); ++ii) {
        QObject *object = windowObjects.at(ii);
        QFxWebViewAttached *attached = static_cast<QFxWebViewAttached *>(qmlAttachedPropertiesObject<QFxWebView>(object));
        if (attached && !attached->windowObjectName().isEmpty()) {
            page->mainFrame()->addToJavaScriptWindowObject(attached->windowObjectName(), object);
        }
    }
}

void QFxWebView::drawContents(QPainter *p, const QRect &r)
{
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


void QFxWebView::timerEvent(QTimerEvent *event)
{
    Q_D(QFxWebView);
    if (event->timerId() ==d->dcTimer.timerId()) {
        d->dcTimer.stop();
        if (d->lastPress) {
            page()->event(d->lastPress);
            delete d->lastPress;
            d->lastPress = 0;
        }
        if (d->lastRelease) {
            page()->event(d->lastRelease);
            delete d->lastRelease;
            d->lastRelease = 0;
        }
    }
}

int QFxWebView::mouseX() const
{
    Q_D(const QFxWebView);
    if (d->lastPress)
        return d->lastPress->x();
    if (d->lastRelease)
        return d->lastRelease->x();
    return d->mouseX;
}

int QFxWebView::mouseY() const
{
    Q_D(const QFxWebView);
    if (d->lastPress)
        return d->lastPress->y();
    if (d->lastRelease)
        return d->lastRelease->y();
    return d->mouseY;
}

void QFxWebView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QMouseEvent *me = sceneMouseEventToMouseEvent(event);
    Q_D(QFxWebView);
    d->dcTimer.stop();
    delete d->lastPress;
    delete d->lastRelease;
    d->lastPress = 0;
    d->lastRelease = 0;
    d->mouseX = me->x();
    d->mouseY = me->y();
    emit doubleClick();
    d->mouseX = 0;
    d->mouseY = 0;
    delete me;
}

void QFxWebView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxWebView);
    if (d->interactive) {
        setFocus (true);
        QMouseEvent *me = sceneMouseEventToMouseEvent(event);
        if (d->lastPress) delete d->lastPress;
        d->lastPress = me;
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
    } else {
        event->setAccepted(false);
    }
    if (!event->isAccepted()) {
        QFxPaintedItem::mousePressEvent(event);
    }
}

void QFxWebView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxWebView);
    if (d->interactive) {
        QMouseEvent *me = sceneMouseEventToMouseEvent(event);
        if (d->lastRelease) delete d->lastRelease;
        d->lastRelease = me;
        page()->event(me);
        d->dcTimer.start(MAX_DOUBLECLICK_TIME,this);
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
    } else {
        event->setAccepted(false);
    }
    if (!event->isAccepted()) {
        QFxPaintedItem::mouseReleaseEvent(event);
    }
}

void QFxWebView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(const QFxWebView);
    if (d->interactive && !d->dcTimer.isActive()) {
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
    } else {
        event->setAccepted(false);
    }
    if (!event->isAccepted())
        QFxPaintedItem::mouseMoveEvent(event);

}
void QFxWebView::hoverMoveEvent (QGraphicsSceneHoverEvent * event)
{
    Q_D(const QFxWebView);
    if (d->interactive) {
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
    }
    else {
        event->setAccepted(false);
    }
    if (!event->isAccepted())
        QFxPaintedItem::hoverMoveEvent(event);
}

void QFxWebView::keyPressEvent(QKeyEvent* event)
{
    Q_D(const QFxWebView);
    if (d->interactive)
        page()->event(event);
    if (!event->isAccepted())
        QFxPaintedItem::keyPressEvent(event);
}

void QFxWebView::keyReleaseEvent(QKeyEvent* event)
{
    Q_D(const QFxWebView);
    if (d->interactive)
        page()->event(event);
    if (!event->isAccepted())
        QFxPaintedItem::keyReleaseEvent(event);
}

bool QFxWebView::sceneEvent(QEvent *event) 
{ 
    Q_D(const QFxWebView); 

    if (d->interactive) { 
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
    }
    return QFxPaintedItem::sceneEvent(event); 
} 



/*!
    \qmlproperty action WebView::back
    This property holds the action for causing the previous URL in the history to be displayed.
*/
QAction *QFxWebView::backAction() const
{
    return page()->action(QWebPage::Back);
}

/*!
    \qmlproperty action WebView::forward
    This property holds the action for causing the next URL in the history to be displayed.
*/
QAction *QFxWebView::forwardAction() const
{
    return page()->action(QWebPage::Forward);
}

/*!
    \qmlproperty action WebView::reload
    This property holds the action for reloading with the current URL
*/
QAction *QFxWebView::reloadAction() const
{
    return page()->action(QWebPage::Reload);
}

/*!
    \qmlproperty action WebView::stop
    This property holds the action for stopping loading with the current URL
*/
QAction *QFxWebView::stopAction() const
{
    return page()->action(QWebPage::Stop);
}

/*!
    \qmlproperty real WebView::title
    This property holds the title of the web page currently viewed

    By default, this property contains an empty string.
*/
QString QFxWebView::title() const
{
    return page()->mainFrame()->title();
}



/*!
    \qmlproperty pixmap WebView::icon
    This property holds the icon associated with the web page currently viewed
*/
QPixmap QFxWebView::icon() const
{
    return page()->mainFrame()->icon().pixmap(QSize(256,256));
}


/*!
    \qmlproperty real WebView::textSizeMultiplier
    This property holds the multiplier used to scale the text in a Web page
*/
void QFxWebView::setTextSizeMultiplier(qreal factor)
{
    page()->mainFrame()->setTextSizeMultiplier(factor);
}

qreal QFxWebView::textSizeMultiplier() const
{
    return page()->mainFrame()->textSizeMultiplier();
}

/*!
    \qmlproperty real WebView::zoomFactor
    This property holds the multiplier used to scale the contents of a Web page.
*/
void QFxWebView::setZoomFactor(qreal factor)
{
    Q_D(QFxWebView);
    if (factor == page()->mainFrame()->zoomFactor())
        return;

    //reset viewport size so we resize correctly
    page()->setViewportSize(QSize(
        d->idealwidth>0 ? d->idealwidth : -1,
        d->idealheight>0 ? d->idealheight : -1));

    page()->mainFrame()->setZoomFactor(factor);
    expandToWebPage();
    emit zoomFactorChanged();
}

qreal QFxWebView::zoomFactor() const
{
    return page()->mainFrame()->zoomFactor();
}

void QFxWebView::setStatusBarMessage(const QString& s)
{
    Q_D(QFxWebView);
    d->statusBarMessage = s;
    emit statusChanged();
}

void QFxWebView::windowObjectCleared()
{
    Q_D(QFxWebView);
    d->updateWindowObjects();
}

QString QFxWebView::status() const
{
    Q_D(const QFxWebView);
    return d->statusBarMessage;
}

QWebPage *QFxWebView::page() const
{
    Q_D(const QFxWebView);

    if (!d->page) {
        QFxWebView *self = const_cast<QFxWebView*>(this);
        QWebPage *wp = new QFxWebPage(self);

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

    These properties give access to the settings controlling the web view.

    See QWebSettings for the list of sub-properties.

    \qml
        WebView {
            settings.pluginsEnabled: true
            ...
        }
    \endqml
*/
QFxWebSettings *QFxWebView::settingsObject() const
{
    Q_D(const QFxWebView);
    d->settings.s = page()->settings();
    return &d->settings;
}

void QFxWebView::setPage(QWebPage *page)
{
    Q_D(QFxWebView);
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
        d->idealwidth>0 ? d->idealwidth : -1,
        d->idealheight>0 ? d->idealheight : -1));
    d->page->mainFrame()->setScrollBarPolicy(Qt::Horizontal,Qt::ScrollBarAlwaysOff);
    d->page->mainFrame()->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
    connect(d->page,SIGNAL(repaintRequested(QRect)),this,SLOT(paintPage(QRect)));
    connect(d->page->mainFrame(),SIGNAL(urlChanged(QUrl)),this,SIGNAL(urlChanged()));
    connect(d->page->mainFrame(), SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    connect(d->page->mainFrame(), SIGNAL(iconChanged()), this, SIGNAL(iconChanged()));

    connect(d->page,SIGNAL(loadStarted()),this,SIGNAL(loadStarted()));
    connect(d->page,SIGNAL(loadProgress(int)),this,SLOT(doLoadProgress(int)));
    connect(d->page,SIGNAL(loadFinished(bool)),this,SLOT(doLoadFinished(bool)));
    connect(d->page,SIGNAL(statusBarMessage(QString)),this,SLOT(setStatusBarMessage(QString)));

    connect(d->page->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(windowObjectCleared()));
}

void QFxWebView::load(const QNetworkRequest &request,
          QNetworkAccessManager::Operation operation,
          const QByteArray &body)
{
    page()->mainFrame()->load(request, operation, body);
}

QString QFxWebView::html() const
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
void QFxWebView::setHtml(const QString &html, const QUrl &baseUrl)
{
    Q_D(QFxWebView);
    page()->setViewportSize(QSize(
        d->idealwidth>0 ? d->idealwidth : width(),
        d->idealheight>0 ? d->idealheight : height()));
    if (isComponentComplete())
        page()->mainFrame()->setHtml(html, baseUrl);
    else {
        d->pending = d->PendingHtml;
        d->pending_url = baseUrl;
        d->pending_string = html;
    }
}

void QFxWebView::setContent(const QByteArray &data, const QString &mimeType, const QUrl &baseUrl)
{
    Q_D(QFxWebView);
    page()->setViewportSize(QSize(
        d->idealwidth>0 ? d->idealwidth : width(),
        d->idealheight>0 ? d->idealheight : height()));

    if (isComponentComplete())
        page()->mainFrame()->setContent(data,mimeType,qmlContext(this)->resolvedUrl(baseUrl));
    else {
        d->pending = d->PendingContent;
        d->pending_url = baseUrl;
        d->pending_string = mimeType;
        d->pending_data = data;
    }
}

QWebHistory *QFxWebView::history() const
{
    return page()->history();
}

QWebSettings *QFxWebView::settings() const
{
    return page()->settings();
}

/*!
    \internal
    \class QFxWebPage
    \brief The QFxWebPage class is a QWebPage that can create QML plugins.

    \sa QFxWebView
*/
QFxWebPage::QFxWebPage(QFxWebView *parent) :
    QWebPage(parent)
{
}

QFxWebPage::~QFxWebPage()
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
    QWidget_Dummy_Plugin(const QUrl& url, QFxWebView *view, const QStringList &paramNames, const QStringList &paramValues) :
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
            // XXX Could instead give these errors to the WebView to handle.
            foreach (QmlError err, component->errors())
                qWarning(err.toString().toLatin1());
            return;
        }
        item = qobject_cast<QFxItem*>(component->create(qmlContext(webview)));
        item->setParent(webview);
        QString jsObjName;
        for (int i=0; i<propertyNames.count(); ++i) {
            if (propertyNames[i] != QLatin1String("type") && propertyNames[i] != QLatin1String("data")) {
                item->setProperty(propertyNames[i].toLatin1(),propertyValues[i]);
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
    QFxItem *item;
    QStringList propertyNames, propertyValues;
    QFxWebView *webview;
};

QFxWebView *QFxWebPage::viewItem()
{
    return static_cast<QFxWebView*>(parent());
}

QObject *QFxWebPage::createPlugin(const QString &, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues)
{
    QUrl comp = qmlContext(viewItem())->resolvedUrl(url);
    return new QWidget_Dummy_Plugin(comp,viewItem(),paramNames,paramValues);
}

QT_END_NAMESPACE

#include "qfxwebview.moc"
