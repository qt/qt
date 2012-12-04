/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QtGui>

#include "multipagewidget.h"

MultiPageWidget::MultiPageWidget(QWidget *parent)
    : QWidget(parent)
{
    comboBox = new QComboBox();
    comboBox->setObjectName("__qt__passive_comboBox");
    stackWidget = new QStackedWidget();

    connect(comboBox, SIGNAL(activated(int)),
            this, SLOT(setCurrentIndex(int)));

    layout = new QVBoxLayout();
    layout->addWidget(comboBox);
    layout->addWidget(stackWidget);
    setLayout(layout);
}

QSize MultiPageWidget::sizeHint() const
{
    return QSize(200, 150);
}

void MultiPageWidget::addPage(QWidget *page)
{
    insertPage(count(), page);
}

void MultiPageWidget::removePage(int index)
{
    QWidget *widget = stackWidget->widget(index);
    stackWidget->removeWidget(widget);

    comboBox->removeItem(index);
}

int MultiPageWidget::count() const
{
    return stackWidget->count();
}

int MultiPageWidget::currentIndex() const
{
    return stackWidget->currentIndex();
}

void MultiPageWidget::insertPage(int index, QWidget *page)
{
    page->setParent(stackWidget);

    stackWidget->insertWidget(index, page);

    QString title = page->windowTitle();
    if (title.isEmpty()) {
        title = tr("Page %1").arg(comboBox->count() + 1);
        page->setWindowTitle(title);
    }
    comboBox->insertItem(index, title);
}

void MultiPageWidget::setCurrentIndex(int index)
{
    if (index != currentIndex()) {
        stackWidget->setCurrentIndex(index);
        comboBox->setCurrentIndex(index);
        emit currentIndexChanged(index);
    }
}

QWidget* MultiPageWidget::widget(int index)
{
    return stackWidget->widget(index);
}

QString MultiPageWidget::pageTitle() const
{
    if (const QWidget *currentWidget = stackWidget->currentWidget())
        return currentWidget->windowTitle();
    return QString();
}

void MultiPageWidget::setPageTitle(QString const &newTitle)
{
    comboBox->setItemText(currentIndex(), newTitle);
    if (QWidget *currentWidget = stackWidget->currentWidget())
        currentWidget->setWindowTitle(newTitle);
    emit pageTitleChanged(newTitle);
}
