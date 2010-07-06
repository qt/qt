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
#include "tracer.h"

#include "bookmarkmanager.h"
#include "bookmarkmanagerwidget.h"
#include "bookmarkdialog.h"
#include "bookmarkfiltermodel.h"
#include "bookmarkitem.h"
#include "bookmarkmodel.h"
#include "centralwidget.h"
#include "helpenginewrapper.h"

#include <QtGui/QMenu>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QSortFilterProxyModel>

QT_BEGIN_NAMESPACE

// -- BookmarkManager::BookmarkWidget

void BookmarkManager::BookmarkWidget::focusInEvent(QFocusEvent *event)
{
    TRACE_OBJ
    if (event->reason() != Qt::MouseFocusReason) {
        ui.lineEdit->selectAll();
        ui.lineEdit->setFocus();

        // force the focus in event on bookmark manager
        emit focusInEvent();
    }
}

// -- BookmarkManager::BookmarkTreeView

BookmarkManager::BookmarkTreeView::BookmarkTreeView(QWidget *parent)
    : QTreeView(parent)
{
    TRACE_OBJ
    setAcceptDrops(true);
    setDragEnabled(true);
    setAutoExpandDelay(1000);
    setUniformRowHeights(true);
    setDropIndicatorShown(true);
    setExpandsOnDoubleClick(true);

    connect(this, SIGNAL(expanded(QModelIndex)), this,
        SLOT(setExpandedData(QModelIndex)));
    connect(this, SIGNAL(collapsed(QModelIndex)), this,
        SLOT(setExpandedData(QModelIndex)));

}

void BookmarkManager::BookmarkTreeView::subclassKeyPressEvent(QKeyEvent *event)
{
    TRACE_OBJ
    QTreeView::keyPressEvent(event);
}

void BookmarkManager::BookmarkTreeView::setExpandedData(const QModelIndex &index)
{
    TRACE_OBJ
    if (BookmarkModel *treeModel = qobject_cast<BookmarkModel*> (model()))
        treeModel->setData(index, isExpanded(index), UserRoleExpanded);
}

// -- BookmarkManager

QMutex BookmarkManager::mutex;
BookmarkManager* BookmarkManager::bookmarkManager = 0;

// -- public

BookmarkManager* BookmarkManager::instance()
{
    TRACE_OBJ
    if (!bookmarkManager) {
        QMutexLocker _(&mutex);
        if (!bookmarkManager)
            bookmarkManager = new BookmarkManager();
    }
    return bookmarkManager;
}

void BookmarkManager::destroy()
{
    TRACE_OBJ
    delete bookmarkManager;
    bookmarkManager = 0;
}

QWidget* BookmarkManager::bookmarkDockWidget() const
{
    TRACE_OBJ
    if (bookmarkWidget)
        return bookmarkWidget;
    return 0;
}

void BookmarkManager::takeBookmarksMenu(QMenu* menu)
{
    TRACE_OBJ
    bookmarkMenu = menu;
    refeshBookmarkMenu();
}

// -- public slots

void BookmarkManager::addBookmark(const QString &title, const QString &url)
{
    TRACE_OBJ
    showBookmarkDialog(title.isEmpty() ? tr("Untitled") : title,
        url.isEmpty() ? QLatin1String("about:blank") : url);
}

// -- private

BookmarkManager::BookmarkManager()
    : typeAndSearch(false)
    , bookmarkMenu(0)
    , bookmarkModel(new BookmarkModel)
    , bookmarkWidget(new BookmarkWidget)
    , bookmarkTreeView(new BookmarkTreeView)
    , bookmarkManagerWidget(0)
{
    TRACE_OBJ
    bookmarkWidget->installEventFilter(this);
    connect(bookmarkWidget->ui.add, SIGNAL(clicked()), this,
        SLOT(addBookmark()));
    connect(bookmarkWidget->ui.remove, SIGNAL(clicked()), this,
        SLOT(removeBookmark()));
    connect(bookmarkWidget->ui.lineEdit, SIGNAL(textChanged(QString)), this,
        SLOT(textChanged(QString)));
    connect(bookmarkWidget, SIGNAL(focusInEvent()), this, SLOT(focusInEvent()));

    bookmarkTreeView->setModel(bookmarkModel);
    bookmarkTreeView->installEventFilter(this);
    bookmarkTreeView->viewport()->installEventFilter(this);
    bookmarkTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    bookmarkWidget->ui.stackedWidget->addWidget(bookmarkTreeView);

    connect(bookmarkTreeView, SIGNAL(activated(QModelIndex)), this,
        SLOT(setSourceFromIndex(QModelIndex)));
    connect(bookmarkTreeView, SIGNAL(customContextMenuRequested(QPoint)), this,
        SLOT(customContextMenuRequested(QPoint)));

    connect(&HelpEngineWrapper::instance(), SIGNAL(setupFinished()), this,
        SLOT(setupFinished()));
    connect(bookmarkModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), this,
        SLOT(refeshBookmarkMenu()));
    connect(bookmarkModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this,
        SLOT(refeshBookmarkMenu()));
    connect(bookmarkModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this,
        SLOT(refeshBookmarkMenu()));
}

