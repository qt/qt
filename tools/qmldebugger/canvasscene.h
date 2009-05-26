#ifndef CANVASSCENE_H
#define CANVASSCENE_H

#include <QWidget>
#include <QTreeWidget>
#include <QtDeclarative/qsimplecanvas.h>
#include <QtDeclarative/qsimplecanvasitem.h>

class QmlDebugClient;
class CanvasSceneClient;
class QmlDebugClientPlugin;
class CanvasScene : public QWidget
{
Q_OBJECT
public:
    CanvasScene(QmlDebugClient *, QWidget *parent = 0);

    void message(QDataStream &);
private slots:
    void refresh();
    void itemClicked(QTreeWidgetItem *);
    void itemExpanded(QTreeWidgetItem *);
    void itemCollapsed(QTreeWidgetItem *);
    void setX(int);
    void setY(int);

private:
    void setOpacityRecur(QTreeWidgetItem *, qreal);
    void clone(QTreeWidgetItem *item, QSimpleCanvasItem *me, QDataStream &);
    QmlDebugClientPlugin *client;

    QTreeWidget *m_tree;
    QSimpleCanvas *m_canvas;
    QSimpleCanvasItem *m_canvasRoot;
    QTreeWidgetItem *m_selected;
};

#endif // CANVASSCENE_H

