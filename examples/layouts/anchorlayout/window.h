#ifndef WINDOW_H
#define WINDOW_H

#include "ui_anchorlayout.h"
#include <QtGui/qgraphicsanchorlayout.h>

class QGraphicsWidget;
class Scene;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    Window(QWidget *parent = 0);

private slots:
    void on_anchors_cellChanged(int row, int column);
    void on_pbAddAnchor_clicked(bool);
    void on_actionAdd_item_triggered(bool checked);
    void on_layouts_currentRowChanged(int row);
    void on_itemName_textEdited(const QString &text);
    void on_itemMinW_valueChanged(double d);
    void on_itemMinH_valueChanged(double d);
    void on_itemPrefW_valueChanged(double d);
    void on_itemPrefH_valueChanged(double d);
    void on_itemMaxW_valueChanged(double d);
    void on_itemMaxH_valueChanged(double d);



    // not in ui file
    void scene_focusItemChanged(QGraphicsItem *item);
private:
    void updateItem();
    void setItemData(const QString &name, const QSizeF &min, const QSizeF &pref, const QSizeF &max);
    QGraphicsLayoutItem *addItem(const QString &name = QString());
    void addAnchorRow();
    void setAnchorData(QGraphicsLayoutItem *startItem, const QString &startName, QGraphicsAnchorLayout::Edge startEdge,
                       QGraphicsLayoutItem *endItem, const QString &endName, QGraphicsAnchorLayout::Edge endEdge, int row = -1);

    bool saveLayout(const QString& fileName);
    bool loadLayout(const QString& fileName, QGraphicsAnchorLayout *layout);
    void findLayoutFiles();

    void rebuildLayout();
    QGraphicsLayoutItem *layoutItemAt(QAbstractItemModel *mode, int row, int column = 0);

    Ui::MainWindow m_ui;
    Scene *m_scene;
    bool m_changingCell;
    QGraphicsWidget *m_window;
    QGraphicsAnchorLayout *m_layout;
    QVector<QGraphicsLayoutItem*> m_layoutItems;
    QGraphicsLayoutItem *m_currentItem;
    bool m_inAddAnchor;
    bool m_ignoreCurrentLayoutChange;
};

#endif // WINDOW_H

