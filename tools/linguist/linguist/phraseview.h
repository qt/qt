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

#ifndef PHRASEVIEW_H
#define PHRASEVIEW_H

#include <QList>
#include <QShortcut>
#include <QTreeView>
#include "phrase.h"

QT_BEGIN_NAMESPACE

class MultiDataModel;
class PhraseModel;

class GuessShortcut : public QShortcut
{
    Q_OBJECT
public:
    GuessShortcut(int nkey, QWidget *parent, const char *member)
        : QShortcut(parent), nrkey(nkey)
    {
        setKey(Qt::CTRL + (Qt::Key_1 + nrkey));
        connect(this, SIGNAL(activated()), this, SLOT(keyActivated()));
        connect(this, SIGNAL(activated(int)), parent, member);
    }

private slots:
    void keyActivated() { emit activated(nrkey); }

signals:
    void activated(int nkey);

private:
    int nrkey;
};

class PhraseView : public QTreeView
{
    Q_OBJECT

public:
    PhraseView(MultiDataModel *model, QList<QHash<QString, QList<Phrase *> > > *phraseDict, QWidget *parent = 0);
    ~PhraseView();
    void setSourceText(int model, const QString &sourceText);

public slots:
    void toggleGuessing();
    void update();

signals:
    void phraseSelected(int latestModel, const QString &phrase);

protected:
    // QObject
    virtual void contextMenuEvent(QContextMenuEvent *event);
    // QAbstractItemView
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void guessShortcut(int nkey);
    void selectPhrase(const QModelIndex &index);
    void selectPhrase();
    void editPhrase();

private:
    QList<Phrase *> getPhrases(int model, const QString &sourceText);
    void deleteGuesses();

    MultiDataModel *m_dataModel;
    QList<QHash<QString, QList<Phrase *> > > *m_phraseDict;
    QList<Phrase *> m_guesses;
    PhraseModel *m_phraseModel;
    QString m_sourceText;
    int m_modelIndex;
    bool m_doGuesses;
};

QT_END_NAMESPACE

#endif // PHRASEVIEW_H
