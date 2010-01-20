/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include "mainwindow.h"
#include <QSplitter>
#include <QHeaderView>

StyleWidget::StyleWidget(QWidget *parent, Qt::WFlags f)
    : QWidget(parent, f)
{
    QHBoxLayout *hbox = new QHBoxLayout(this);
    QSplitter *spl = new QSplitter(this);

    // standard widgets
    QWidget *leftWidget = new QWidget(this);
    m_staWidget.setupUi(leftWidget);

    // advanced/system widgets
    QGroupBox *rightWidget = new QGroupBox("Advanced", this);
    QVBoxLayout *vbox = new QVBoxLayout(rightWidget);
    QWidget *adv = new QWidget(rightWidget);
    m_advWidget.setupUi(adv);
    QWidget *sys = new QWidget(rightWidget);
    m_sysWidget.setupUi(sys);
    vbox->addWidget(adv);
    vbox->addWidget(sys);

    spl->addWidget(leftWidget);
    spl->addWidget(rightWidget);

    hbox->setMargin(4);
    hbox->addWidget(spl);

    m_small1 = QIcon(":/icons/icon.bmp");
    m_small2 = QIcon(":/icons/icon.png");
    m_big = QIcon(":/icons/big.png");

    addComboBoxItems();
    addTreeItems();
    addTreeListItems();
    addListItems();
    addTextEdit();
    setupOtherWidgets();
    setupButtons();

    foreach(QWidget *w, qFindChildren<QWidget *>(parentWidget()))
        w->setWhatsThis(w->metaObject()->className());
}

StyleWidget::~StyleWidget()
{

}

void StyleWidget::addTextEdit()
{
    m_staWidget.textEdit->setPlainText(
        "Some Text\nSome Text\nSome Text\nSome Text\nSome Text\nSome Text\nSome Text\nSome Text");
    m_staWidget.textEdit_2->setPlainText(
        "Some Text, Some Text, Some Text, Some Text, Some Text, Some Text, Some Text, Some Text");
    m_staWidget.textEdit_3->setPlainText(
        "Some Text, Some Text, Some Text, Some Text, Some Text, Some Text, Some Text, Some Text" \
        "Some Text\nSome Text\nSome Text\nSome Text\nSome Text\nSome Text\nSome Text\nSome Text\n");
}

void StyleWidget::addComboBoxItems()
{
    m_staWidget.comboBox->addItem("Item 1");
    m_staWidget.comboBox->addItem("Item 2");
    m_staWidget.comboBox->addItem("Item 3");
    m_staWidget.comboBox->addItem("Item 4");
    m_staWidget.comboBox->addItem("Item 5");
}

