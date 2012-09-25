/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QMLTOOLBAR_H
#define QMLTOOLBAR_H

#include <QtGui/QToolBar>
#include <QtGui/QIcon>

#include "../qmlinspectorconstants.h"

QT_FORWARD_DECLARE_CLASS(QActionGroup)

namespace QmlJSDebugger {

class ToolBarColorBox;

class QmlToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit QmlToolBar(QWidget *parent = 0);
    ~QmlToolBar();

public slots:
    void setDesignModeBehavior(bool inDesignMode);
    void setColorBoxColor(const QColor &color);
    void activateColorPicker();
    void activateSelectTool();
    void activateMarqueeSelectTool();
    void activateZoom();

    void setAnimationSpeed(qreal slowDownFactor);
    void setAnimationPaused(bool paused);

signals:
    void animationSpeedChanged(qreal factor);
    void animationPausedChanged(bool paused);

    void designModeBehaviorChanged(bool inDesignMode);
    void colorPickerSelected();
    void selectToolSelected();
    void marqueeSelectToolSelected();
    void zoomToolSelected();

    void applyChangesToQmlFileSelected();
    void applyChangesFromQmlFileSelected();

private slots:
    void setDesignModeBehaviorOnClick(bool inDesignMode);
    void activatePlayOnClick();
    void activateColorPickerOnClick();
    void activateSelectToolOnClick();
    void activateMarqueeSelectToolOnClick();
    void activateZoomOnClick();

    void activateFromQml();
    void activateToQml();

    void changeAnimationSpeed();

    void updatePlayAction();

private:
    class Ui {
    public:
        QAction *designmode;
        QAction *play;
        QAction *select;
        QAction *selectMarquee;
        QAction *zoom;
        QAction *colorPicker;
        QAction *toQml;
        QAction *fromQml;
        QIcon playIcon;
        QIcon pauseIcon;
        ToolBarColorBox *colorBox;

        QActionGroup *playSpeedMenuActions;
    };

    bool m_emitSignals;
    bool m_paused;
    qreal m_animationSpeed;

    Constants::DesignTool m_activeTool;

    Ui *ui;
};

}

#endif // QMLTOOLBAR_H
