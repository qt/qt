/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QFSFILEENGINE_H
#define QFSFILEENGINE_H

#include <QtCore/qabstractfileengine.h>
#ifdef Q_OS_SYMBIAN
#include <f32file.h>
#endif

#ifndef QT_NO_FSFILEENGINE

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QFSFileEnginePrivate;

class Q_CORE_EXPORT QFSFileEngine : public QAbstractFileEngine
{
    Q_DECLARE_PRIVATE(QFSFileEngine)
public:
    QFSFileEngine();
    explicit QFSFileEngine(const QString &file);
    ~QFSFileEngine();

    bool open(QIODevice::OpenMode openMode) override;
    bool open(QIODevice::OpenMode flags, FILE *fh);
    bool close() override;
    bool flush() override;
    qint64 size() const override;
    qint64 pos() const override;
    bool seek(qint64) override;
    bool isSequential() const override;
    bool remove() override;
    bool copy(const QString &newName) override;
    bool rename(const QString &newName) override;
    bool link(const QString &newName) override;
    bool mkdir(const QString &dirName, bool createParentDirectories) const override;
    bool rmdir(const QString &dirName, bool recurseParentDirectories) const override;
    bool setSize(qint64 size) override;
    bool caseSensitive() const override;
    bool isRelativePath() const override;
    QStringList entryList(QDir::Filters filters, const QStringList &filterNames) const override;
    FileFlags fileFlags(FileFlags type) const override;
    bool setPermissions(uint perms) override;
    QString fileName(FileName file) const override;
    uint ownerId(FileOwner) const override;
    QString owner(FileOwner) const override;
    QDateTime fileTime(FileTime time) const override;
    void setFileName(const QString &file) override;
    int handle() const override;

#ifndef QT_NO_FILESYSTEMITERATOR
    Iterator *beginEntryList(QDir::Filters filters, const QStringList &filterNames) override;
    Iterator *endEntryList() override;
#endif

    qint64 read(char *data, qint64 maxlen) override;
    qint64 readLine(char *data, qint64 maxlen) override;
    qint64 write(const char *data, qint64 len) override;

    bool extension(Extension extension, const ExtensionOption *option = nullptr, ExtensionReturn *output = nullptr) override;
    bool supportsExtension(Extension extension) const override;

    //FS only!!
    bool open(QIODevice::OpenMode flags, int fd);
    bool open(QIODevice::OpenMode flags, int fd, QFile::FileHandleFlags handleFlags);
    bool open(QIODevice::OpenMode flags, FILE *fh, QFile::FileHandleFlags handleFlags);
#ifdef Q_OS_SYMBIAN
    bool open(QIODevice::OpenMode flags, const RFile &f, QFile::FileHandleFlags handleFlags);
#endif
    static bool setCurrentPath(const QString &path);
    static QString currentPath(const QString &path = QString());
    static QString homePath();
    static QString rootPath();
    static QString tempPath();
    static QFileInfoList drives();

protected:
    QFSFileEngine(QFSFileEnginePrivate &dd);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_FSFILEENGINE

#endif // QFSFILEENGINE_H
