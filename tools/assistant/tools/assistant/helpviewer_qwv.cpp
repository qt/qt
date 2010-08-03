/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "helpviewer_qwv.h"

#include "centralwidget.h"
#include "helpenginewrapper.h"
#include "tracer.h"

#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QStringBuilder>
#include <QtCore/QTimer>

#include <QtGui/QWheelEvent>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

QT_BEGIN_NAMESPACE

class HelpNetworkReply : public QNetworkReply
{
public:
    HelpNetworkReply(const QNetworkRequest &request, const QByteArray &fileData,
        const QString &mimeType);

    virtual void abort();

    virtual qint64 bytesAvailable() const
        { return data.length() + QNetworkReply::bytesAvailable(); }

protected:
    virtual qint64 readData(char *data, qint64 maxlen);

private:
    QByteArray data;
    qint64 origLen;
};

HelpNetworkReply::HelpNetworkReply(const QNetworkRequest &request,
        const QByteArray &fileData, const QString& mimeType)
    : data(fileData), origLen(fileData.length())
{
    TRACE_OBJ
    setRequest(request);
    setOpenMode(QIODevice::ReadOnly);

    setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
    setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(origLen));
    QTimer::singleShot(0, this, SIGNAL(metaDataChanged()));
    QTimer::singleShot(0, this, SIGNAL(readyRead()));
}

void HelpNetworkReply::abort()
{
    TRACE_OBJ
}

qint64 HelpNetworkReply::readData(char *buffer, qint64 maxlen)
{
    TRACE_OBJ
    qint64 len = qMin(qint64(data.length()), maxlen);
    if (len) {
        memcpy(buffer, data.constData(), len);
        data.remove(0, len);
    }
    if (!data.length())
        QTimer::singleShot(0, this, SIGNAL(finished()));
    return len;
}

class HelpNetworkAccessManager : public QNetworkAccessManager
{
public:
    HelpNetworkAccessManager(QObject *parent);

protected:
    virtual QNetworkReply *createRequest(Operation op,
        const QNetworkRequest &request, QIODevice *outgoingData = 0);
};

HelpNetworkAccessManager::HelpNetworkAccessManager(QObject *parent)
    : QNetworkAccessManager(parent)
{
    TRACE_OBJ
}

QNetworkReply *HelpNetworkAccessManager::createRequest(Operation /*op*/,
    const QNetworkRequest &request, QIODevice* /*outgoingData*/)
{
    TRACE_OBJ
    QString url = request.url().toString();
    HelpEngineWrapper &helpEngine = HelpEngineWrapper::instance();

    // TODO: For some reason the url to load is already wrong (passed from webkit)
    // though the css file and the references inside should work that way. One 
    // possible problem might be that the css is loaded at the same level as the
    // html, thus a path inside the css like (../images/foo.png) might cd out of
    // the virtual folder
    if (!helpEngine.findFile(url).isValid()) {
        if (url.startsWith(AbstractHelpViewer::DocPath)) {
            QUrl newUrl = request.url();
            if (!newUrl.path().startsWith(QLatin1String("/qdoc/"))) {
                newUrl.setPath(QLatin1String("qdoc") + newUrl.path());
                url = newUrl.toString();
            }
        }
    }

    const QString &mimeType = AbstractHelpViewer::mimeFromUrl(url);
    const QByteArray &data = helpEngine.findFile(url).isValid()
        ? helpEngine.fileData(url)
        : AbstractHelpViewer::PageNotFoundMessage.arg(url).toUtf8();
    return new HelpNetworkReply(request, data, mimeType.isEmpty()
        ? QLatin1String("application/octet-stream") : mimeType);
}

class HelpPage : public QWebPage
{
public:
    HelpPage(CentralWidget *central, QObject *parent);

protected:
    virtual QWebPage *createWindow(QWebPage::WebWindowType);
    virtual void triggerAction(WebAction action, bool checked = false);

    virtual bool acceptNavigationRequest(QWebFrame *frame,
        const QNetworkRequest &request, NavigationType type);

private:
    CentralWidget *centralWidget;
    bool closeNewTabIfNeeded;

