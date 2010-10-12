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
#ifndef BOOKMARKMANAGERWIDGET_H
#define BOOKMARKMANAGERWIDGET_H

#include "ui_bookmarkmanagerwidget.h"

#include <QtCore/QPersistentModelIndex>

#include <QtGui/QMenu>

QT_BEGIN_NAMESPACE

class BookmarkModel;
class QCloseEvent;
class QString;

class BookmarkManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BookmarkManagerWidget(BookmarkModel *bookmarkModel,
        QWidget *parent = 0);
    ~BookmarkManagerWidget();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void setSource(const QUrl &url);
    void setSourceInNewTab(const QUrl &url);

    void managerWidgetAboutToClose();

private:
    void renameItem(const QModelIndex &index);
    void selectNextIndex(bool direction) const;
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void findNext();
    void findPrevious();

    void importBookmarks();
    void exportBookmarks();

    void refeshBookmarkCache();
    void textChanged(const QString &text);

    void removeItem(const QModelIndex &index = QModelIndex());

    void customContextMenuRequested(const QPoint &point);
    void setSourceFromIndex(const QModelIndex &index, bool newTab = false);

private:
    QMenu importExportMenu;
    Ui::BookmarkManagerWidget ui;
    QList<QPersistentModelIndex> cache;

    BookmarkModel *bookmarkModel;
};

QT_END_NAMESPACE

#endif  // BOOKMARKMANAGERWIDGET_H
