/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt scene graph research project.
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

#include <QtCore>
#include <QtGui>

#include <private/qsgdistancefieldglyphcache_p.h>

static void usage()
{
    qWarning("Usage: distfieldgen [options] <font_filename>");
    qWarning(" ");
    qWarning("Distfieldgen generates distance-field renderings of the provided font file,");
    qWarning("one for each font family/style it contains.");
    qWarning("Unless the QT_QML_DISTFIELDDIR environment variable is set, the renderings are");
    qWarning("saved in the fonts/distancefields directory where the Qt libraries are located.");
    qWarning("You can also override the output directory with the -d option.");
    qWarning(" ");
    qWarning(" options:");
    qWarning("  -d <directory>................................ output directory");
    qWarning("  --no-multithread.............................. don't use multiple threads to render distance-fields");
    qWarning("  --force-all-styles............................ force rendering of styles Normal, Bold, Italic and Bold Italic");
    qWarning("  -styles \"style1,style2,..\".................... force rendering of specified styles");

    qWarning(" ");
    exit(1);
}

void printProgress(int p)
{
    printf("\r  [");
    for (int i = 0; i < 50; ++i)
        printf(i < p / 2 ? "=" : " ");
    printf("]");
    printf(" %d%%", p);
    fflush(stdout);
}

class DistFieldGenTask : public QRunnable
{
public:
    DistFieldGenTask(QSGDistanceFieldGlyphCache *atlas, int c, int nbGlyph, QMap<int, QImage> *outList)
        : QRunnable()
        , m_atlas(atlas)
        , m_char(c)
        , m_nbGlyph(nbGlyph)
        , m_outList(outList)
    { }

    void run()
    {
        QImage df = m_atlas->renderDistanceFieldGlyph(m_char);
        QMutexLocker lock(&m_mutex);
        m_outList->insert(m_char, df);
        printProgress(float(m_outList->count()) / m_nbGlyph * 100);
    }

    static QMutex m_mutex;
    QSGDistanceFieldGlyphCache *m_atlas;
    int m_char;
    int m_nbGlyph;
    QMap<int, QImage> *m_outList;
};

QMutex DistFieldGenTask::m_mutex;

static void generateDistanceFieldForFont(const QFont &font, const QString &destinationDir, bool multithread)
{
    QSGDistanceFieldGlyphCache *atlas = QSGDistanceFieldGlyphCache::get(QGLContext::currentContext(), font);
    QFontDatabase db;
    QString fontString = font.family() + QLatin1String(" ") + db.styleString(font);
    qWarning("> Generating distance-field for font '%s' (%d glyphs)", fontString.toLatin1().constData(), atlas->glyphCount());

    QMap<int, QImage> distfields;
    for (int i = 0; i < atlas->glyphCount(); ++i) {
        if (multithread) {
            DistFieldGenTask *task = new DistFieldGenTask(atlas, i, atlas->glyphCount(), &distfields);
            QThreadPool::globalInstance()->start(task);
        } else {
            QImage df = atlas->renderDistanceFieldGlyph(i);
            distfields.insert(i, df);
            printProgress(float(distfields.count()) / atlas->glyphCount() * 100);
        }
    }

    if (multithread)
        QThreadPool::globalInstance()->waitForDone();

    // Combine dist fields in one image
    int size = qCeil(qSqrt(qreal(atlas->glyphCount()))) * 64;
    QImage output(size, size, QImage::Format_ARGB32_Premultiplied);
    output.fill(Qt::transparent);
    QPainter p(&output);
    int x, y = 0;
    for (QMap<int, QImage>::const_iterator i = distfields.constBegin(); i != distfields.constEnd(); ++i) {
        p.drawImage(x, y, i.value());
        x += 64;
        if (x >= size) {
            x = 0;
            y += 64;
        }
    }
    p.end();
    printProgress(100);
    printf("\n");

    // Save output
    QFileInfo dfi(destinationDir);
    if (!dfi.isDir()) {
        qWarning("Error: '%s' is not a directory.", destinationDir.toLatin1().constData());
        qWarning(" ");
        exit(1);
    }

    QString filename = font.family();
    filename.remove(QLatin1String(" "));
    QString italic = font.italic() ? QLatin1String("i") : QLatin1String("");
    QString bold = font.weight() > QFont::Normal ? QLatin1String("b") : QLatin1String("");
    filename = filename + bold + italic;
    QString out = QString(QLatin1String("%1/%2.png")).arg(destinationDir).arg(filename);
    output.save(out);
    qWarning("  Distance-field saved to '%s'\n", out.toLatin1().constData());
}

class MyWidget : public QGLWidget
{
    Q_OBJECT
public:
    MyWidget()
        : QGLWidget()
    { }

    ~MyWidget() { }

    void showEvent(QShowEvent *e)
    {
        QStringList args = QApplication::arguments();

        bool noMultithread = args.contains(QLatin1String("--no-multithread"));
        bool forceAllStyles = args.contains(QLatin1String("--force-all-styles"));

        QString fontFile;
        QString destDir;
        for (int i = 0; i < args.count(); ++i) {
            QString a = args.at(i);
            if (!a.startsWith('-') && QFileInfo(a).exists())
                fontFile = a;
            if (a == QLatin1String("-d"))
                destDir = args.at(++i);
        }
        if (destDir.isEmpty()) {
            destDir = QFileInfo(fontFile).canonicalPath();
        }

        QStringList customStyles;
        if (args.contains(QLatin1String("-styles"))) {
            int index = args.indexOf(QLatin1String("-styles"));
            QString styles = args.at(index + 1);
            customStyles = styles.split(QLatin1String(","));
        }

        // Load the font
        int fontID = QFontDatabase::addApplicationFont(fontFile);
        if (fontID == -1) {
            qWarning("Error: Invalid font file.");
            qWarning(" ");
            exit(1);
        }

        QStringList allStyles = QStringList() << QLatin1String("Normal")
                                              << QLatin1String("Bold")
                                              << QLatin1String("Italic")
                                              << QLatin1String("Bold Italic");

        // Generate distance-fields for all families and all styles provided by the font file
        QFontDatabase fontDatabase;
        QStringList families = QFontDatabase::applicationFontFamilies(fontID);
        int famCount = families.count();
        for (int i = 0; i < famCount; ++i) {
            QStringList styles;
            if (forceAllStyles)
                styles = allStyles;
            else if (customStyles.count() > 0)
                styles = customStyles;
            else
                styles = fontDatabase.styles(families.at(i));

            int styleCount = styles.count();
            for (int j = 0; j < styleCount; ++j) {
                QFont font;
                if (forceAllStyles || customStyles.count() > 0) {
                    int weight = styles.at(j).contains(QLatin1String("Bold")) ? QFont::Bold : QFont::Normal;
                    font = QFont(families.at(i), 10, weight, styles.at(j).contains(QLatin1String("Italic")));
                } else {
                    font = fontDatabase.font(families.at(i), styles.at(j), 10); // point size is ignored
                }
                generateDistanceFieldForFont(font, destDir, !noMultithread);
            }
        }

        exit(0);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList args = QApplication::arguments();

    if (argc < 2
            || args.contains(QLatin1String("--help"))
            || args.contains(QLatin1String("-help"))
            || args.contains(QLatin1String("--h"))
            || args.contains(QLatin1String("-h")))
        usage();


   MyWidget w;
   w.show();

   return app.exec();
}

#include "main.moc"