void StyleWidget::addListItems()
{
    m_staWidget.listWidget->addItem("Item 1");
    m_staWidget.listWidget->addItem("Item 2");
    m_staWidget.listWidget->addItem("Item 3");
    m_staWidget.listWidget->addItem("Item 4");
    m_staWidget.listWidget->addItem("Item 5");

    QListWidgetItem *tmp = new QListWidgetItem("Item 1", m_staWidget.listWidget_2);
    tmp->setCheckState(Qt::Checked);
    tmp = new QListWidgetItem("Item 2", m_staWidget.listWidget_2);
    tmp->setCheckState(Qt::Checked);
    tmp = new QListWidgetItem("Item 3", m_staWidget.listWidget_2);
    tmp->setCheckState(Qt::Checked);
    tmp = new QListWidgetItem("Item 4", m_staWidget.listWidget_2);
    tmp->setCheckState(Qt::Checked);
    tmp = new QListWidgetItem("Item 5", m_staWidget.listWidget_2);
    tmp->setCheckState(Qt::Checked);

    tmp = new QListWidgetItem("Item 1", m_advWidget.listWidget_3);
    tmp->setCheckState(Qt::Checked);
    tmp->setIcon(m_small1);
    tmp = new QListWidgetItem("Item 2", m_advWidget.listWidget_3);
    tmp->setCheckState(Qt::Checked);
    tmp->setIcon(m_small1);
    tmp = new QListWidgetItem("Item 3", m_advWidget.listWidget_3);
    tmp->setCheckState(Qt::Checked);
    tmp->setIcon(m_small1);
    tmp = new QListWidgetItem("Item 4", m_advWidget.listWidget_3);
    tmp->setCheckState(Qt::Checked);
    tmp->setIcon(m_small1);
    tmp = new QListWidgetItem("Item 5", m_advWidget.listWidget_3);
    tmp->setCheckState(Qt::Checked);
    tmp->setIcon(m_small1);

    m_advWidget.listWidget->setViewMode(QListView::IconMode);
    QIcon folder(":/icons/folder.png");
    tmp = new QListWidgetItem("Item 1", m_advWidget.listWidget);
    tmp->setIcon(folder);
    tmp = new QListWidgetItem("Item 2", m_advWidget.listWidget);
    tmp->setIcon(folder);
    tmp = new QListWidgetItem("Item 3", m_advWidget.listWidget);
    tmp->setIcon(folder);
    tmp = new QListWidgetItem("Item 4", m_advWidget.listWidget);
    tmp->setIcon(folder);
    tmp = new QListWidgetItem("Item 5", m_advWidget.listWidget);
    tmp->setIcon(folder);

    tmp = new QListWidgetItem("Item 1", m_advWidget.listWidget_2);
    tmp->setIcon(m_small1);
    tmp = new QListWidgetItem("Item 2", m_advWidget.listWidget_2);
    tmp->setIcon(m_small1);
    tmp = new QListWidgetItem("Item 3", m_advWidget.listWidget_2);
    tmp->setIcon(m_small1);
    tmp = new QListWidgetItem("Item 4", m_advWidget.listWidget_2);
    tmp->setIcon(m_small1);
    tmp = new QListWidgetItem("Item 5", m_advWidget.listWidget_2);
    tmp->setIcon(m_small1);
}

void StyleWidget::setupOtherWidgets()
{
    m_sysWidget.tableWidget->setRowCount(100);
    m_sysWidget.tableWidget->setColumnCount(100);
}

void StyleWidget::addTreeItems()
{
    //standard tree
    m_staWidget.treeWidget_2->setColumnCount(1);
    m_staWidget.treeWidget_2->header()->hide();

    QTreeWidgetItem *tmp;
    QTreeWidgetItem *subtmp;
    QTreeWidgetItem *root1 = new QTreeWidgetItem(m_staWidget.treeWidget_2);
    root1->setText(0, "Root 1");
    QTreeWidgetItem *root2 = new QTreeWidgetItem(m_staWidget.treeWidget_2);
    root2->setText(0, "Root 2");

    for (int i=1; i<=10; ++i)
    {
        tmp = new QTreeWidgetItem(root1);
        tmp->setText(0, QString("Item %1").arg(i));
        for (int j=1; j<=5; ++j)
        {
            subtmp = new QTreeWidgetItem(tmp);
            subtmp->setText(0, QString("Sub Item %1").arg(j));
        }
    }

    // standard checked tree
    m_staWidget.treeWidget_4->setColumnCount(1);
    m_staWidget.treeWidget_4->header()->hide();

    root1 = new QTreeWidgetItem(m_staWidget.treeWidget_4);
    root1->setText(0, "Root 1");
    root1->setCheckState(0, Qt::Checked);
    root2 = new QTreeWidgetItem(m_staWidget.treeWidget_4);
    root2->setText(0, "Root 2");
    root2->setCheckState(0, Qt::Checked);

    for (int i=1; i<=10; ++i)
    {
        tmp = new QTreeWidgetItem(root1);
        tmp->setText(0, QString("Item %1").arg(i));
        tmp->setCheckState(0, Qt::Checked);
        for (int j=1; j<=5; ++j)
        {
            subtmp = new QTreeWidgetItem(tmp);
            subtmp->setText(0, QString("Sub Item %1").arg(j));
            subtmp->setCheckState(0, Qt::Checked);
        }
    }

    // advanced (images) tree
    m_advWidget.treeWidget_2->setColumnCount(1);
    m_advWidget.treeWidget_2->header()->hide();

    root1 = new QTreeWidgetItem(m_advWidget.treeWidget_2);
    root1->setText(0, "Root 1");
    root1->setIcon(0, m_small1);
    root2 = new QTreeWidgetItem(m_advWidget.treeWidget_2);
    root2->setText(0, "Root 2");
    root2->setIcon(0, m_small1);

    for (int i=1; i<=10; ++i)
    {
        tmp = new QTreeWidgetItem(root1);
        tmp->setText(0, QString("Item %1").arg(i));
        tmp->setIcon(0, m_small2);
        for (int j=1; j<=5; ++j)
        {
            subtmp = new QTreeWidgetItem(tmp);
            subtmp->setText(0, QString("Sub Item %1").arg(j));
            tmp->setIcon(0, m_small1);
        }
    }

}