    friend class HelpViewer;
    QUrl m_loadingUrl;
    Qt::MouseButtons m_pressedButtons;
    Qt::KeyboardModifiers m_keyboardModifiers;
};

HelpPage::HelpPage(CentralWidget *central, QObject *parent)
    : QWebPage(parent)
    , centralWidget(central)
    , closeNewTabIfNeeded(false)
    , m_pressedButtons(Qt::NoButton)
    , m_keyboardModifiers(Qt::NoModifier)
{
    TRACE_OBJ
}

QWebPage *HelpPage::createWindow(QWebPage::WebWindowType)
{
    TRACE_OBJ
    HelpPage* newPage = static_cast<HelpPage*>(centralWidget->newEmptyTab()->page());
    if (newPage)
        newPage->closeNewTabIfNeeded = closeNewTabIfNeeded;
    closeNewTabIfNeeded = false;
    return newPage;
}

void HelpPage::triggerAction(WebAction action, bool checked)
{
    TRACE_OBJ
    switch (action) {
        case OpenLinkInNewWindow:
            closeNewTabIfNeeded = true;
        default:        // fall through
            QWebPage::triggerAction(action, checked);
            break;
    }
}

bool HelpPage::acceptNavigationRequest(QWebFrame *,
    const QNetworkRequest &request, QWebPage::NavigationType type)
{
    TRACE_OBJ
    const bool closeNewTab = closeNewTabIfNeeded;
    closeNewTabIfNeeded = false;

    const QUrl &url = request.url();
    if (AbstractHelpViewer::launchWithExternalApp(url)) {
        if (closeNewTab)
            QMetaObject::invokeMethod(centralWidget, "closeTab");
        return false;
    }

    if (type == QWebPage::NavigationTypeLinkClicked
        && (m_keyboardModifiers & Qt::ControlModifier
        || m_pressedButtons == Qt::MidButton)) {
            if (centralWidget->newEmptyTab())
                centralWidget->setSource(url);
            m_pressedButtons = Qt::NoButton;
            m_keyboardModifiers = Qt::NoModifier;
            return false;
    }

    m_loadingUrl = url; // because of async page loading, we will hit some kind
    // of race condition while using a remote command, like a combination of
    // SetSource; SyncContent. SetSource would be called and SyncContents shortly
    // afterwards, but the page might not have finished loading and the old url
    // would be returned.
    return true;
}

// -- HelpViewer

HelpViewer::HelpViewer(CentralWidget *parent, qreal zoom)
    : QWebView(parent)
    , parentWidget(parent)
    , loadFinished(false)
    , helpEngine(HelpEngineWrapper::instance())
{
    TRACE_OBJ
    setAcceptDrops(false);

    setPage(new HelpPage(parent, this));

    page()->setNetworkAccessManager(new HelpNetworkAccessManager(this));

    QAction* action = pageAction(QWebPage::OpenLinkInNewWindow);
    action->setText(tr("Open Link in New Tab"));
    if (!parent)
        action->setVisible(false);

    pageAction(QWebPage::DownloadLinkToDisk)->setVisible(false);
    pageAction(QWebPage::DownloadImageToDisk)->setVisible(false);
    pageAction(QWebPage::OpenImageInNewWindow)->setVisible(false);

    connect(pageAction(QWebPage::Copy), SIGNAL(changed()), this,
        SLOT(actionChanged()));
    connect(pageAction(QWebPage::Back), SIGNAL(changed()), this,
        SLOT(actionChanged()));
    connect(pageAction(QWebPage::Forward), SIGNAL(changed()), this,
        SLOT(actionChanged()));
    connect(page(), SIGNAL(linkHovered(QString,QString,QString)), this,
        SIGNAL(highlighted(QString)));
    connect(this, SIGNAL(urlChanged(QUrl)), this, SIGNAL(sourceChanged(QUrl)));
    connect(this, SIGNAL(loadStarted()), this, SLOT(setLoadStarted()));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(setLoadFinished(bool)));
    connect(page(), SIGNAL(printRequested(QWebFrame*)), this, SIGNAL(printRequested()));

    setFont(viewerFont());
    setTextSizeMultiplier(zoom == 0.0 ? 1.0 : zoom);
}

