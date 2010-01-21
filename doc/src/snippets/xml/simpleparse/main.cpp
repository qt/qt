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

#include <qfile.h>
#include <qxml.h>
#include "handler.h"

#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    QFile *file = new QFile(argv[1]);

//! [0]
    QXmlSimpleReader xmlReader;
    QXmlInputSource *source = new QXmlInputSource(file);
//! [0]

//! [1]
    Handler *handler = new Handler;
    xmlReader.setContentHandler(handler);
    xmlReader.setErrorHandler(handler);
//! [1]

//! [2]
    bool ok = xmlReader.parse(source);

    if (!ok)
        std::cout << "Parsing failed." << std::endl;
//! [2]
    else {
        QStringList names = handler->names();
        QList<int> indentations = handler->indentations();

        int items = names.count();

        for (int i = 0; i < items; ++i) {
            for (int j = 0; j < indentations[i]; ++j)
                std::cout << " ";
            std::cout << names[i].toLocal8Bit().constData() << std::endl;
        }
    }

    return 0;
}