BookmarkManager::~BookmarkManager()
{
    TRACE_OBJ
    delete bookmarkManagerWidget;
    HelpEngineWrapper::instance().setBookmarks(bookmarkModel->bookmarks());
    delete bookmarkModel;
}

void BookmarkManager::removeItem(const QModelIndex &index)
{
    TRACE_OBJ
    QModelIndex current = index;
    if (typeAndSearch) { // need to map because of proxy
        current = typeAndSearchModel->mapToSource(current);
        current = bookmarkFilterModel->mapToSource(current);
    } else if (!bookmarkModel->parent(index).isValid()) {
        return;  // check if we should delete the "Bookmarks Menu", bail
    }

    if (bookmarkModel->hasChildren(current)) {
        int value = QMessageBox::question(bookmarkTreeView, tr("Remove"),
            tr("You are going to delete a Folder, this will also<br>"
            "remove it's content. Are you sure to continue?"),
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
        if (value == QMessageBox::Cancel)
            return;
    }
    bookmarkModel->removeItem(current);
}

bool BookmarkManager::eventFilter(QObject *object, QEvent *event)
{
    if (object != bookmarkTreeView && object != bookmarkTreeView->viewport()
        && object != bookmarkWidget)
            return QObject::eventFilter(object, event);

    TRACE_OBJ
    const bool isWidget = object == bookmarkWidget;
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        switch (ke->key()) {
            case Qt::Key_F2: {
                renameBookmark(bookmarkTreeView->currentIndex());
            }   break;

            case Qt::Key_Delete: {
                removeItem(bookmarkTreeView->currentIndex());
                return true;
            }   break;

            case Qt::Key_Up: {    // needs event filter on widget
            case Qt::Key_Down:
                if (isWidget)
                    bookmarkTreeView->subclassKeyPressEvent(ke);
            }   break;

            case Qt::Key_Escape: {
                emit escapePressed();
            }   break;

            default: break;
        }
    }

    if (event->type() == QEvent::MouseButtonRelease && !isWidget) {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        switch (me->button()) {
            case Qt::LeftButton: {
                if (me->modifiers() & Qt::ControlModifier)
                    setSourceFromIndex(bookmarkTreeView->currentIndex(), true);
            }   break;

            case Qt::MidButton: {
                setSourceFromIndex(bookmarkTreeView->currentIndex(), true);
            }   break;

            default: break;
        }
    }

    return QObject::eventFilter(object, event);
}

void BookmarkManager::buildBookmarksMenu(const QModelIndex &index, QMenu* menu)
{
    TRACE_OBJ
    if (!index.isValid())
        return;

    const QString &text = index.data().toString();
    const QIcon &icon = qVariantValue<QIcon>(index.data(Qt::DecorationRole));
    if (index.data(UserRoleFolder).toBool()) {
        if (QMenu* subMenu = menu->addMenu(icon, text)) {
            for (int i = 0; i < bookmarkModel->rowCount(index); ++i)
                buildBookmarksMenu(bookmarkModel->index(i, 0, index), subMenu);
        }
    } else {
        QAction *action = menu->addAction(icon, text);
        action->setData(index.data(UserRoleUrl).toString());
    }
}

void BookmarkManager::showBookmarkDialog(const QString &name, const QString &url)
{
    TRACE_OBJ
    BookmarkDialog dialog(bookmarkModel, name, url, bookmarkTreeView);
    dialog.exec();
}

// -- private slots

void BookmarkManager::setupFinished()
{
    TRACE_OBJ
    bookmarkModel->setBookmarks(HelpEngineWrapper::instance().bookmarks());
    bookmarkModel->expandFoldersIfNeeeded(bookmarkTreeView);

    refeshBookmarkMenu();

    bookmarkTreeView->hideColumn(1);
    bookmarkTreeView->header()->setVisible(false);
    bookmarkTreeView->header()->setStretchLastSection(true);

    bookmarkFilterModel = new BookmarkFilterModel(this);
    bookmarkFilterModel->setSourceModel(bookmarkModel);
    bookmarkFilterModel->filterBookmarkFolders();

    typeAndSearchModel = new QSortFilterProxyModel(this);
    typeAndSearchModel->setDynamicSortFilter(true);
    typeAndSearchModel->setSourceModel(bookmarkFilterModel);
}

void BookmarkManager::addBookmark()
{
    TRACE_OBJ
    if (CentralWidget *widget = CentralWidget::instance())
        addBookmark(widget->currentTitle(), widget->currentSource().toString());
}

void BookmarkManager::removeBookmark()
{
    TRACE_OBJ
    removeItem(bookmarkTreeView->currentIndex());
}

void BookmarkManager::manageBookmarks()
{
    TRACE_OBJ
    if (bookmarkManagerWidget == 0) {
        bookmarkManagerWidget = new BookmarkManagerWidget(bookmarkModel);
        connect(bookmarkManagerWidget, SIGNAL(setSource(QUrl)), this,
            SIGNAL(setSource(QUrl)));
        connect(bookmarkManagerWidget, SIGNAL(setSourceInNewTab(QUrl))
            , this, SIGNAL(setSourceInNewTab(QUrl)));
        connect(bookmarkManagerWidget, SIGNAL(managerWidgetAboutToClose())
            , this, SLOT(managerWidgetAboutToClose()));
    }
    bookmarkManagerWidget->show();
    bookmarkManagerWidget->raise();
}