HelpViewer::~HelpViewer()
{
    TRACE_OBJ
}

QFont HelpViewer::viewerFont() const
{
    TRACE_OBJ
    if (helpEngine.usesBrowserFont())
        return helpEngine.browserFont();

    QWebSettings *webSettings = QWebSettings::globalSettings();
    return QFont(webSettings->fontFamily(QWebSettings::StandardFont),
        webSettings->fontSize(QWebSettings::DefaultFontSize));
}

void HelpViewer::setViewerFont(const QFont &font)
{
    TRACE_OBJ
    QWebSettings *webSettings = settings();
    webSettings->setFontFamily(QWebSettings::StandardFont, font.family());
    webSettings->setFontSize(QWebSettings::DefaultFontSize, font.pointSize());
}

void HelpViewer::scaleUp()
{
    TRACE_OBJ
    setTextSizeMultiplier(textSizeMultiplier() + 0.1);
}

void HelpViewer::scaleDown()
{
    TRACE_OBJ
    setTextSizeMultiplier(qMax(0.0, textSizeMultiplier() - 0.1));
}

void HelpViewer::resetScale()
{
    TRACE_OBJ
    setTextSizeMultiplier(1.0);
}

bool HelpViewer::handleForwardBackwardMouseButtons(QMouseEvent *e)
{
    TRACE_OBJ
    if (e->button() == Qt::XButton1) {
        triggerPageAction(QWebPage::Back);
        return true;
    }

    if (e->button() == Qt::XButton2) {
        triggerPageAction(QWebPage::Forward);
        return true;
    }

    return false;
}

QUrl HelpViewer::source() const
{
    HelpPage *currentPage = static_cast<HelpPage*> (page());
    if (currentPage && !hasLoadFinished()) {
        // see HelpPage::acceptNavigationRequest(...)
        return currentPage->m_loadingUrl;
    }
    return url();
}

void HelpViewer::setSource(const QUrl &url)
{
    TRACE_OBJ
    load(url.toString() == QLatin1String("help") ? LocalHelpFile : url);
}

void HelpViewer::home()
{
    TRACE_OBJ
    setSource(helpEngine.homePage());
}

void HelpViewer::wheelEvent(QWheelEvent *e)
{
    TRACE_OBJ
    if (e->modifiers()& Qt::ControlModifier) {
        e->accept();
        e->delta() > 0 ? scaleUp() : scaleDown();
    } else {
        QWebView::wheelEvent(e);
    }
}

void HelpViewer::mouseReleaseEvent(QMouseEvent *e)
{
    TRACE_OBJ
#ifndef Q_OS_LINUX
    if (handleForwardBackwardMouseButtons(e))
        return;
#endif

    QWebView::mouseReleaseEvent(e);
}

void HelpViewer::actionChanged()
{
    TRACE_OBJ
    QAction *a = qobject_cast<QAction *>(sender());
    if (a == pageAction(QWebPage::Copy))
        emit copyAvailable(a->isEnabled());
    else if (a == pageAction(QWebPage::Back))
        emit backwardAvailable(a->isEnabled());
    else if (a == pageAction(QWebPage::Forward))
        emit forwardAvailable(a->isEnabled());
}

void HelpViewer::mousePressEvent(QMouseEvent *event)
{
    TRACE_OBJ
#ifdef Q_OS_LINUX
    if (handleForwardBackwardMouseButtons(event))
        return;
#endif

    HelpPage *currentPage = static_cast<HelpPage*>(page());
    if (currentPage) {
        currentPage->m_pressedButtons = event->buttons();
        currentPage->m_keyboardModifiers = event->modifiers();
    }
    QWebView::mousePressEvent(event);
}

void HelpViewer::setLoadStarted()
{
    loadFinished = false;
}

void HelpViewer::setLoadFinished(bool ok)
{
    TRACE_OBJ
    loadFinished = ok;
    emit sourceChanged(url());
}

QT_END_NAMESPACE
