#include "window.h"
#include "layoutitem.h"
#include "scene.h"
#include "widgetchooserdelegate.h"

#include <QtGui/qgraphicswidget.h>
#include <QtGui/qgraphicsanchorlayout.h>

static QGraphicsLayoutItem *layoutItem(const QVariant &data)
{
    return reinterpret_cast<QGraphicsLayoutItem *>(qVariantValue<void *>(data));
}

static QString nodeName(QGraphicsLayoutItem *item)
{
    QString str = item->isLayout() ? QLatin1String("layout") : item->graphicsItem()->data(0).toString();
    str.replace(QLatin1Char(' '), QLatin1Char('_'));
    return str;
}

#define _QUOTEMACRO(x) #x
#define QUOTEMACRO(x) _QUOTEMACRO(x)

Window::Window(QWidget *parent)
    : QMainWindow(parent), m_inAddAnchor(false)
{
    m_ui.setupUi(this);
    m_scene = new Scene;

    m_ui.graphicsView->setScene(m_scene);
    connect(m_scene, SIGNAL(focusItemChanged(QGraphicsItem*)), this, SLOT(scene_focusItemChanged(QGraphicsItem*)));

    m_window = new QGraphicsWidget(0, Qt::Window);
    m_scene->addItem(m_window);
    m_layout = new QGraphicsAnchorLayout(m_window);
    m_window->setLayout(m_layout);
    m_window->resize(400, 300);

    findLayoutFiles();

    WidgetChooserDelegate *delegate = new WidgetChooserDelegate(&m_layoutItems, m_layout, m_ui.anchors);
    m_ui.anchors->setItemDelegate(delegate);

}

void Window::findLayoutFiles()
{
    m_ignoreCurrentLayoutChange = true;
    QLatin1String s(QUOTEMACRO(PRO_FILE_PWD)"/xml");
    QDirIterator it(s);
    while (it.hasNext()) {
        it.next();
        QFileInfo fi = it.fileInfo();
        QString baseName = fi.baseName();
        if (!baseName.isEmpty()) {  // avoid "." and ".."
            m_ui.layouts->addItem(baseName);
        }
    }
    m_ignoreCurrentLayoutChange = false;
}

void Window::on_layouts_currentRowChanged(int row)
{
    if (m_ignoreCurrentLayoutChange)
        return;

    QListWidgetItem *item = m_ui.layouts->item(row);
    if (item) {
        QString fileName = QString::fromAscii("%1/xml/%2.xml").arg(QUOTEMACRO(PRO_FILE_PWD), item->text());
        if (!loadLayout(fileName, m_layout)) {
            qWarning("could not find %s", qPrintable(fileName));
        }
    }
}

void Window::on_anchors_cellChanged(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!m_inAddAnchor)
        rebuildLayout();
}


void Window::on_pbAddAnchor_clicked(bool)
{
    addAnchorRow();
}

void Window::on_actionAdd_item_triggered(bool )
{
    addItem();
}

void Window::on_actionSave_layout_triggered(bool )
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save layout"), QLatin1String(QUOTEMACRO(PRO_FILE_PWD)"/xml"), QLatin1String("*.xml"));
    if (!fileName.isEmpty())
        saveLayout(fileName);
}

void Window::on_itemName_textEdited(const QString & )
{
    updateItem();
}

void Window::on_itemMinW_valueChanged(double )
{
    updateItem();
}

void Window::on_itemMinH_valueChanged(double )
{
    updateItem();
}

void Window::on_itemPrefW_valueChanged(double )
{
    updateItem();
}

void Window::on_itemPrefH_valueChanged(double )
{
    updateItem();
}

void Window::on_itemMaxW_valueChanged(double )
{
    updateItem();
}

void Window::on_itemMaxH_valueChanged(double )
{
    updateItem();
}

