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

#include "private/qobject_p.h"
#include <QDirModel>
#include <qdebug.h>
#include "qmlfolderlistmodel.h"

QT_BEGIN_NAMESPACE

class QmlFolderListModelPrivate : public QObjectPrivate
{
public:
    QmlFolderListModelPrivate()
        : sortField(QmlFolderListModel::Name), sortReversed(false), count(0) {
        folder = QDir::currentPath();
        nameFilters << QLatin1String("*");
    }

    void updateSorting() {
        QDir::SortFlags flags = 0;
        switch(sortField) {
        case QmlFolderListModel::Unsorted:
            flags |= QDir::Unsorted;
            break;
        case QmlFolderListModel::Name:
            flags |= QDir::Name;
            break;
        case QmlFolderListModel::Time:
            flags |= QDir::Time;
            break;
        case QmlFolderListModel::Size:
            flags |= QDir::Size;
            break;
        case QmlFolderListModel::Type:
            flags |= QDir::Type;
            break;
        }

        if (sortReversed)
            flags |= QDir::Reversed;

        model.setSorting(flags);
    }

    QDirModel model;
    QString folder;
    QStringList nameFilters;
    QModelIndex folderIndex;
    QmlFolderListModel::SortField sortField;
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

QmlFolderListModel::QmlFolderListModel(QObject *parent)
    : QListModelInterface(*(new QmlFolderListModelPrivate), parent)
{
    Q_D(QmlFolderListModel);
    d->model.setFilter(QDir::AllDirs | QDir::Files | QDir::Drives);
    connect(&d->model, SIGNAL(rowsInserted(const QModelIndex&,int,int))
            , this, SLOT(inserted(const QModelIndex&,int,int)));
    connect(&d->model, SIGNAL(rowsRemoved(const QModelIndex&,int,int))
            , this, SLOT(removed(const QModelIndex&,int,int)));
    connect(&d->model, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&))
            , this, SLOT(dataChanged(const QModelIndex&,const QModelIndex&)));
    connect(&d->model, SIGNAL(modelReset()), this, SLOT(refresh()));
    connect(&d->model, SIGNAL(layoutChanged()), this, SLOT(refresh()));
}

QmlFolderListModel::~QmlFolderListModel()
{
}

QHash<int,QVariant> QmlFolderListModel::data(int index, const QList<int> &roles) const
{
    Q_UNUSED(roles);
    Q_D(const QmlFolderListModel);
    QHash<int,QVariant> folderData;
    QModelIndex modelIndex = d->model.index(index, 0, d->folderIndex);
    if (modelIndex.isValid()) {
        folderData[QDirModel::FileNameRole] = d->model.data(modelIndex, QDirModel::FileNameRole);
        folderData[QDirModel::FilePathRole] = d->model.data(modelIndex, QDirModel::FilePathRole);
    }

    return folderData;
}

int QmlFolderListModel::count() const
{
    Q_D(const QmlFolderListModel);
    return d->count;
}

QList<int> QmlFolderListModel::roles() const
{
    QList<int> r;
    r << QDirModel::FileNameRole;
    r << QDirModel::FilePathRole;
    return r;
}

QString QmlFolderListModel::toString(int role) const
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
*/
QString QmlFolderListModel::folder() const
{
    Q_D(const QmlFolderListModel);
    return d->folder;
}

void QmlFolderListModel::setFolder(const QString &folder)
{
    Q_D(QmlFolderListModel);
    if (folder == d->folder)
        return;
    QModelIndex index = d->model.index(folder);
    if (index.isValid() && d->model.isDir(index)) {
        d->folder = folder;
        QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection);
        emit folderChanged();
    }
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
QStringList QmlFolderListModel::nameFilters() const
{
    Q_D(const QmlFolderListModel);
    return d->nameFilters;
}

void QmlFolderListModel::setNameFilters(const QStringList &filters)
{
    Q_D(QmlFolderListModel);
    d->nameFilters = filters;
    d->model.setNameFilters(d->nameFilters);
}

void QmlFolderListModel::classComplete()
{
    Q_D(QmlFolderListModel);
    if (!d->folderIndex.isValid())
        QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection);
}

QmlFolderListModel::SortField QmlFolderListModel::sortField() const
{
    Q_D(const QmlFolderListModel);
    return d->sortField;
}

void QmlFolderListModel::setSortField(SortField field)
{
    Q_D(QmlFolderListModel);
    if (field != d->sortField) {
        d->sortField = field;
        d->updateSorting();
    }
}

bool QmlFolderListModel::sortReversed() const
{
    Q_D(const QmlFolderListModel);
    return d->sortReversed;
}

void QmlFolderListModel::setSortReversed(bool rev)
{
    Q_D(QmlFolderListModel);
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
bool QmlFolderListModel::isFolder(int index) const
{
    Q_D(const QmlFolderListModel);
    if (index != -1) {
        QModelIndex idx = d->model.index(index, 0, d->folderIndex);
        if (idx.isValid())
            return d->model.isDir(idx);
    }
    return false;
}

void QmlFolderListModel::refresh()
{
    Q_D(QmlFolderListModel);
    d->folderIndex = QModelIndex();
    if (d->count) {
        int tmpCount = d->count;
        d->count = 0;
        emit itemsRemoved(0, tmpCount);
    }
    d->folderIndex = d->model.index(d->folder);
    d->count = d->model.rowCount(d->folderIndex);
    if (d->count) {
        emit itemsInserted(0, d->count);
    }
}

void QmlFolderListModel::inserted(const QModelIndex &index, int start, int end)
{
    Q_D(QmlFolderListModel);
    if (index == d->folderIndex) {
        d->count = d->model.rowCount(d->folderIndex);
        emit itemsInserted(start, end - start + 1);
    }
}

void QmlFolderListModel::removed(const QModelIndex &index, int start, int end)
{
    Q_D(QmlFolderListModel);
    if (index == d->folderIndex) {
        d->count = d->model.rowCount(d->folderIndex);
        emit itemsRemoved(start, end - start + 1);
    }
}

void QmlFolderListModel::dataChanged(const QModelIndex &start, const QModelIndex &end)
{
    Q_D(QmlFolderListModel);
    qDebug() << "data changed";
    if (start.parent() == d->folderIndex)
        emit itemsChanged(start.row(), end.row() - start.row() + 1, roles());
}

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,FolderListModel,QmlFolderListModel)

QT_END_NAMESPACE

