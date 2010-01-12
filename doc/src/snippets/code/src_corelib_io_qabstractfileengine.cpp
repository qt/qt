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

//! [0]
class ZipEngineHandler : public QAbstractFileEngineHandler
{
public:
    QAbstractFileEngine *create(const QString &fileName) const;
};

QAbstractFileEngine *ZipEngineHandler::create(const QString &fileName) const
{
    // ZipEngineHandler returns a ZipEngine for all .zip files
    return fileName.toLower().endsWith(".zip") ? new ZipEngine(fileName) : 0;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    ZipEngineHandler engine;

    MainWindow window;
    window.show();

    return app.exec();
}
//! [0]


//! [1]
QAbstractSocketEngine *ZipEngineHandler::create(const QString &fileName) const
{
    // ZipEngineHandler returns a ZipEngine for all .zip files
    return fileName.toLower().endsWith(".zip") ? new ZipEngine(fileName) : 0;
}
//! [1]


//! [2]
QAbstractFileEngineIterator *
CustomFileEngine::beginEntryList(QDir::Filters filters, const QStringList &filterNames)
{
    return new CustomFileEngineIterator(filters, filterNames);
}
//! [2]


//! [3]
class CustomIterator : public QAbstractFileEngineIterator
{
public:
    CustomIterator(const QStringList &nameFilters, QDir::Filters filters)
        : QAbstractFileEngineIterator(nameFilters, filters), index(0)
    {
        // In a real iterator, these entries are fetched from the
        // file system based on the value of path().
        entries << "entry1" << "entry2" << "entry3";
    }

    bool hasNext() const
    {
        return index < entries.size() - 1;
    }

    QString next()
    {
       if (!hasNext())
           return QString();
       ++index;
       return currentFilePath();
    }

    QString currentFileName()
    {
        return entries.at(index);
    }

private:
    QStringList entries;
    int index;
};
//! [3]
