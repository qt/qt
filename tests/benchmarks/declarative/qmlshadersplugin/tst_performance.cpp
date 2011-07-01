/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QML Shaders plugin of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/qtest.h>
#include <QtDeclarative>
#include "../../../../src/imports/shaders/shadereffectitem.h"
#include "../../../../src/imports/shaders/shadereffectsource.h"
//#include "../../../src/shadereffect.h"

class BenchmarkItem : public QDeclarativeItem
{
    Q_OBJECT

public:
    BenchmarkItem( QDeclarativeItem * parent = 0 ) : QDeclarativeItem(parent)
        , m_frameCount(0)
    {
        setFlag(QGraphicsItem::ItemHasNoContents, false);
    }

    void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
        QDeclarativeItem::paint(painter, option, widget);
        if (timer.restart() > 7) m_frameCount++;
    }

    int frameCount() { return m_frameCount; }

private:
    int m_frameCount;
    QTime timer;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDeclarativeView view;
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view.setAttribute(Qt::WA_OpaquePaintEvent);
    view.setAttribute(Qt::WA_NoSystemBackground);
    view.setResizeMode(QDeclarativeView::SizeViewToRootObject);

    qmlRegisterType<ShaderEffectItem>("Qt.labs.shaders", 1, 0, "ShaderEffectItem");
    qmlRegisterType<ShaderEffectSource>("Qt.labs.shaders", 1, 0, "ShaderEffectSource");

    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);
    format.setSwapInterval(1);

    QGLWidget* glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    view.setViewport(glWidget);
    view.show();

    view.setSource(QUrl::fromLocalFile("TestWater.qml"));
    BenchmarkItem *benchmarkItem;

    qDebug() << "Sea Water benchmark:";
    benchmarkItem = new BenchmarkItem(dynamic_cast<QDeclarativeItem *>(view.rootObject()));
    QTest::qWait(5000);
    qDebug() << "Rendered " << benchmarkItem->frameCount() << " frames in 5 seconds";
    qDebug() << "Average " << benchmarkItem->frameCount() / 5.0 << " frames per second";

    qDebug() << "Gaussian drop shadow benchmark:";
    view.setSource(QUrl::fromLocalFile("TestGaussianDropShadow.qml"));
    benchmarkItem = new BenchmarkItem(dynamic_cast<QDeclarativeItem *>(view.rootObject()));
    QTest::qWait(5000);
    qDebug() << "Rendered " << benchmarkItem->frameCount() << " frames in 5 seconds";
    qDebug() << "Average " << benchmarkItem->frameCount() / 5.0 << " frames per second";
}

#include "tst_performance.moc"
