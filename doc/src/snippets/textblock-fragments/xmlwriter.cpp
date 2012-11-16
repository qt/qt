/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
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

#include "xmlwriter.h"

QDomDocument *XmlWriter::toXml()
{
    QDomImplementation implementation;
    QDomDocumentType docType = implementation.createDocumentType(
        "scribe-document", "scribe", "qt.nokia.com/scribe");

    document = new QDomDocument(docType);

    // ### This processing instruction is required to ensure that any kind
    // of encoding is given when the document is written.
    QDomProcessingInstruction process = document->createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"utf-8\"");
    document->appendChild(process);

    QDomElement documentElement = document->createElement("document");
    document->appendChild(documentElement);

//! [0]
    QTextBlock currentBlock = textDocument->begin();

    while (currentBlock.isValid()) {
//! [0]
        QDomElement blockElement = document->createElement("block");
        document->appendChild(blockElement);

        readFragment(currentBlock, blockElement, document);

//! [1]
        processBlock(currentBlock);
//! [1]

//! [2]
        currentBlock = currentBlock.next();
    }
//! [2]

    return document;
}

void XmlWriter::readFragment(const QTextBlock &currentBlock,
                             QDomElement blockElement,
                             QDomDocument *document)
{
//! [3] //! [4]
    QTextBlock::iterator it;
    for (it = currentBlock.begin(); !(it.atEnd()); ++it) {
        QTextFragment currentFragment = it.fragment();
        if (currentFragment.isValid())
//! [3] //! [5]
            processFragment(currentFragment);
//! [4] //! [5]

        if (currentFragment.isValid()) {
            QDomElement fragmentElement = document->createElement("fragment");
            blockElement.appendChild(fragmentElement);

            fragmentElement.setAttribute("length", currentFragment.length());
            QDomText fragmentText = document->createTextNode(currentFragment.text());

            fragmentElement.appendChild(fragmentText);
//! [6]
        }
//! [7]
    }
//! [6] //! [7]
}

void XmlWriter::processBlock(const QTextBlock &currentBlock)
{
}

void XmlWriter::processFragment(const QTextFragment &currentFragment)
{
}
