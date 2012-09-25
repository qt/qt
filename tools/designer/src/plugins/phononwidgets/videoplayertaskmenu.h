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

#ifndef VIDEOPLAYERTASKMENU_H
#define VIDEOPLAYERTASKMENU_H


#include <QtCore/QObject>
#include <QtDesigner/QDesignerTaskMenuExtension>
#include <QtDesigner/private/extensionfactory_p.h>

#include <phonon/backendcapabilities.h>
#include <phonon/videoplayer.h>

QT_BEGIN_NAMESPACE

class VideoPlayerTaskMenu: public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    explicit VideoPlayerTaskMenu(Phonon::VideoPlayer *object, QObject *parent = 0);
    virtual QList<QAction*> taskActions() const;

private slots:
    void slotLoad();
    void slotMimeTypes();
    void mediaObjectStateChanged(Phonon::State newstate, Phonon::State oldstate);

private:
    Phonon::VideoPlayer *m_widget;
    QAction *m_displayMimeTypesAction;
    QAction *m_loadAction;
    QAction *m_playAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;

    QList<QAction*> m_taskActions;
};

typedef qdesigner_internal::ExtensionFactory<QDesignerTaskMenuExtension, Phonon::VideoPlayer, VideoPlayerTaskMenu>  VideoPlayerTaskMenuFactory;

QT_END_NAMESPACE

#endif // VIDEOPLAYERTASKMENU_H
