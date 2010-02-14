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

#ifndef QMLGRAPHICSWEBVIEW_H
#define QMLGRAPHICSWEBVIEW_H

#include "qmlgraphicspainteditem_p.h"

#include <QtGui/QAction>
#include <QtCore/QUrl>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtWebKit/QWebPage>

QT_BEGIN_HEADER

class QWebHistory;
class QWebSettings;

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QmlGraphicsWebViewPrivate;
class QNetworkRequest;
class QmlGraphicsWebView;

class Q_DECLARATIVE_EXPORT QmlGraphicsWebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit QmlGraphicsWebPage(QmlGraphicsWebView *parent);
    ~QmlGraphicsWebPage();
protected:
    QObject *createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
    QWebPage *createWindow(WebWindowType type);
    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID);
    QString chooseFile(QWebFrame *originatingFrame, const QString& oldFile);
    void javaScriptAlert(QWebFrame *originatingFrame, const QString& msg);
    bool javaScriptConfirm(QWebFrame *originatingFrame, const QString& msg);
    bool javaScriptPrompt(QWebFrame *originatingFrame, const QString& msg, const QString& defaultValue, QString* result);

private:
    QmlGraphicsWebView *viewItem();
};


class QmlGraphicsWebViewAttached;
class QmlGraphicsWebSettings;

//### TODO: browser plugins

class Q_DECLARATIVE_EXPORT QmlGraphicsWebView : public QmlGraphicsPaintedItem
{
    Q_OBJECT

    Q_ENUMS(Status SelectionMode)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QPixmap icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

    Q_PROPERTY(QString html READ html WRITE setHtml)

    Q_PROPERTY(int pressGrabTime READ pressGrabTime WRITE setPressGrabTime)

    Q_PROPERTY(int preferredWidth READ preferredWidth WRITE setPreferredWidth NOTIFY preferredWidthChanged)
    Q_PROPERTY(int preferredHeight READ preferredHeight WRITE setPreferredHeight NOTIFY preferredHeightChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_PROPERTY(QAction* reload READ reloadAction CONSTANT)
    Q_PROPERTY(QAction* back READ backAction CONSTANT)
    Q_PROPERTY(QAction* forward READ forwardAction CONSTANT)
    Q_PROPERTY(QAction* stop READ stopAction CONSTANT)

    Q_PROPERTY(QmlGraphicsWebSettings* settings READ settingsObject CONSTANT)

    Q_PROPERTY(QmlList<QObject *>* javaScriptWindowObjects READ javaScriptWindowObjects CONSTANT)

    Q_PROPERTY(QmlComponent* newWindowComponent READ newWindowComponent WRITE setNewWindowComponent)
    Q_PROPERTY(QmlGraphicsItem* newWindowParent READ newWindowParent WRITE setNewWindowParent)

    Q_PROPERTY(bool renderingEnabled READ renderingEnabled WRITE setRenderingEnabled)

public:
    QmlGraphicsWebView(QmlGraphicsItem *parent=0);
    ~QmlGraphicsWebView();

    QUrl url() const;
    void setUrl(const QUrl &);

    QString title() const;

    QPixmap icon() const;

    qreal zoomFactor() const;
    void setZoomFactor(qreal);
    Q_INVOKABLE bool heuristicZoom(int clickX, int clickY, qreal maxzoom);
    QRect elementAreaAt(int x, int y, int minwidth, int minheight) const;

    int pressGrabTime() const;
    void setPressGrabTime(int);

    int preferredWidth() const;
    void setPreferredWidth(int);
    int preferredHeight() const;
    void setPreferredHeight(int);

    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;
    QString statusText() const;

    QAction *reloadAction() const;
    QAction *backAction() const;
    QAction *forwardAction() const;
    QAction *stopAction() const;

    QWebPage *page() const;
    void setPage(QWebPage *page);

    void load(const QNetworkRequest &request,
              QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
              const QByteArray &body = QByteArray());

    QString html() const;

    void setHtml(const QString &html, const QUrl &baseUrl = QUrl());
    void setContent(const QByteArray &data, const QString &mimeType = QString(), const QUrl &baseUrl = QUrl());

    QWebHistory *history() const;
    QWebSettings *settings() const;
    QmlGraphicsWebSettings *settingsObject() const;

    bool renderingEnabled() const;
    void setRenderingEnabled(bool);

    QmlList<QObject *> *javaScriptWindowObjects();

    static QmlGraphicsWebViewAttached *qmlAttachedProperties(QObject *);

    QmlComponent *newWindowComponent() const;
    void setNewWindowComponent(QmlComponent *newWindow);
    QmlGraphicsItem *newWindowParent() const;
    void setNewWindowParent(QmlGraphicsItem *newWindow);

Q_SIGNALS:
    void preferredWidthChanged();
    void preferredHeightChanged();
    void urlChanged();
    void progressChanged();
    void statusChanged(Status);
    void titleChanged(const QString&);
    void iconChanged();
    void statusTextChanged();
    void zoomFactorChanged();

    void loadStarted();
    void loadFinished();
    void loadFailed();

    void doubleClick(int clickX, int clickY);

    void zoomTo(qreal zoom, int centerX, int centerY);

    void alert(const QString& message);

public Q_SLOTS:
    QVariant evaluateJavaScript(const QString&);

private Q_SLOTS:
    void expandToWebPage();
    void paintPage(const QRect&);
    void doLoadStarted();
    void doLoadProgress(int p);
    void doLoadFinished(bool ok);
    void setStatusText(const QString&);
    void windowObjectCleared();
    void pageUrlChanged();
    void noteContentsSizeChanged(const QSize&);
    void initialLayout();

protected:
    void drawContents(QPainter *, const QRect &);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void timerEvent(QTimerEvent *event);
    void hoverMoveEvent (QGraphicsSceneHoverEvent * event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    virtual void focusChanged(bool);
    virtual bool sceneEvent(QEvent *event);
    QmlGraphicsWebView *createWindow(QWebPage::WebWindowType type);

private:
    void init();
    virtual void componentComplete();
    Q_DISABLE_COPY(QmlGraphicsWebView)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsWebView)
    QMouseEvent *sceneMouseEventToMouseEvent(QGraphicsSceneMouseEvent *);
    QMouseEvent *sceneHoverMoveEventToMouseEvent(QGraphicsSceneHoverEvent *);
    friend class QmlGraphicsWebPage;
};

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


QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsWebView)
QML_DECLARE_TYPEINFO(QmlGraphicsWebView, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QAction)

QT_END_HEADER

#endif