void Window::scene_focusItemChanged(QGraphicsItem *item)
{
    bool isItemValid = item && item->isWidget();
    m_ui.groupBox->setEnabled(isItemValid);
    QGraphicsLayoutItem *newCurrentItem = m_currentItem;
    m_currentItem = 0;
    if (isItemValid) {
        QGraphicsWidget *w = static_cast<QGraphicsWidget *>(item);
        setItemData(item->data(0).toString(), w->minimumSize(), w->preferredSize(), w->maximumSize());
        newCurrentItem = w;
    } else {
        setItemData(QString(), QSizeF(), QSizeF(), QSizeF());
    }
    m_currentItem = newCurrentItem;
}

void Window::updateItem()
{
    if (!m_currentItem)
        return;
    if (QGraphicsItem *gi = m_currentItem->graphicsItem()) {
        gi->setData(0, m_ui.itemName->text());
        gi->update();
    }
    QSizeF min(m_ui.itemMinW->value(), m_ui.itemMinH->value());
    QSizeF pref(m_ui.itemPrefW->value(), m_ui.itemPrefH->value());
    QSizeF max(m_ui.itemMaxW->value(), m_ui.itemMaxH->value());

    if (min.isValid())
        m_currentItem->setMinimumSize(min);
    if (pref.isValid())
        m_currentItem->setPreferredSize(pref);
    if (max.isValid())
        m_currentItem->setMaximumSize(max);
}

void Window::rebuildLayout()
{
    int i;
    for (i = m_layout->count(); i > 0; --i) {
        m_layout->removeAt(0);
    }

    int rc = m_ui.anchors->rowCount();
    for (i = 0; i < rc; ++i) {
        bool ok;

        QGraphicsLayoutItem *startItem = layoutItemAt(m_ui.anchors->model(), i, 0);
        if (!startItem)
            continue;
        QGraphicsAnchorLayout::Edge startEdge = (QGraphicsAnchorLayout::Edge)(m_ui.anchors->item(i, 1)->data(Qt::UserRole).toInt(&ok));
        if (!ok)
            continue;
        QGraphicsLayoutItem *endItem = layoutItemAt(m_ui.anchors->model(), i, 2);
        if (!endItem)
            continue;
        QGraphicsAnchorLayout::Edge endEdge = (QGraphicsAnchorLayout::Edge)(m_ui.anchors->item(i, 3)->data(Qt::UserRole).toInt(&ok));
        if (!ok)
            continue;

        m_layout->anchor(startItem, startEdge, endItem, endEdge);
    }
}

void Window::setItemData(const QString &name, const QSizeF &min, const QSizeF &pref, const QSizeF &max)
{
    m_ui.itemName->setText(name);
    m_ui.itemMinW->setValue(min.width());
    m_ui.itemMinH->setValue(min.height());
    m_ui.itemPrefW->setValue(pref.width());
    m_ui.itemPrefH->setValue(pref.height());
    m_ui.itemMaxW->setValue(max.width());
    m_ui.itemMaxH->setValue(max.height());
}

QGraphicsLayoutItem *Window::addItem(const QString &name)
{
    int rc = m_layoutItems.count();
    QString effectiveName = name.isEmpty() ? QString::fromAscii("item %1").arg(rc + 1) : name;
    QGraphicsLayoutItem *layoutItem = new LayoutItem(effectiveName, m_window);
    m_layoutItems.append(layoutItem);
    m_scene->setFocusItem(layoutItem->graphicsItem());
    return layoutItem;
}

static const char *strEdges[] = {"Left",
                                 "HCenter",
                                 "Right",
                                 "Top",
                                 "VCenter",
                                 "Bottom"};

