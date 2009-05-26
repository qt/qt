#ifndef TEXTWRITER_H
#define TEXTWRITER_H

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtGui/QTextCursor>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

namespace JavaScript {

class TextWriter
{
    QString *string;
    QTextCursor *cursor;

    struct Replace {
        int pos;
        int length;
        QString replacement;
    };

    QList<Replace> replaceList;

    struct Move {
        int pos;
        int length;
        int to;
    };

    QList<Move> moveList;

    bool hasOverlap(int pos, int length);
    bool hasMoveInto(int pos, int length);

    void doReplace(const Replace &replace);
    void doMove(const Move &move);

    void write_helper();

public:
    TextWriter();

    void replace(int pos, int length, const QString &replacement);
    void move(int pos, int length, int to);

    void write(QString *s);
    void write(QTextCursor *textCursor);

};

} // end of namespace JavaScript

QT_END_NAMESPACE
QT_END_HEADER

#endif // TEXTWRITER_H
