/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "ambientproperties.h"

#include <QtGui>

QT_BEGIN_NAMESPACE

AmbientProperties::AmbientProperties(QWidget *parent)
: QDialog(parent), container(0)
{
    setupUi(this);

    connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));
}

void AmbientProperties::setControl(QWidget *widget)
{
    container = widget;

    QColor c = container->palette().color(container->backgroundRole());
    QPalette p = backSample->palette(); p.setColor(backSample->backgroundRole(), c); backSample->setPalette(p);

    c = container->palette().color(container->foregroundRole());
    p = foreSample->palette(); p.setColor(foreSample->backgroundRole(), c); foreSample->setPalette(p);

    fontSample->setFont( container->font() );
    buttonEnabled->setChecked( container->isEnabled() );
    enabledSample->setEnabled( container->isEnabled() );
}

void AmbientProperties::on_buttonBackground_clicked()
{
    QColor c = QColorDialog::getColor(backSample->palette().color(backSample->backgroundRole()), this);
    QPalette p = backSample->palette(); p.setColor(backSample->backgroundRole(), c); backSample->setPalette(p);
    p = container->palette(); p.setColor(container->backgroundRole(), c); container->setPalette(p);

    if (QWorkspace *ws = qobject_cast<QWorkspace*>(container)) {
	QWidgetList list( ws->windowList() );
	for (int i = 0; i < list.count(); ++i) {
	    QWidget *widget = list.at(i);
	    p = widget->palette(); p.setColor(widget->backgroundRole(), c); widget->setPalette(p);
	}
    }
}

void AmbientProperties::on_buttonForeground_clicked()
{
    QColor c = QColorDialog::getColor(foreSample->palette().color(foreSample->backgroundRole()), this);
    QPalette p = foreSample->palette(); p.setColor(foreSample->backgroundRole(), c); foreSample->setPalette(p);
    p = container->palette(); p.setColor(container->foregroundRole(), c); container->setPalette(p);

    if (QWorkspace *ws = qobject_cast<QWorkspace*>(container)) {
	QWidgetList list( ws->windowList() );
	for (int i = 0; i < list.count(); ++i) {
	    QWidget *widget = list.at(i);
	    p = widget->palette(); p.setColor(widget->foregroundRole(), c); widget->setPalette(p);
	}
    }
}

void AmbientProperties::on_buttonFont_clicked()
{
    bool ok;
    QFont f = QFontDialog::getFont( &ok, fontSample->font(), this );
    if ( !ok )
	return;
    fontSample->setFont( f );
    container->setFont( f );

    if (QWorkspace *ws = qobject_cast<QWorkspace*>(container)) {
	QWidgetList list( ws->windowList() );
	for (int i = 0; i < list.count(); ++i) {
	    QWidget *widget = list.at(i);
	    widget->setFont( f );
	}
    }
}

void AmbientProperties::on_buttonEnabled_toggled(bool on)
{
    enabledSample->setEnabled( on );
    container->setEnabled( on );
}

QT_END_NAMESPACE
