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

#include <QtGui>
#include <QtSvg>

#include "window.h"
#include "svgtextobject.h"

Window::Window()
{
    setupGui();
    setupTextObject();

    setWindowTitle("Text Object Example");
}

//![1]
void Window::insertTextObject()
{
    QString fileName = fileNameLineEdit->text();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error Opening File"),
                             tr("Could not open '%1'").arg(fileName));
    }

    QByteArray svgData = file.readAll();
//![1]

//![2]
    QTextCharFormat svgCharFormat;
    svgCharFormat.setObjectType(SvgTextFormat);
    QSvgRenderer renderer(svgData);

    QImage svgBufferImage(renderer.defaultSize(), QImage::Format_ARGB32);
    QPainter painter(&svgBufferImage);
    renderer.render(&painter, svgBufferImage.rect());

    svgCharFormat.setProperty(SvgData, svgBufferImage);

    QTextCursor cursor = textEdit->textCursor();
    cursor.insertText(QString(QChar::ObjectReplacementCharacter), svgCharFormat);
    textEdit->setTextCursor(cursor);
}
//![2]

//![3]
void Window::setupTextObject()
{
    QObject *svgInterface = new SvgTextObject;
    textEdit->document()->documentLayout()->registerHandler(SvgTextFormat, svgInterface);
}
//![3]

void Window::setupGui()
{
    fileNameLabel = new QLabel(tr("Svg File Name:"));
    fileNameLineEdit = new QLineEdit;
    insertTextObjectButton = new QPushButton(tr("Insert Image"));

    fileNameLineEdit->setText("./files/heart.svg");
    connect(insertTextObjectButton, SIGNAL(clicked()),
            this, SLOT(insertTextObject()));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(fileNameLabel);
    bottomLayout->addWidget(fileNameLineEdit);
    bottomLayout->addWidget(insertTextObjectButton);

    textEdit = new QTextEdit;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(textEdit);
    mainLayout->addLayout(bottomLayout);

    setLayout(mainLayout);
}

