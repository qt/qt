#ifndef CANVASSCENE_H
#define CANVASSCENE_H

#include <QWidget>
#include <QTreeWidget>
#include <QtDeclarative/qsimplecanvas.h>
#include <QtDeclarative/qsimplecanvasitem.h>

QT_BEGIN_NAMESPACE

class QmlDebugConnection;
class CanvasSceneClient;
class QmlDebugClient;
class CanvasScene : public QWidget
{
Q_OBJECT
public:
    CanvasScene(QmlDebugConnection *, QWidget *parent = 0);

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
    QmlDebugClient *client;

    QTreeWidget *m_tree;
    QSimpleCanvas *m_canvas;
    QSimpleCanvasItem *m_canvasRoot;
    QTreeWidgetItem *m_selected;
};

QT_END_NAMESPACE

#endif // CANVASSCENE_H

