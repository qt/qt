#include "canvasscene.h"
#include <QtDeclarative/qmldebugclient.h>
#include <QDataStream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QtDeclarative/qfxrect.h>
#include <QtDeclarative/qfximage.h>

class CanvasSceneClientPlugin : public QmlDebugClientPlugin
{
public:
   CanvasSceneClientPlugin(QmlDebugClient *, CanvasScene *s);

protected:
   void messageReceived(const QByteArray &);

private:
   void dump(QDataStream &, int indent);
   CanvasScene *scene;
};

class QmlCanvasDebuggerItem : public QTreeWidgetItem
{
public:
    QmlCanvasDebuggerItem(QTreeWidget *tree)
        : QTreeWidgetItem(tree), me(0), img(0)
    {
    }

    QmlCanvasDebuggerItem(QTreeWidgetItem *item)
        : QTreeWidgetItem(item), me(0), img(0)
    {
    }

    QFxRect *me;
    QFxImage *img;
};

CanvasSceneClientPlugin::CanvasSceneClientPlugin(QmlDebugClient *c, 
                                                 CanvasScene *s)
: QmlDebugClientPlugin(QLatin1String("CanvasScene"), c), scene(s)
{
}

void CanvasSceneClientPlugin::messageReceived(const QByteArray &data)
{
    QByteArray d = data;
    QDataStream ds(&d, QIODevice::ReadOnly);

    scene->message(ds);
}

void CanvasScene::message(QDataStream &ds)
{
    QList<QSimpleCanvasItem *> children = m_canvasRoot->children();
    qDeleteAll(children);
    m_tree->clear();
    m_selected = 0;

    QTreeWidgetItem *root = new QmlCanvasDebuggerItem(m_tree);
    root->setText(0, tr("Root"));
    root->setExpanded(true);
    clone(root, m_canvasRoot, ds);
}

void CanvasScene::clone(QTreeWidgetItem *item, QSimpleCanvasItem *me, 
                        QDataStream &ds)
{
    int children;
    ds >> children;
    
    for (int ii = 0; ii < children; ++ii) {
        QString name;
        qreal x, y, z, width, height, scale;
        QTransform transform;
        bool activeFocus;
        int transformOrigin, flip, options;
        QPixmap pix;

        ds >> name >> x >> y >> z >> width >> height >> transformOrigin >> scale
           >> flip >> transform >> activeFocus >> options >> pix;

        QmlCanvasDebuggerItem *childItem = new QmlCanvasDebuggerItem(item);
        childItem->setText(0, name);
        childItem->setExpanded(true);

        QFxRect *rect = new QFxRect;
        rect->setParent(me);
        rect->setX(x);
        rect->setY(y);
        rect->setZ(z);
        rect->setWidth(width);
        rect->setHeight(height);
        rect->setTransformOrigin((QSimpleCanvasItem::TransformOrigin)transformOrigin);
        rect->setScale(scale);
        rect->setFlip((QSimpleCanvasItem::Flip)flip);
        rect->setTransform(transform);

        if (activeFocus)
            rect->setColor(QColor(0, 0, 0, 10));
        else if(options & QSimpleCanvasItem::IsFocusPanel)
            rect->setColor(QColor(0, 255, 0, 10));
        else if(options & QSimpleCanvasItem::IsFocusRealm)
            rect->setColor(QColor(0, 0, 255, 10));
        else
            rect->setColor(QColor(255, 0, 0, 10));

        if (pix.width() > 0 || pix.height() > 0) {
            QFxImage *img = new QFxImage;
            img->setParent(rect);
            img->setWidth(width);
            img->setHeight(height);
            img->setPixmap(pix);
            img->setOpacity(0);
            childItem->img = img;
        }

        childItem->me = rect;

        clone(childItem, rect, ds);
    }
}

void CanvasSceneClientPlugin::dump(QDataStream &ds, int indent)
{
    QString name;
    qreal x, y, z, width, height, scale;
    QTransform transform;
    bool activeFocus;
    int transformOrigin, flip, options, count;
    QPixmap pix;

    ds >> name >> x >> y >> z >> width >> height >> transformOrigin >> scale 
       >> flip >> transform >> activeFocus >> options >> pix >> count;

    QByteArray ba(indent * 4, ' ');
    qWarning() << ba.constData() << name << x << y;

    for(int ii = 0; ii < count; ++ii)
        dump(ds, indent + 1);
}

CanvasScene::CanvasScene(QmlDebugClient *c, QWidget *parent)
: QWidget(parent)
{
    client = new CanvasSceneClientPlugin(c, this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    setLayout(layout);
    QSplitter *splitter = new QSplitter(this);

    m_tree = new QTreeWidget(this);
    m_tree->setHeaderHidden(true);
    QObject::connect(m_tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), 
                     this, SLOT(itemExpanded(QTreeWidgetItem*)));
    QObject::connect(m_tree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), 
                     this, SLOT(itemCollapsed(QTreeWidgetItem*)));
    QObject::connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), 
                     this, SLOT(itemClicked(QTreeWidgetItem*)));
    m_canvas = new QSimpleCanvas(QSimpleCanvas::SimpleCanvas, this);
    m_canvasRoot = new QSimpleCanvasItem;
    m_canvasRoot->setParent(m_canvas->root());
    splitter->addWidget(m_tree);
    splitter->addWidget(m_canvas);
    splitter->setStretchFactor(1, 2);
    layout->addWidget(splitter);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->addStretch(2);
    hlayout->setSpacing(0);
    layout->addLayout(hlayout);
    QSpinBox *x = new QSpinBox(this);
    x->setSingleStep(50);
    x->setMaximum(10000);
    x->setMinimum(-10000);
    QObject::connect(x, SIGNAL(valueChanged(int)), this, SLOT(setX(int)));
    QSpinBox *y = new QSpinBox(this);
    y->setSingleStep(50);
    y->setMaximum(10000);
    y->setMinimum(-10000);
    QObject::connect(y, SIGNAL(valueChanged(int)), this, SLOT(setY(int)));
    hlayout->addWidget(x);
    hlayout->addWidget(y);
    QPushButton *pb = new QPushButton(tr("Refresh"), this);
    QObject::connect(pb, SIGNAL(clicked()), this, SLOT(refresh()));
    hlayout->addWidget(pb);
}

void CanvasScene::refresh()
{
    client->sendMessage(QByteArray());
}

void CanvasScene::itemExpanded(QTreeWidgetItem *i)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->me)
        item->me->setOpacity(1);
}

void CanvasScene::setOpacityRecur(QTreeWidgetItem *i, qreal op)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->img)
        item->img->setOpacity(op);

    for(int ii = 0; ii < item->childCount(); ++ii)
        setOpacityRecur(item->child(ii), op);
}

void CanvasScene::itemClicked(QTreeWidgetItem *i)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);

    if(m_selected) {
        setOpacityRecur(m_selected, 0);
        m_selected = 0;
    }
        
    m_selected = item;
    setOpacityRecur(m_selected, 1);
}

void CanvasScene::itemCollapsed(QTreeWidgetItem *i)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->me)
        item->me->setOpacity(0);
}

void CanvasScene::setX(int x)
{
    m_canvasRoot->setX(x);
}

void CanvasScene::setY(int y)
{
    m_canvasRoot->setY(y);
}

