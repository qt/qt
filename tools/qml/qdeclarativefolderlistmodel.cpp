/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "qdeclarativefolderlistmodel.h"
#include <QDirModel>
#include <QDebug>
#include <qdeclarativecontext.h>

QT_BEGIN_NAMESPACE

class QDeclarativeFolderListModelPrivate
{
public:
    QDeclarativeFolderListModelPrivate()
        : sortField(QDeclarativeFolderListModel::Name), sortReversed(false), count(0) {
        nameFilters << QLatin1String("*");
    }

    void updateSorting() {
        QDir::SortFlags flags = 0;
        switch(sortField) {
        case QDeclarativeFolderListModel::Unsorted:
            flags |= QDir::Unsorted;
            break;
        case QDeclarativeFolderListModel::Name:
            flags |= QDir::Name;
            break;
        case QDeclarativeFolderListModel::Time:
            flags |= QDir::Time;
            break;
        case QDeclarativeFolderListModel::Size:
            flags |= QDir::Size;
            break;
        case QDeclarativeFolderListModel::Type:
            flags |= QDir::Type;
            break;
        }

        if (sortReversed)
            flags |= QDir::Reversed;

        model.setSorting(flags);
    }

    QDirModel model;
    QUrl folder;
    QStringList nameFilters;
    QModelIndex folderIndex;
    QDeclarativeFolderListModel::SortField sortField;
    bool sortReversed;
    int count;
};

/*!
    \qmlclass FolderListModel
    \brief The FolderListModel provides a model of the contents of a folder in a filesystem.

    FolderListModel provides access to the local filesystem.  The \e folder property
    specifies the folder to list.

    Qt uses "/" as a universal directory separator in the same way that "/" is
    used as a path separator in URLs. If you always use "/" as a directory
    separator, Qt will translate your paths to conform to the underlying
    operating system.

    The roles available are:
    \list
    \o fileName
    \o filePath
    \endlist

    Additionally a file entry can be differentiated from a folder entry
    via the \l isFolder() method.
*/

QDeclarativeFolderListModel::QDeclarativeFolderListModel(QObject *parent)
    : QListModelInterface(parent)
{
    d = new QDeclarativeFolderListModelPrivate;
    d->model.setFilter(QDir::AllDirs | QDir::Files | QDir::Drives | QDir::NoDotAndDotDot);
    connect(&d->model, SIGNAL(rowsInserted(const QModelIndex&,int,int))
            , this, SLOT(inserted(const QModelIndex&,int,int)));
    connect(&d->model, SIGNAL(rowsRemoved(const QModelIndex&,int,int))
            , this, SLOT(removed(const QModelIndex&,int,int)));
    connect(&d->model, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&))
            , this, SLOT(dataChanged(const QModelIndex&,const QModelIndex&)));
    connect(&d->model, SIGNAL(modelReset()), this, SLOT(refresh()));
    connect(&d->model, SIGNAL(layoutChanged()), this, SLOT(refresh()));
}

QDeclarativeFolderListModel::~QDeclarativeFolderListModel()
{
    delete d;
}

QHash<int,QVariant> QDeclarativeFolderListModel::data(int index, const QList<int> &roles) const
{
    Q_UNUSED(roles);
    QHash<int,QVariant> folderData;
    QModelIndex modelIndex = d->model.index(index, 0, d->folderIndex);
    if (modelIndex.isValid()) {
        folderData[QDirModel::FileNameRole] = d->model.data(modelIndex, QDirModel::FileNameRole);
        folderData[QDirModel::FilePathRole] = QUrl::fromLocalFile(d->model.data(modelIndex, QDirModel::FilePathRole).toString());
    }

    return folderData;
}

QVariant QDeclarativeFolderListModel::data(int index, int role) const
{
    QVariant rv;
    QModelIndex modelIndex = d->model.index(index, 0, d->folderIndex);
    if (modelIndex.isValid()) {
        if (role == QDirModel::FileNameRole)
            rv = d->model.data(modelIndex, QDirModel::FileNameRole);
        else if (role == QDirModel::FilePathRole)
            rv = QUrl::fromLocalFile(d->model.data(modelIndex, QDirModel::FilePathRole).toString());
    }

    return rv;
}

int QDeclarativeFolderListModel::count() const
{
    return d->count;
}

QList<int> QDeclarativeFolderListModel::roles() const
{
    QList<int> r;
    r << QDirModel::FileNameRole;
    r << QDirModel::FilePathRole;
    return r;
}

QString QDeclarativeFolderListModel::toString(int role) const
{
    switch (role) {
    case QDirModel::FileNameRole:
        return QLatin1String("fileName");
    case QDirModel::FilePathRole:
        return QLatin1String("filePath");
    }

    return QString();
}

/*!
    \qmlproperty string FolderListModel::folder

    The \a folder property holds the folder the model is currently providing.

    It is a URL, but must be a file: or qrc: URL (or relative to such a URL).
*/
QUrl QDeclarativeFolderListModel::folder() const
{
    return d->folder;
}

void QDeclarativeFolderListModel::setFolder(const QUrl &folder)
{
    if (folder == d->folder)
        return;
    QModelIndex index = d->model.index(folder.toLocalFile());
    if (index.isValid() && d->model.isDir(index)) {
        d->folder = folder;
        QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection);
        emit folderChanged();
    }
}

