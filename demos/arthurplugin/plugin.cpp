/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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

#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerCustomWidgetInterface>

#include <QtCore/qplugin.h>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

#include "xform.h"
#include "pathdeform.h"
#include "gradients.h"
#include "pathstroke.h"
#include "hoverpoints.h"
#include "composition.h"

QT_FORWARD_DECLARE_CLASS(QDesignerFormEditorInterface)

static inline QString customWidgetDomXml(const QString &className)
{
    QString rc = QLatin1String("<ui language=\"c++\"><widget class=\"");
    rc += className;
    rc += QLatin1String("\" name=\"");
    QString objectName = className;
    objectName[0] = objectName.at(0).toLower();
    rc += objectName;
    rc += QLatin1String("\"/></ui>");
    return rc;
}

class PathDeformRendererEx : public PathDeformRenderer
{
    Q_OBJECT
public:
    PathDeformRendererEx(QWidget *parent) : PathDeformRenderer(parent) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class DemoPlugin : public QDesignerCustomWidgetInterface
{
    Q_INTERFACES(QDesignerCustomWidgetInterface)

protected:
    DemoPlugin(const QString &className);

public:
    QString name() const { return m_className; }
    bool isContainer() const { return false; }
    bool isInitialized() const { return m_initialized; }
    QIcon icon() const { return QIcon(); }
    QString codeTemplate() const { return QString(); }
    QString whatsThis() const { return QString(); }
    QString toolTip() const { return QString(); }
    QString group() const { return "Arthur Widgets [Demo]"; }
    void initialize(QDesignerFormEditorInterface *)
    {
        if (m_initialized)
            return;
        m_initialized = true;
    }
    QString domXml() const { return m_domXml; }

private:
    const QString m_className;
    const QString m_domXml;
    bool m_initialized;
};

DemoPlugin::DemoPlugin(const QString &className) :
    m_className(className),
    m_domXml(customWidgetDomXml(className)),
    m_initialized(false)
{
}

class DeformPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT

public:
    DeformPlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("PathDeformRendererEx")) { }
    QString includeFile() const { return "deform.h"; }

    QWidget *createWidget(QWidget *parent)
    {
        PathDeformRenderer *deform = new PathDeformRendererEx(parent);
        deform->setRadius(70);
        deform->setAnimated(false);
        deform->setFontSize(20);
        deform->setText("Arthur Widgets Demo");

        return deform;
    }
};

class XFormRendererEx : public XFormView
{
    Q_OBJECT
public:
    XFormRendererEx(QWidget *parent) : XFormView(parent) {}
    QSize sizeHint() const { return QSize(300, 200); }
};

class XFormPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    XFormPlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("XFormRendererEx")) { }
    QString includeFile() const { return "xform.h"; }

    QWidget *createWidget(QWidget *parent)
    {
        XFormRendererEx *xform = new XFormRendererEx(parent);
        xform->setText("Qt - Hello World!!");
        xform->setPixmap(QPixmap(":/trolltech/arthurplugin/bg1.jpg"));
        return xform;
    }
};


class GradientEditorPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    GradientEditorPlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("GradientEditor")) { }
    QString includeFile() const { return "gradients.h"; }

    QWidget *createWidget(QWidget *parent)
    {
        GradientEditor *editor = new GradientEditor(parent);
        return editor;
    }
};

class GradientRendererEx : public GradientRenderer
{
    Q_OBJECT
public:
    GradientRendererEx(QWidget *p) : GradientRenderer(p) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class GradientRendererPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    GradientRendererPlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("GradientRendererEx")) { }
    QString includeFile() const { return "gradients.h"; }

    QWidget *createWidget(QWidget *parent)
    {
        GradientRenderer *renderer = new GradientRendererEx(parent);
        renderer->setConicalGradient();
        return renderer;
    }
};

class PathStrokeRendererEx : public PathStrokeRenderer
{
    Q_OBJECT
public:
    PathStrokeRendererEx(QWidget *p) : PathStrokeRenderer(p) { }
    QSize sizeHint() const { return QSize(300, 200); }
};

class StrokeRenderPlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    StrokeRenderPlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("PathStrokeRendererEx")) { }
    QString includeFile() const { return "pathstroke.h"; }

    QWidget *createWidget(QWidget *parent)
    {
        PathStrokeRenderer *stroke = new PathStrokeRendererEx(parent);
        return stroke;
    }
};


class CompositionModePlugin : public QObject, public DemoPlugin
{
    Q_OBJECT
public:
    CompositionModePlugin(QObject *parent = 0) : QObject(parent), DemoPlugin(QLatin1String("CompositionRenderer")) { }
    QString includeFile() const { return "composition.h"; }

    QWidget *createWidget(QWidget *parent)
    {
        CompositionRenderer *renderer = new CompositionRenderer(parent);
        renderer->setAnimationEnabled(false);
        return renderer;
    }
};


class ArthurPlugins : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    ArthurPlugins(QObject *parent = 0);
    QList<QDesignerCustomWidgetInterface*> customWidgets() const { return m_plugins; }

private:
    QList<QDesignerCustomWidgetInterface *> m_plugins;
};

ArthurPlugins::ArthurPlugins(QObject *parent) :
    QObject(parent)
{
    m_plugins << new DeformPlugin(this)
              << new XFormPlugin(this)
              << new GradientEditorPlugin(this)
              << new GradientRendererPlugin(this)
              << new StrokeRenderPlugin(this)
              << new CompositionModePlugin(this);
}

#include "plugin.moc"

Q_EXPORT_PLUGIN2(ArthurPlugins, ArthurPlugins)
