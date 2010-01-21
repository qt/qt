/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#ifndef STYLEWIDGET_H
#define STYLEWIDGET_H

#include <QWidget>
#include <QPainterPath>
#include "renderarea.h"

class QLabel;

class StyleWidget : public QWidget
{
    Q_OBJECT

public:
    StyleWidget(QWidget *parent = 0);

private:
    RenderArea *solid;
    RenderArea *dense1;
    RenderArea *dense2;
    RenderArea *dense3;
    RenderArea *dense4;
    RenderArea *dense5;
    RenderArea *dense6;
    RenderArea *dense7;
    RenderArea *no;
    RenderArea *hor;
    RenderArea *ver;
    RenderArea *cross;
    RenderArea *bdiag;
    RenderArea *fdiag;
    RenderArea *diagCross;
    RenderArea *linear;
    RenderArea *radial;
    RenderArea *conical;
    RenderArea *texture;

    QLabel *solidLabel;
    QLabel *dense1Label;
    QLabel *dense2Label;
    QLabel *dense3Label;
    QLabel *dense4Label;
    QLabel *dense5Label;
    QLabel *dense6Label;
    QLabel *dense7Label;
    QLabel *noLabel;
    QLabel *horLabel;
    QLabel *verLabel;
    QLabel *crossLabel;
    QLabel *bdiagLabel;
    QLabel *fdiagLabel;
    QLabel *diagCrossLabel;
    QLabel *linearLabel;
    QLabel *radialLabel;
    QLabel *conicalLabel;
    QLabel *textureLabel;
};
#endif