void BookmarkManager::refeshBookmarkMenu()
{
    TRACE_OBJ
    if (!bookmarkMenu)
        return;

    bookmarkMenu->clear();

    bookmarkMenu->addAction(tr("Manage Bookmarks..."), this,
        SLOT(manageBookmarks()));
    bookmarkMenu->addAction(QIcon::fromTheme("bookmark-new"),
                            tr("Add Bookmark..."), this, SLOT(addBookmark()),
                            QKeySequence(tr("Ctrl+D")));
    bookmarkMenu->addSeparator();

    const QModelIndex &root = bookmarkModel->index(0, 0, QModelIndex());
    for (int i = 0; i < bookmarkModel->rowCount(root); ++i)
        buildBookmarksMenu(bookmarkModel->index(i, 0, root), bookmarkMenu);

    connect(bookmarkMenu, SIGNAL(triggered(QAction*)), this,
        SLOT(setSourceFromAction(QAction*)));
}

void BookmarkManager::renameBookmark(const QModelIndex &index)
{
    // check if we should rename the "Bookmarks Menu", bail
    if (!typeAndSearch && !bookmarkModel->parent(index).isValid())
        return;

    bookmarkModel->setItemsEditable(true);
    bookmarkTreeView->edit(index);
    bookmarkModel->setItemsEditable(false);
}

void BookmarkManager::setSourceFromAction(QAction *action)
{
    TRACE_OBJ
    const QVariant &data = action->data();

    if (data.canConvert<QUrl>())
        emit setSource(data.toUrl());
}

void BookmarkManager::setSourceFromIndex(const QModelIndex &index, bool newTab)
{
    TRACE_OBJ
    QAbstractItemModel *base = bookmarkModel;
    if (typeAndSearch)
        base = typeAndSearchModel;

    if (base->data(index, UserRoleFolder).toBool())
        return;

    const QVariant &data = base->data(index, UserRoleUrl);
    if (data.canConvert<QUrl>()) {
        if (newTab)
            emit setSourceInNewTab(data.toUrl());
        else
            emit setSource(data.toUrl());
    }
}

void BookmarkManager::customContextMenuRequested(const QPoint &point)
{
    TRACE_OBJ
    QModelIndex index = bookmarkTreeView->indexAt(point);
    if (!index.isValid())
        return;

    // check if we should open the menu on "Bookmarks Menu", bail
    if (!typeAndSearch && !bookmarkModel->parent(index).isValid())
        return;

    QAction *remove = 0;
    QAction *rename = 0;
    QAction *showItem = 0;
    QAction *showItemInNewTab = 0;

    QMenu menu(QLatin1String(""));
    if (!typeAndSearch && bookmarkModel->data(index, UserRoleFolder).toBool()) {
        remove = menu.addAction(tr("Delete Folder"));
        rename = menu.addAction(tr("Rename Folder"));
    } else {
        showItem = menu.addAction(tr("Show Bookmark"));
        showItemInNewTab = menu.addAction(tr("Show Bookmark in New Tab"));
        menu.addSeparator();
        remove = menu.addAction(tr("Delete Bookmark"));
        rename = menu.addAction(tr("Rename Bookmark"));
    }

    QAction *pickedAction = menu.exec(bookmarkTreeView->mapToGlobal(point));
    if (pickedAction == rename)
        renameBookmark(index);
    else if (pickedAction == remove)
        removeItem(index);
    else if (pickedAction == showItem || pickedAction == showItemInNewTab)
        setSourceFromIndex(index, pickedAction == showItemInNewTab);
}

void BookmarkManager::focusInEvent()
{
    TRACE_OBJ
    const QModelIndex &index = bookmarkTreeView->indexAt(QPoint(2, 2));
    if (index.isValid())
        bookmarkTreeView->setCurrentIndex(index);
}

void BookmarkManager::managerWidgetAboutToClose()
{
    delete bookmarkManagerWidget;
    bookmarkManagerWidget = 0;
}

void BookmarkManager::textChanged(const QString &text)
{
    TRACE_OBJ
    if (!bookmarkWidget->ui.lineEdit->text().isEmpty()) {
        if (!typeAndSearch) {
            typeAndSearch = true;
            bookmarkTreeView->setItemsExpandable(false);
            bookmarkTreeView->setRootIsDecorated(false);
            bookmarkTreeView->setModel(typeAndSearchModel);
        }
        typeAndSearchModel->setFilterRegExp(QRegExp(text));
    } else {
        typeAndSearch = false;
        bookmarkTreeView->setModel(bookmarkModel);
        bookmarkTreeView->setItemsExpandable(true);
        bookmarkTreeView->setRootIsDecorated(true);
        bookmarkModel->expandFoldersIfNeeeded(bookmarkTreeView);
    }
}

QT_END_NAMESPACE
