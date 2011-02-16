/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QtCore/QUrl>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class FindWidget;
class HelpViewer;
class MainWindow;
class QHelpSearchEngine;
class QTabWidget;
class SearchWidget;

class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    CentralWidget(MainWindow *parent);
    ~CentralWidget();

    void setupWidget();
    bool hasSelection() const;
    QUrl currentSource() const;
    QString currentTitle() const;
    bool isHomeAvailable() const;
    bool isForwardAvailable() const;
    bool isBackwardAvailable() const;
    QList<QAction*> globalActions() const;
    void setGlobalActions(const QList<QAction*> &actions);

    HelpViewer *viewerAt(int index) const;
    HelpViewer *currentHelpViewer() const;

    bool searchWidgetAttached() const;
    void createSearchWidget(QHelpSearchEngine *searchEngine);
    void activateSearchWidget(bool updateLastTabPage = false);
    void removeSearchWidget();

    int availableHelpViewer() const;
    bool enableTabCloseAction() const;

    void closeOrReloadTabs(const QList<int> &indices, bool tryReload);
    void closeTabAt(int index);
    QMap<int, QString> currentSourceFileList() const;

    static CentralWidget *instance();

public slots:
    void zoomIn();
    void zoomOut();
    void nextPage();
    void resetZoom();
    void previousPage();
    void copySelection();
    void showTextSearch();
    void print();
    void pageSetup();
    void printPreview();
    void updateBrowserFont();
    void setSource(const QUrl &url);
    void setSourceInNewTab(const QUrl &url, qreal zoom = 0.0);
    HelpViewer *newEmptyTab();
    void home();
    void forward();
    void backward();

    void activateTab(bool onlyHelpViewer = false);

    void findNext();
    void findPrevious();
    void find(const QString &text, bool forward);

signals:
    void currentViewerChanged();
    void copyAvailable(bool yes);
    void sourceChanged(const QUrl &url);
    void highlighted(const QString &link);
    void forwardAvailable(bool available);
    void backwardAvailable(bool available);
    void addBookmark(const QString &title, const QString &url);

protected:
    void keyPressEvent(QKeyEvent *);

private slots:
    void newTab();
    void closeTab();
    void setTabTitle(const QUrl& url);
    void currentPageChanged(int index);
    void showTabBarContextMenu(const QPoint &point);
    void printPreview(QPrinter *printer);
    void setSourceFromSearch(const QUrl &url);
    void setSourceFromSearchInNewTab(const QUrl &url);
    void highlightSearchTerms();

private:
    void connectSignals();
    bool eventFilter(QObject *object, QEvent *e);
    bool findInWebPage(const QString &ttf, bool forward);
    bool findInTextBrowser(const QString &ttf, bool forward);
    void initPrinter();
    QString quoteTabTitle(const QString &title) const;
    void setLastShownPages();

private:
    int lastTabPage;
    QList<QAction*> globalActionList;

    QTabWidget *tabWidget;
    FindWidget *findWidget;
    QPrinter *printer;
    bool usesDefaultCollection;

    SearchWidget *m_searchWidget;
};

QT_END_NAMESPACE

#endif  // CENTRALWIDGET_H
