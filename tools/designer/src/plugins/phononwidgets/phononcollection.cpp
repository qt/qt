/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "videoplayerplugin.h"
#include "seeksliderplugin.h"
#include "volumesliderplugin.h"

#include <QtDesigner/QDesignerCustomWidgetCollectionInterface>
#include <QtCore/qplugin.h>

QT_BEGIN_NAMESPACE

class PhononCollection: public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
public:
    explicit PhononCollection(QObject *parent = 0);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> m_plugins;
};

PhononCollection::PhononCollection(QObject *parent) :
    QObject(parent)
{
    const QString group = QLatin1String("Phonon");
    m_plugins.push_back(new VideoPlayerPlugin(group, this));
    m_plugins.push_back(new SeekSliderPlugin(group, this));
    m_plugins.push_back(new VolumeSliderPlugin(group, this));
}

QList<QDesignerCustomWidgetInterface*> PhononCollection::customWidgets() const
{
    return m_plugins;
}

Q_EXPORT_PLUGIN(PhononCollection)

QT_END_NAMESPACE

#include "phononcollection.moc"
