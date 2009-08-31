/*
    Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef DYNAMICTREEMODEL_H
#define DYNAMICTREEMODEL_H

#include <QAbstractItemModel>

#include <QHash>
#include <QList>

#include <QDebug>

#include <kdebug.h>

template<typename T> class QList;

class DynamicTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  DynamicTreeModel(QObject *parent = 0);

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &index = QModelIndex()) const;
  int columnCount(const QModelIndex &index = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  void clear();

protected slots:

  /**
  Finds the parent id of the string with id @p searchId.

  Returns -1 if not found.
  */
  qint64 findParentId(qint64 searchId) const;

private:
  QHash<qint64, QString> m_items;
  QHash<qint64, QList<QList<qint64> > > m_childItems;
  qint64 nextId;
  qint64 newId() { return nextId++; };

  QModelIndex m_nextParentIndex;
  int m_nextRow;

  int m_depth;
  int maxDepth;

  friend class ModelInsertCommand;
  friend class ModelMoveCommand;
  friend class ModelResetCommand;
  friend class ModelResetCommandFixed;

};


class ModelChangeCommand : public QObject
{
  Q_OBJECT
public:

  ModelChangeCommand( DynamicTreeModel *model, QObject *parent = 0 );

  virtual ~ModelChangeCommand() {}

  void setAncestorRowNumbers(QList<int> rowNumbers) { m_rowNumbers = rowNumbers; }

  QModelIndex findIndex(QList<int> rows);

  void setStartRow(int row) { m_startRow = row; }

  void setEndRow(int row) { m_endRow = row; }

  void setNumCols(int cols) { m_numCols = cols; }

  virtual void doCommand() = 0;

protected:
  DynamicTreeModel* m_model;
  QList<int> m_rowNumbers;
  int m_numCols;
  int m_startRow;
  int m_endRow;

};

typedef QList<ModelChangeCommand*> ModelChangeCommandList;

class ModelInsertCommand : public ModelChangeCommand
{
  Q_OBJECT

public:

  ModelInsertCommand(DynamicTreeModel *model, QObject *parent = 0 );
  virtual ~ModelInsertCommand() {}

  virtual void doCommand();
};


class ModelMoveCommand : public ModelChangeCommand
{
  Q_OBJECT
public:
  ModelMoveCommand(DynamicTreeModel *model, QObject *parent);

  virtual ~ModelMoveCommand() {}

  virtual bool emitPreSignal(const QModelIndex &srcParent, int srcStart, int srcEnd, const QModelIndex &destParent, int destRow);

  virtual void doCommand();

  virtual void emitPostSignal();

  void setDestAncestors( QList<int> rows ) { m_destRowNumbers = rows; }

  void setDestRow(int row) { m_destRow = row; }

protected:
  QList<int> m_destRowNumbers;
  int m_destRow;
};

/**
  A command which does a move and emits a reset signal.
*/
class ModelResetCommand : public ModelMoveCommand
{
  Q_OBJECT
public:
  ModelResetCommand(DynamicTreeModel* model, QObject* parent = 0);

  virtual ~ModelResetCommand();

  virtual bool emitPreSignal(const QModelIndex &srcParent, int srcStart, int srcEnd, const QModelIndex &destParent, int destRow);
  virtual void emitPostSignal();

};

/**
  A command which does a move and emits a beginResetModel and endResetModel signals.
*/
class ModelResetCommandFixed : public ModelMoveCommand
{
  Q_OBJECT
public:
  ModelResetCommandFixed(DynamicTreeModel* model, QObject* parent = 0);

  virtual ~ModelResetCommandFixed();

  virtual bool emitPreSignal(const QModelIndex &srcParent, int srcStart, int srcEnd, const QModelIndex &destParent, int destRow);
  virtual void emitPostSignal();

};


#endif