void Window::setAnchorData(QGraphicsLayoutItem *startItem, const QString &startName, QGraphicsAnchorLayout::Edge startEdge,
                       QGraphicsLayoutItem *endItem, const QString &endName, QGraphicsAnchorLayout::Edge endEdge, int row /*= -1*/)
{
    if (row == -1) {
        row = m_ui.anchors->rowCount();
        m_ui.anchors->insertRow(row);
    }
    m_inAddAnchor = true;

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setData(Qt::UserRole, qVariantFromValue<void*>(reinterpret_cast<void*>(startItem)));
    item->setData(Qt::DisplayRole, startName);
    m_ui.anchors->setItem(row, 0, item);

    item = new QTableWidgetItem;
    item->setData(Qt::UserRole, int(startEdge));
    item->setData(Qt::DisplayRole, QLatin1String(strEdges[startEdge]));
    m_ui.anchors->setItem(row, 1, item);

    item = new QTableWidgetItem;
    item->setData(Qt::UserRole, qVariantFromValue<void*>(reinterpret_cast<void*>(endItem)));
    item->setData(Qt::DisplayRole, endName);
    m_ui.anchors->setItem(row, 2, item);

    item = new QTableWidgetItem;
    item->setData(Qt::UserRole, int(endEdge));
    item->setData(Qt::DisplayRole, QLatin1String(strEdges[endEdge]));
    m_ui.anchors->setItem(row, 3, item);

    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, 6);
    m_ui.anchors->setItem(row, 4, item);

    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, 6);
    m_ui.anchors->setItem(row, 5, item);
    m_inAddAnchor = false;

}

void Window::addAnchorRow()
{
    int rc = m_ui.anchors->rowCount();
    QGraphicsLayoutItem *defaultLayoutItem = m_layout;
    if (m_layoutItems.count() > 0) {
        defaultLayoutItem = m_layoutItems.at(0);
    }

    m_ui.anchors->insertRow(rc);

    if (defaultLayoutItem) {
        QString defaultName = defaultLayoutItem->isLayout() ?
                                QLatin1String("layout") :
                                defaultLayoutItem->graphicsItem()->data(0).toString();
        setAnchorData(defaultLayoutItem, defaultName, QGraphicsAnchorLayout::Right, defaultLayoutItem, defaultName, QGraphicsAnchorLayout::Left, rc);
        rebuildLayout();
    }
}

QGraphicsLayoutItem *Window::layoutItemAt(QAbstractItemModel *model, int row, int column /*= 0*/)
{
    int rc = model->rowCount();
    QGraphicsLayoutItem *item = 0;
    if (row < rc) {
        QVariant data = model->data(model->index(row, column), Qt::UserRole);
        if (data.isValid()) {
            item = layoutItem(data);
        }
    }
    return item;
}

