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

#ifndef QFXWEBVIEW_H
#define QFXWEBVIEW_H

#include <QtGui/QAction>
#include <QtCore/QUrl>
#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qfxpainteditem.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtWebKit/QWebPage>

QT_BEGIN_HEADER

class QWebHistory;
class QWebSettings;

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QFxWebViewPrivate;
class QNetworkRequest;
class QFxWebView;

class Q_DECLARATIVE_EXPORT QFxWebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit QFxWebPage(QFxWebView *parent);
    ~QFxWebPage();
protected:
    QObject *createPlugin(const QString &classid, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues);
private:
    QFxWebView *view();
};


class QFxWebViewAttached;
class Q_DECLARATIVE_EXPORT QFxWebView : public QFxPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QPixmap icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(qreal textSizeMultiplier READ textSizeMultiplier WRITE setTextSizeMultiplier DESIGNABLE false)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

    Q_PROPERTY(int mouseX READ mouseX)
    Q_PROPERTY(int mouseY READ mouseY)

    Q_PROPERTY(QString html READ html WRITE setHtml)

    Q_PROPERTY(int idealWidth READ idealWidth WRITE setIdealWidth NOTIFY idealWidthChanged)
    Q_PROPERTY(int idealHeight READ idealHeight WRITE setIdealHeight NOTIFY idealHeightChanged)
    Q_PROPERTY(int cacheSize READ cacheSize WRITE setCacheSize)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)

    Q_PROPERTY(QObject* reload READ reloadAction CONSTANT)
    Q_PROPERTY(QObject* back READ backAction CONSTANT)
    Q_PROPERTY(QObject* forward READ forwardAction CONSTANT)
    Q_PROPERTY(QObject* stop READ stopAction CONSTANT)

    Q_PROPERTY(QObject* settings READ settingsObject CONSTANT)

    Q_PROPERTY(QmlList<QObject *>* javaScriptWindowObjects READ javaScriptWindowObjects CONSTANT)

public:
    QFxWebView(QFxItem *parent=0);
    ~QFxWebView();

    QUrl url() const;
    void setUrl(const QUrl &);

    QString title() const;

    QPixmap icon() const;

    qreal textSizeMultiplier() const;
    void setTextSizeMultiplier(qreal);

    qreal zoomFactor() const;
    void setZoomFactor(qreal);

    bool interactive() const;
    void setInteractive(bool);

    int mouseX() const;
    int mouseY() const;

    int idealWidth() const;
    void setIdealWidth(int);
    int idealHeight() const;
    void setIdealHeight(int);


    qreal progress() const;

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
    QObject *settingsObject() const;

    QString status() const;

    int cacheSize() const;
    void setCacheSize(int pixels);

    QmlList<QObject *> *javaScriptWindowObjects();

    static QFxWebViewAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void idealWidthChanged();
    void idealHeightChanged();
    void urlChanged();
    void interactiveChanged();
    void progressChanged();
    void titleChanged(const QString&);
    void iconChanged();
    void statusChanged();
    void zoomFactorChanged();

    void loadStarted();
    void loadFinished();
    void loadFailed();

    void singleClick();
    void doubleClick();

public Q_SLOTS:
    QVariant evaluateJavaScript (const QString&);

private Q_SLOTS:
    void expandToWebPage();
    void paintPage(const QRect&);
    void doLoadProgress(int p);
    void doLoadFinished(bool ok);
    void setStatusBarMessage(const QString&);
    void windowObjectCleared();

protected:
    QFxWebView(QFxWebViewPrivate &dd, QFxItem *parent);

    void drawContents(QPainter *, const QRect &);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverMoveEvent (QGraphicsSceneHoverEvent * event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void timerEvent(QTimerEvent *event);
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    virtual void focusChanged(bool);

private:
    void init();
    virtual void componentComplete();
    Q_DISABLE_COPY(QFxWebView)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QFxWebView)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxWebView)

QT_END_HEADER

#endif