void StyleWidget::addTreeListItems()
{
    //standard list
    QTreeWidgetItem *tmp;
    m_staWidget.treeWidget->setColumnCount(3);
    m_staWidget.treeWidget->headerItem()->setText(0, "Col1");
    m_staWidget.treeWidget->headerItem()->setText(1, "Col2");
    m_staWidget.treeWidget->headerItem()->setText(2, "Col3");

    for (int i=1; i<10; ++i)
    {
        tmp = new QTreeWidgetItem(m_staWidget.treeWidget);
        tmp->setText(0, QString("Item%1").arg(i));
        tmp->setText(1, QString("Item%11").arg(i));
        tmp->setText(2, QString("Item%12").arg(i));
    }

    //standard checked list
    m_staWidget.treeWidget_3->setColumnCount(3);
    m_staWidget.treeWidget_3->headerItem()->setText(0, "Col1");
    m_staWidget.treeWidget_3->headerItem()->setText(1, "Col2");
    m_staWidget.treeWidget_3->headerItem()->setText(2, "Col3");

    for (int i=1; i<10; ++i)
    {
        tmp = new QTreeWidgetItem(m_staWidget.treeWidget_3);
        tmp->setText(0, QString("Item%1").arg(i));
        tmp->setCheckState(0, Qt::Checked);
        tmp->setText(1, QString("Item%11").arg(i));
        tmp->setText(2, QString("Item%12").arg(i));
    }

    //with images
    m_advWidget.treeWidget->setColumnCount(2);
    m_advWidget.treeWidget->headerItem()->setText(0, "Col1");
    m_advWidget.treeWidget->headerItem()->setIcon(0, m_small2);
    m_advWidget.treeWidget->headerItem()->setText(1, "Col2");
    m_advWidget.treeWidget->headerItem()->setIcon(1, m_small2);

    for (int i=1; i<10; ++i)
    {
        tmp = new QTreeWidgetItem(m_advWidget.treeWidget);
        tmp->setText(0, QString("Item%1").arg(i));
        tmp->setIcon(0, m_small1);
        tmp->setText(1, QString("Item%11").arg(i));
    }
}

void StyleWidget::setupButtons()
{
    m_advWidget.pushButton->setIcon(m_small1);
    m_advWidget.pushButton_2->setIcon(m_small1);
    m_advWidget.checkBox->setIcon(m_small2);
    m_advWidget.checkBox_2->setIcon(m_small2);
    m_advWidget.radioButton->setIcon(m_small2);
    m_advWidget.radioButton_2->setIcon(m_small2);

    // tab page images
    m_advWidget.tabWidget->setTabIcon(0, m_small2);
    m_advWidget.tabWidget->setTabIcon(1, m_small2);
}