bool Window::saveLayout(const QString& fileName)
{
    bool ok;
    QFile out(fileName);
    ok = out.open(QIODevice::WriteOnly);
    if (ok) {
        QXmlStreamWriter xml(&out);
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        xml.writeStartElement(QLatin1String("anchorlayout"));
        int i;
        for (i = 0; i < m_layoutItems.count(); ++i) {
            QGraphicsLayoutItem *item = m_layoutItems.at(i);
            xml.writeStartElement(QLatin1String("item"));
            QString name = nodeName(item);
            if (name == QLatin1String("layout"))
                name = QLatin1String("this");
            xml.writeAttribute(QLatin1String("id"), name);
            for (int p = 0; p < 3; ++p) {
                const char *propertyNames[] = {"minimumSize", "preferredSize", "maximumSize"};
                int b;
                typedef  QSizeF (QGraphicsLayoutItem::*QGLISizeGetter)(void) const;
                QGLISizeGetter sizeGetters[] = { &QGraphicsLayoutItem::minimumSize,
                                                                        &QGraphicsLayoutItem::preferredSize,
                                                                        &QGraphicsLayoutItem::maximumSize};
                QSizeF size = ((*item).*(sizeGetters[p])) ();
                xml.writeStartElement(QLatin1String("property"));
                xml.writeAttribute(QLatin1String("name"), QLatin1String(propertyNames[p]));
                xml.writeStartElement(QLatin1String("size"));
                xml.writeAttribute(QLatin1String("width"), QString::number(size.width()));
                xml.writeAttribute(QLatin1String("height"), QString::number(size.height()));
                xml.writeEndElement();
                xml.writeEndElement();
            }
            xml.writeEndElement();
        }

        QHash<int, QString> edgeString;
        for (i = 0; i < sizeof(strEdges)/sizeof(char*); ++i) {
            edgeString.insert(i, QLatin1String(strEdges[i]));
        }
        int rc = m_ui.anchors->rowCount();
        for (i = 0; i < rc; ++i) {
            xml.writeStartElement(QLatin1String("anchor"));
            bool ok;

            QGraphicsLayoutItem *startItem = layoutItemAt(m_ui.anchors->model(), i, 0);
            if (!startItem)
                continue;
            QGraphicsAnchorLayout::Edge startEdge = (QGraphicsAnchorLayout::Edge)(m_ui.anchors->item(i, 1)->data(Qt::UserRole).toInt(&ok));
            if (!ok)
                continue;
            QGraphicsLayoutItem *endItem = layoutItemAt(m_ui.anchors->model(), i, 2);
            if (!endItem)
                continue;
            QGraphicsAnchorLayout::Edge endEdge = (QGraphicsAnchorLayout::Edge)(m_ui.anchors->item(i, 3)->data(Qt::UserRole).toInt(&ok));
            if (!ok)
                continue;

            QString strStart = nodeName(startItem);
            if (strStart == QLatin1String("layout"))
                strStart = QLatin1String("this");
            xml.writeAttribute(QLatin1String("first"), QString::fromAscii("%1.%2").arg(strStart, edgeString.value(startEdge)));

            QString strEnd = nodeName(endItem);
            if (strEnd == QLatin1String("layout"))
                strEnd = QLatin1String("this");
            xml.writeAttribute(QLatin1String("second"), QString::fromAscii("%1.%2").arg(strEnd, edgeString.value(endEdge)));

            xml.writeEndElement();
        }

        xml.writeEndElement();
        xml.writeEndDocument();
        out.close();
    }
    return ok;
}

static bool parseProperty(QXmlStreamReader *xml, QString *name, QSizeF *size)
{
    QString propName = xml->attributes().value("name").toString();
    QSizeF sz;
    while (!xml->atEnd()) {
        xml->readNext();
        if (xml->isStartElement() && xml->name() == QLatin1String("size")) {
            QXmlStreamAttributes attrs = xml->attributes();
            QString sw = attrs.value("width").toString();
            QString sh = attrs.value("height").toString();
            bool ok;
            float w = sw.toFloat(&ok);
            if (ok) {
                sz.setWidth(w);
            }

            float h = sw.toFloat(&ok);
            if (ok) {
                sz.setHeight(h);
            }

        }
        if (xml->isEndElement() && xml->name() == QLatin1String("property")) {
            if (name && size && sz.isValid()) {
                *name = propName;
                *size = sz;
                return true;
            }
        }
    }
    return false;
}

static bool parseEdge(const QString &itemEdge, QByteArray *id, QGraphicsAnchorLayout::Edge *edge)
{
    QStringList item_edge = itemEdge.split(QLatin1Char('.'));
    bool ok = item_edge.count() == 2;
    if (ok) {
        QByteArray strEdge = item_edge.at(1).toAscii().toLower();
        if (strEdge == "left") {
            *edge = QGraphicsAnchorLayout::Left;
        } else if (strEdge == "hcenter") {
            *edge = QGraphicsAnchorLayout::HCenter;
        } else if (strEdge == "right") {
            *edge = QGraphicsAnchorLayout::Right;
        } else if (strEdge == "top") {
            *edge = QGraphicsAnchorLayout::Top;
        } else if (strEdge == "vcenter") {
            *edge = QGraphicsAnchorLayout::VCenter;
        } else if (strEdge == "bottom") {
            *edge = QGraphicsAnchorLayout::Bottom;
        } else {
            ok = false;
        }
        if (ok)
            *id = item_edge.at(0).toAscii();
    }
    return ok;
}

