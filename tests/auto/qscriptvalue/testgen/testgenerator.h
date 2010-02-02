/*
  Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef TESTGENERATOR_H
#define TESTGENERATOR_H

#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qtemporaryfile.h>

class TestGenerator {
public:
    TestGenerator(QString& outputpath)
        : m_ofile(outputpath)
    {
        // Open output file
        if (!m_ofile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Can't open output file: " << outputpath;
            exit(2);
        }
        m_tempFile.open();
    }

    void run()
    {
        prepareData();
        Q_ASSERT(m_tempFile.size());
        save(generateTest());
    }

    void prepareData();
    QString generateTest();
    void save(const QString& data);
private:
    QFile m_ofile;
    QTemporaryFile m_tempFile;
};

#endif // TESTGENERATOR_H
