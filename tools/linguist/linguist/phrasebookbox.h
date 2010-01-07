/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#ifndef PHRASEBOOKBOX_H
#define PHRASEBOOKBOX_H

#include "ui_phrasebookbox.h"
#include "phrase.h"
#include "phrasemodel.h"
#include <QDialog>

QT_BEGIN_NAMESPACE

class TranslationSettingsDialog;

class QSortFilterProxyModel;

class PhraseBookBox : public QDialog, public Ui::PhraseBookBox
{
    Q_OBJECT
public:
    PhraseBookBox(PhraseBook *phraseBook, QWidget *parent = 0);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void newPhrase();
    void removePhrase();
    void settings();
    void save();
    void sourceChanged(const QString &source);
    void targetChanged(const QString &target);
    void definitionChanged(const QString &definition);
    void selectionChanged();

private:
    void selectItem(const QModelIndex &index);
    void enableDisable();
    QModelIndex currentPhraseIndex() const;

    QString fn;
    PhraseBook *m_phraseBook;
    PhraseModel *phrMdl;
    QSortFilterProxyModel *m_sortedPhraseModel;
    TranslationSettingsDialog *m_translationSettingsDialog;
};

QT_END_NAMESPACE

#endif
