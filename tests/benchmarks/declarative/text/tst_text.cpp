/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <qtest.h>
#include <QTextLayout>
#include <QPainter>
#include <QSize>
#include <qmath.h>
#include <private/qtextcontrol_p.h>

class tst_text : public QObject
{
    Q_OBJECT
public:
    tst_text()
    {
        m_text = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
    }

private slots:
    void layout();
    void paintLayoutToPixmap();
    void document();
    void paintDocToPixmap();

private:
    QString m_text;
};

QSize setupTextLayout(QTextLayout *layout)
{
    bool wrap = true;
    int wrapWidth = 300;
    layout->setCacheEnabled(true);

    int height = 0;
    qreal widthUsed = 0;
    qreal lineWidth = 0;

    //set manual width
    if (wrap)
        lineWidth = wrapWidth;

    layout->beginLayout();

    while (1) {
        QTextLine line = layout->createLine();
        if (!line.isValid())
            break;

        if (wrap)
            line.setLineWidth(lineWidth);
    }
    layout->endLayout();

    for (int i = 0; i < layout->lineCount(); ++i) {
        QTextLine line = layout->lineAt(i);
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
        line.setPosition(QPointF(0, height));
        height += int(line.height());
    }
    return QSize(qCeil(widthUsed), height);
}

void tst_text::layout()
{
    //get rid of initialization effects
    QTextLayout layout(m_text);
    setupTextLayout(&layout);

    QBENCHMARK {
        QTextLayout layout(m_text);
        setupTextLayout(&layout);
    }
}

void tst_text::paintLayoutToPixmap()
{
    QTextLayout layout(m_text);
    QSize size = setupTextLayout(&layout);

    QBENCHMARK {
        QPixmap img(size);
        img.fill(Qt::transparent);
        QPainter p(&img);
        layout.draw(&p, QPointF(0, 0));
    }
}

void tst_text::document()
{
    QTextControl *control = new QTextControl(m_text);

    QBENCHMARK {
        QTextControl *control = new QTextControl;
        QTextDocument *doc = control->document();
        doc->setHtml(m_text);
    }
}

void tst_text::paintDocToPixmap()
{
    QTextControl *control = new QTextControl;
    QTextDocument *doc = control->document();
    doc->setHtml(m_text);
    QSize size = doc->size().toSize();

    QBENCHMARK {
        QPixmap img(size);
        img.fill(Qt::transparent);
        QPainter p(&img);
        control->drawContents(&p, QRectF(QPointF(0, 0), QSizeF(size)));
    }
}

QTEST_MAIN(tst_text)
#include "tst_text.moc"