QUrl QDeclarativeFolderListModel::parentFolder() const
{
    QUrl r;
    QString localFile = d->folder.toLocalFile();
    if (!localFile.isEmpty()) {
        QDir dir(localFile);
#if defined(Q_OS_SYMBIAN)
        if (dir.isRoot())
            dir.setPath("");
        else
#endif
            dir.cdUp();
        r = d->folder;
        r.setPath(dir.path());
    } else {
        int pos = d->folder.path().lastIndexOf(QLatin1Char('/'));
        if (pos == -1)
            return QUrl();
        r = d->folder;
        r.setPath(d->folder.path().left(pos));
    }
    return r;
}

/*!
    \qmlproperty list<string> FolderListModel::nameFilters

    The \a nameFilters property contains a list of filename filters.
    The filters may include the ? and * wildcards.

    The example below filters on PNG and JPEG files:

    \code
    FolderListModel {
        nameFilters: [ "*.png", "*.jpg" ]
    }
    \endcode
*/
QStringList QDeclarativeFolderListModel::nameFilters() const
{
    return d->nameFilters;
}

void QDeclarativeFolderListModel::setNameFilters(const QStringList &filters)
{
    d->nameFilters = filters;
    d->model.setNameFilters(d->nameFilters);
}

void QDeclarativeFolderListModel::componentComplete()
{
    if (!d->folder.isValid() || !QDir().exists(d->folder.toLocalFile()))
        setFolder(QUrl(QLatin1String("file://")+QDir::currentPath()));

    if (!d->folderIndex.isValid())
        QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection);
}

QDeclarativeFolderListModel::SortField QDeclarativeFolderListModel::sortField() const
{
    return d->sortField;
}

void QDeclarativeFolderListModel::setSortField(SortField field)
{
    if (field != d->sortField) {
        d->sortField = field;
        d->updateSorting();
    }
}

bool QDeclarativeFolderListModel::sortReversed() const
{
    return d->sortReversed;
}

void QDeclarativeFolderListModel::setSortReversed(bool rev)
{
    if (rev != d->sortReversed) {
        d->sortReversed = rev;
        d->updateSorting();
    }
}

/*!
    \qmlmethod bool FolderListModel::isFolder(int index)

    Returns true if the entry \a index is a folder; otherwise
    returns false.
*/
bool QDeclarativeFolderListModel::isFolder(int index) const
{
    if (index != -1) {
        QModelIndex idx = d->model.index(index, 0, d->folderIndex);
        if (idx.isValid())
            return d->model.isDir(idx);
    }
    return false;
}

void QDeclarativeFolderListModel::refresh()
{
    d->folderIndex = QModelIndex();
    if (d->count) {
        int tmpCount = d->count;
        d->count = 0;
        emit itemsRemoved(0, tmpCount);
    }
    d->folderIndex = d->model.index(d->folder.toLocalFile());
    d->count = d->model.rowCount(d->folderIndex);
    if (d->count) {
        emit itemsInserted(0, d->count);
    }
}

void QDeclarativeFolderListModel::inserted(const QModelIndex &index, int start, int end)
{
    if (index == d->folderIndex) {
        d->count = d->model.rowCount(d->folderIndex);
        emit itemsInserted(start, end - start + 1);
    }
}

void QDeclarativeFolderListModel::removed(const QModelIndex &index, int start, int end)
{
    if (index == d->folderIndex) {
        d->count = d->model.rowCount(d->folderIndex);
        emit itemsRemoved(start, end - start + 1);
    }
}

void QDeclarativeFolderListModel::dataChanged(const QModelIndex &start, const QModelIndex &end)
{
    qDebug() << "data changed";
    if (start.parent() == d->folderIndex)
        emit itemsChanged(start.row(), end.row() - start.row() + 1, roles());
}

/*!
    \qmlproperty bool FolderListModel::showDirs

    If true (the default), directories are included in the model.

    Note that the nameFilters are ignored for directories.
*/
bool QDeclarativeFolderListModel::showDirs() const
{
    return d->model.filter() & QDir::AllDirs;
}

void  QDeclarativeFolderListModel::setShowDirs(bool on)
{
    if (!(d->model.filter() & QDir::AllDirs) == !on)
        return;
    if (on)
        d->model.setFilter(d->model.filter() | QDir::AllDirs | QDir::Drives);
    else
        d->model.setFilter(d->model.filter() & ~(QDir::AllDirs | QDir::Drives));
}

/*!
    \qmlproperty bool FolderListModel::showDotAndDotDot

    If true, the "." and ".." directories are included in the model.

    The default is false.
*/
bool QDeclarativeFolderListModel::showDotAndDotDot() const
{
    return !(d->model.filter() & QDir::NoDotAndDotDot);
}

void  QDeclarativeFolderListModel::setShowDotAndDotDot(bool on)
{
    if (!(d->model.filter() & QDir::NoDotAndDotDot) == on)
        return;
    if (on)
        d->model.setFilter(d->model.filter() & ~QDir::NoDotAndDotDot);
    else
        d->model.setFilter(d->model.filter() | QDir::NoDotAndDotDot);
}

/*!
    \qmlproperty bool FolderListModel::showOnlyReadable

    If true, only readable files and directories are shown.

    The default is false.
*/
bool QDeclarativeFolderListModel::showOnlyReadable() const
{
    return d->model.filter() & QDir::Readable;
}

void QDeclarativeFolderListModel::setShowOnlyReadable(bool on)
{
    if (!(d->model.filter() & QDir::Readable) == !on)
        return;
    if (on)
        d->model.setFilter(d->model.filter() | QDir::Readable);
    else
        d->model.setFilter(d->model.filter() & ~QDir::Readable);
}

void QDeclarativeFolderListModel::registerTypes()
{
    qmlRegisterType<QDeclarativeFolderListModel>("Qt",4,7,"FolderListModel");
}

QT_END_NAMESPACE

