/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "bearercloud.h"
#include "cloud.h"

#include <QGraphicsTextItem>
#include <QTimer>
#include <QDateTime>
#include <QHostInfo>

#include <QDebug>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//! [0]
BearerCloud::BearerCloud(QObject *parent)
:   QGraphicsScene(parent), timerId(0)
{
    setSceneRect(-300, -300, 600, 600);

    qsrand(QDateTime::currentDateTime().toTime_t());

    offset[QNetworkConfiguration::Active] = 2 * M_PI * qrand() / RAND_MAX;
    offset[QNetworkConfiguration::Discovered] = offset[QNetworkConfiguration::Active] + M_PI / 6;
    offset[QNetworkConfiguration::Defined] = offset[QNetworkConfiguration::Discovered] - M_PI / 6;
    offset[QNetworkConfiguration::Undefined] = offset[QNetworkConfiguration::Undefined] + M_PI / 6;

    thisDevice = new QGraphicsTextItem(QHostInfo::localHostName());
    thisDevice->setData(0, QLatin1String("This Device"));
    thisDevice->setPos(thisDevice->boundingRect().width() / -2,
                       thisDevice->boundingRect().height() / -2);
    addItem(thisDevice);

    qreal radius = Cloud::getRadiusForState(QNetworkConfiguration::Active);
    QGraphicsEllipseItem *orbit = new QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius);
    orbit->setPen(QColor(Qt::green));
    addItem(orbit);
    radius = Cloud::getRadiusForState(QNetworkConfiguration::Discovered);
    orbit = new QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius);
    orbit->setPen(QColor(Qt::blue));
    addItem(orbit);
    radius = Cloud::getRadiusForState(QNetworkConfiguration::Defined);
    orbit = new QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius);
    orbit->setPen(QColor(Qt::darkGray));
    addItem(orbit);
    radius = Cloud::getRadiusForState(QNetworkConfiguration::Undefined);
    orbit = new QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius);
    orbit->setPen(QColor(Qt::lightGray));
    addItem(orbit);

    connect(&manager, SIGNAL(configurationAdded(QNetworkConfiguration)),
            this, SLOT(configurationAdded(QNetworkConfiguration)));
    connect(&manager, SIGNAL(configurationRemoved(QNetworkConfiguration)),
            this, SLOT(configurationRemoved(QNetworkConfiguration)));
    connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SLOT(configurationChanged(QNetworkConfiguration)));

    QTimer::singleShot(0, this, SLOT(updateConfigurations()));
}
//! [0]

BearerCloud::~BearerCloud()
{
}

void BearerCloud::cloudMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}

void BearerCloud::timerEvent(QTimerEvent *)
{
    QList<Cloud *> clouds;
    foreach (QGraphicsItem *item, items()) {
        if (Cloud *cloud = qgraphicsitem_cast<Cloud *>(item))
            clouds << cloud;
    }

    foreach (Cloud *cloud, clouds)
        cloud->calculateForces();

    bool cloudsMoved = false;
    foreach (Cloud *cloud, clouds)
        cloudsMoved |= cloud->advance();

    if (!cloudsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}

//! [2]
void BearerCloud::configurationAdded(const QNetworkConfiguration &config)
{
    const QNetworkConfiguration::StateFlags state = config.state();

    configStates.insert(state, config.identifier());

    const qreal radius = Cloud::getRadiusForState(state);
    const int count = configStates.count(state);
    const qreal angle = 2 * M_PI / count;

    Cloud *item = new Cloud(config);
    configurations.insert(config.identifier(), item);

    item->setPos(radius * cos((count-1) * angle + offset[state]),
                 radius * sin((count-1) * angle + offset[state]));

    addItem(item);

    cloudMoved();
}
//! [2]

//! [3]
void BearerCloud::configurationRemoved(const QNetworkConfiguration &config)
{
    foreach (const QNetworkConfiguration::StateFlags &state, configStates.uniqueKeys())
        configStates.remove(state, config.identifier());

    Cloud *item = configurations.take(config.identifier());

    item->setFinalScale(0.0);
    item->setDeleteAfterAnimation(true);

    cloudMoved();
}
//! [3]

//! [4]
void BearerCloud::configurationChanged(const QNetworkConfiguration &config)
{
    foreach (const QNetworkConfiguration::StateFlags &state, configStates.uniqueKeys())
        configStates.remove(state, config.identifier());

    configStates.insert(config.state(), config.identifier());

    cloudMoved();
}
//! [4]

//! [1]
void BearerCloud::updateConfigurations()
{
    QList<QNetworkConfiguration> allConfigurations = manager.allConfigurations();

    while (!allConfigurations.isEmpty())
        configurationAdded(allConfigurations.takeFirst());

    cloudMoved();
}
//! [1]

