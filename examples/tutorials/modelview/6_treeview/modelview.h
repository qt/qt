#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QtGui/QMainWindow>

class QTreeView; //forward declaration
class QStandardItemModel;
class QStandardItem;


class ModelView : public QMainWindow
{
    Q_OBJECT
private:
    QTreeView *treeView;
    QStandardItemModel *standardModel;
    QList<QStandardItem *> prepareColumn(const QString &first,
                                         const QString &second,
                                         const QString &third );
public:
    ModelView(QWidget *parent = 0);
};

#endif // MODELVIEW_H
