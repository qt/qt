/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "informationwindow.h"

//! [0]
InformationWindow::InformationWindow(int id, QSqlRelationalTableModel *offices,
                                     QWidget *parent)
    : QDialog(parent)
{
//! [0] //! [1]
    QLabel *locationLabel = new QLabel(tr("Location: "));
    QLabel *countryLabel = new QLabel(tr("Country: "));
    QLabel *descriptionLabel = new QLabel(tr("Description: "));
    QLabel *imageFileLabel = new QLabel(tr("Image file: "));

    createButtons();

    locationText = new QLabel;
    countryText = new QLabel;
    descriptionEditor = new QTextEdit;
//! [1]

//! [2]
    imageFileEditor = new QComboBox;
    imageFileEditor->setModel(offices->relationModel(1));
    imageFileEditor->setModelColumn(offices->relationModel(1)->fieldIndex("file"));
//! [2]

//! [3]
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(offices);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(imageFileEditor, 1);
    mapper->addMapping(locationText, 2, "text");
    mapper->addMapping(countryText, 3, "text");
    mapper->addMapping(descriptionEditor, 4);
    mapper->setCurrentIndex(id);
//! [3]

//! [4]
    connect(descriptionEditor, SIGNAL(textChanged()),
            this, SLOT(enableButtons()));
    connect(imageFileEditor, SIGNAL(currentIndexChanged(int)),
            this, SLOT(enableButtons()));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(locationLabel, locationText);
    formLayout->addRow(countryLabel, countryText);
    formLayout->addRow(imageFileLabel, imageFileEditor);
    formLayout->addRow(descriptionLabel, descriptionEditor);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(formLayout);
    layout->addWidget(buttonBox);
    setLayout(layout);

    locationId = id;
    displayedImage = imageFileEditor->currentText();

    setWindowFlags(Qt::Window);
    enableButtons(false);
    setWindowTitle(tr("Office: %1").arg(locationText->text()));
}
//! [4]

//! [5]
int InformationWindow::id()
{
    return locationId;
}
//! [5]

//! [6]
void InformationWindow::revert()
{
    mapper->revert();
    enableButtons(false);
}
//! [6]

//! [7]
void InformationWindow::submit()
{
    QString newImage(imageFileEditor->currentText());

    if (displayedImage != newImage) {
        displayedImage = newImage;
        emit imageChanged(locationId, newImage);
    }

    mapper->submit();
    mapper->setCurrentIndex(locationId);

    enableButtons(false);
}
//! [7]

//! [8]
void InformationWindow::createButtons()
{
    closeButton = new QPushButton(tr("&Close"));
    revertButton = new QPushButton(tr("&Revert"));
    submitButton = new QPushButton(tr("&Submit"));

    closeButton->setDefault(true);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(submitButton, SIGNAL(clicked()), this, SLOT(submit()));
//! [8]

//! [9]
    buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton(submitButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(revertButton, QDialogButtonBox::ResetRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);
}
//! [9]

//! [10]
void InformationWindow::enableButtons(bool enable)
{
    revertButton->setEnabled(enable);
    submitButton->setEnabled(enable);
}
//! [10]