bool Window::loadLayout(const QString& fileName, QGraphicsAnchorLayout *layout)
{
    QFile input(fileName);
    bool ok = input.open(QIODevice::ReadOnly | QIODevice::Text);
    if (ok) {
        int i;
        for (i = 0; i < m_layoutItems.count(); ++i) {
            QGraphicsLayoutItem *item = m_layoutItems.at(i);
            delete item;
        }

        m_layoutItems.clear();
        m_ui.anchors->setRowCount(0);

        QLatin1String str_anchorlayout("anchorlayout");
        QLatin1String str_item("item");
        QLatin1String str_property("property");
        QXmlStreamReader xml(&input);

        int level = 0;
        QHash<QByteArray, QGraphicsLayoutItem*> hash;
        QString item_id;
        QSizeF min, pref, max;

        while (!xml.atEnd()) {
            QXmlStreamReader::TokenType token = xml.readNext();
            switch (token) {
            case QXmlStreamReader::StartDocument:
                break;
            case QXmlStreamReader::StartElement:
                if (level == 0 && xml.name() == str_anchorlayout) {

                } else if (level == 1 && xml.name() == str_item) {
                    item_id = xml.attributes().value("id").toString();
                } else if (level == 1 && xml.name() == QLatin1String("anchor")) {
                    QXmlStreamAttributes attrs = xml.attributes();
                    QString first = attrs.value("first").toString();
                    QString second = attrs.value("second").toString();
                    QByteArray startID;
                    QGraphicsAnchorLayout::Edge startEdge;
                    QGraphicsLayoutItem *startItem = 0;
                    if (parseEdge(first, &startID, &startEdge)) {
                        if (startID == "this") {
                            startID = "layout";
                            startItem = layout;
                        } else {
                            startItem = hash.value(startID);
                        }
                    } else {
                        qWarning("%s is not a valid edge description", qPrintable(first));
                        break;
                    }

                    QByteArray endID;
                    QGraphicsAnchorLayout::Edge endEdge;
                    QGraphicsLayoutItem *endItem = 0;
                    if (parseEdge(second, &endID, &endEdge)) {
                        if (endID == "this") {
                            endID = "layout";
                            endItem = layout;
                        } else {
                            endItem = hash.value(endID);
                        }
                    } else {
                        qWarning("%s is not a valid edge description", qPrintable(second));
                        break;
                    }
                    setAnchorData(  startItem, startID, startEdge,
                                    endItem, endID, endEdge, -1);
                } else if (level == 2 && xml.name() == str_property) {
                    QString name;
                    QSizeF size;
                    if (parseProperty(&xml, &name, &size)) {
                        if (name == QLatin1String("minimumSize")) {
                            min = size;
                        } else if (name == QLatin1String("preferredSize")) {
                            pref = size;
                        } else if (name == QLatin1String("maximumSize")) {
                            max = size;
                        }
                        break;
                    }
                } else {
                    --level;
                }
                ++level;
                break;
            case QXmlStreamReader::EndElement:
                if (xml.name() == str_anchorlayout) {
                }
                if (xml.name() == str_item) {
                    QGraphicsLayoutItem *item = addItem(item_id);
                    item->setMinimumSize(min);
                    item->setPreferredSize(pref);
                    item->setMaximumSize(max);
                    hash.insert(item_id.toAscii(), item);
                }
                --level;
                break;
            }
        }
        if (xml.hasError()) {
             // do error handling
            qWarning("%s", qPrintable(xml.errorString()));
        }
        input.close();
    }
    if (ok)
        rebuildLayout();
    return ok;
}
