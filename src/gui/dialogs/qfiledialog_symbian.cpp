/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qfiledialog.h"

#ifndef QT_NO_FILEDIALOG

#include <private/qfiledialog_p.h>
#if defined(Q_WS_S60) && defined(SYMBIAN_VERSION_SYMBIAN3)
#include <driveinfo.h>
#include <AknCommonDialogsDynMem.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <MAknFileFilter.h>
#endif
#include "private/qcore_symbian_p.h"

QT_BEGIN_NAMESPACE

enum DialogMode { DialogOpen, DialogSave, DialogFolder };
#if defined(Q_WS_S60) && defined(SYMBIAN_VERSION_SYMBIAN3)
class CExtensionFilter : public MAknFileFilter
{
public:
    void setFilter(const QString filter)
    {
        filterList.clear();
        if (filter.left(2) == QLatin1String("*.")) {
            //Filter has only extensions
            filterList << filter.split(QLatin1String(" "));
            return;
        } else {
            //Extensions are in parenthesis and there may be several filters
            QStringList separatedFilters(filter.split(QLatin1String(";;")));
            for (int i = 0; i < separatedFilters.size(); i++) {
                if (separatedFilters.at(i).contains(QLatin1String("(*)"))) {
                    filterList << QLatin1String("(*)");
                    return;
                }
            }
            QRegExp rx(QLatin1String("\\(([^\\)]*)\\)"));
            int pos = 0;
            while ((pos = rx.indexIn(filter, pos)) != -1) {
                filterList << rx.cap(1).split(QLatin1String(" "));
                pos += rx.matchedLength();
            }
        }
    }

    TBool Accept(const TDesC &/*aDriveAndPath*/, const TEntry &aEntry) const
    {
        if (aEntry.IsDir())
            return ETrue;

        //If no filter for files, all can be accepted
        if (filterList.isEmpty())
            return ETrue;

        if (filterList == QStringList(QLatin1String("(*)")))
            return ETrue;

        for (int i = 0; i < filterList.size(); ++i) {
            QString extension = filterList.at(i);
            //remove '*' from the beginning of the extension
            if (extension.at(0) == QLatin1Char('*'))
                extension = extension.mid(1);

            QString fileName = qt_TDesC2QString(aEntry.iName);
            if (fileName.endsWith(extension))
                return ETrue;
        }
        return EFalse;
    }

private:
    QStringList filterList;
};
#endif

static QString launchSymbianDialog(const QString dialogCaption, const QString startDirectory,
                                   const QString filter, DialogMode dialogMode)
{
    QString selection;
#if defined(Q_WS_S60) && defined(SYMBIAN_VERSION_SYMBIAN3)
    TFileName startFolder;
    if (!startDirectory.isEmpty()) {
        QString dir = QDir::toNativeSeparators(QFileDialogPrivate::workingDirectory(startDirectory));
        startFolder = qt_QString2TPtrC(dir);
    }
    TInt types = AknCommonDialogsDynMem::EMemoryTypeMMCExternal|
                 AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage|
                 AknCommonDialogsDynMem::EMemoryTypePhone;

    TPtrC titlePtr(qt_QString2TPtrC(dialogCaption));
    TFileName target;
    bool select = false;
    int tryCount = 2;
    while (tryCount--) {
        TInt err(KErrNone);
        TRAP(err,
            if (dialogMode == DialogOpen) {
                CExtensionFilter* extensionFilter = new (ELeave) CExtensionFilter;
                CleanupStack::PushL(extensionFilter);
                extensionFilter->setFilter(filter);
                select = AknCommonDialogsDynMem::RunSelectDlgLD(types, target,
                         startFolder, 0, 0, titlePtr, extensionFilter);
                CleanupStack::Pop(extensionFilter);
            } else if (dialogMode == DialogSave) {
                QString defaultFileName = QFileDialogPrivate::initialSelection(startDirectory);
                target = qt_QString2TPtrC(defaultFileName);
                select = AknCommonDialogsDynMem::RunSaveDlgLD(types, target,
                         startFolder, 0, 0, titlePtr);
            } else if (dialogMode == DialogFolder) {
                select = AknCommonDialogsDynMem::RunFolderSelectDlgLD(types, target, startFolder,
                            0, 0, titlePtr, NULL, NULL);
            }
        );

        if (err == KErrNone) {
            tryCount = 0;
        } else {
            // Symbian native file dialog doesn't allow accessing files outside C:/Data
            // It will always leave in that case, so default into QDir::rootPath() in error cases.
            QString dir = QDir::toNativeSeparators(QDir::rootPath());
            startFolder = qt_QString2TPtrC(dir);
        }
    }
    if (select) {
        QFileInfo fi(qt_TDesC2QString(target));
        selection = fi.absoluteFilePath();
    }
#endif
    return selection;
}

QString qtSymbianGetOpenFileName(const QString &caption,
                                 const QString &dir,
                                 const QString &filter)
{
    return launchSymbianDialog(caption, dir, filter, DialogOpen);
}

QStringList qtSymbianGetOpenFileNames(const QString &caption,
                                      const QString &dir,
                                      const QString &filter)
{
    QString fileName;
    fileName.append(launchSymbianDialog(caption, dir, filter, DialogOpen));
    QStringList fileList;
    fileList << fileName;

    return fileList;
}

QString qtSymbianGetSaveFileName(const QString &caption,
                                 const QString &dir)
{
    return launchSymbianDialog(caption, dir, QString(), DialogSave);
}

QString qtSymbianGetExistingDirectory(const QString &caption,
                                      const QString &dir)
{
    QString folderCaption;
    if (!caption.isEmpty()) {
        folderCaption.append(caption);
    } else {
        // Title for folder selection dialog is mandatory
        folderCaption.append(QFileDialog::tr("Find Directory"));
    }
    return launchSymbianDialog(folderCaption, dir, QString(), DialogFolder);
}

QT_END_NAMESPACE

#endif
