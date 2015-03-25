/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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
