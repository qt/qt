#ifndef MODELVIEW_H
#define MODELVIEW_H

#include <QtGui/QMainWindow>

class QTreeView; //forward declaration
class QStandardItemModel;
class QItemSelection;


class ModelView : public QMainWindow
{
    Q_OBJECT
private:
    QTreeView *treeView;
    QStandardItemModel *standardModel;
private slots:
    void selectionChangedSlot(const QItemSelection & newSelection, const QItemSelection & oldSelection);
public:
    ModelView(QWidget *parent = 0);
};

#endif // MODELVIEW_H
