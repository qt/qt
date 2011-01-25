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

#include <private/distancefieldfontatlas_p.h>

static void usage()
{
    qWarning("Usage: distfieldgen [options] <font_filename>");
    qWarning(" ");
    qWarning(" options:");
    qWarning("  -d <directory>................................ output directory");
    qWarning("  -b............................................ set bold");
    qWarning("  -i............................................ set italic");

    qWarning(" ");
    exit(1);
}

void printProgress(int p)
{
    printf("\r[");
    for (int i = 0; i < 50; ++i)
        printf(i < p / 2 ? "=" : " ");
    printf("]");
    printf(" %d%%", p);
    fflush(stdout);
}

class DistFieldGenTask : public QRunnable
{
public:
    DistFieldGenTask(const QFont &f, int c, int nbGlyph, QMap<int, QImage> *outList)
        : QRunnable()
        , m_font(f)
        , m_char(c)
        , m_nbGlyph(nbGlyph)
        , m_outList(outList)
    { }

    void run()
    {
        DistanceFieldFontAtlas atlas(m_font);
        QImage df = atlas.renderDistanceFieldGlyph(m_char);
        QMutexLocker lock(&m_mutex);
        m_outList->insert(m_char, df);
        printProgress(float(m_outList->count()) / m_nbGlyph * 100);
    }

    static QMutex m_mutex;
    QFont m_font;
    int m_char;
    int m_nbGlyph;
    QMap<int, QImage> *m_outList;
};

QMutex DistFieldGenTask::m_mutex;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList args = QApplication::arguments();

    if (argc < 2 ||
        args.contains("--help") || args.contains("-help") || args.contains("--h") || args.contains("-h"))
        usage();

    QString fontFile;
    QString destDir;
    for (int i = 0; i < args.count(); ++i) {
        QString a = args.at(i);
        if (!a.startsWith('-') && QFileInfo(a).exists())
            fontFile = a;
        if (a == QLatin1String("-d"))
            destDir = args.at(++i);
    }

    // Load the font
    int fontID = QFontDatabase::addApplicationFont(fontFile);

    if (fontID == -1) {
        qWarning("Error: Invalid font file.");
        qWarning(" ");
        exit(1);
    }

    QFileInfo fi(fontFile);
    if (destDir.isEmpty()) {
        destDir = fi.canonicalPath();
    } else {
        QFileInfo dfi(destDir);
        if (!dfi.isDir()) {
            qWarning("Error: '%s' is not a directory.", destDir.toLatin1().constData());
            qWarning(" ");
            exit(1);
        }
        destDir = dfi.canonicalFilePath();
    }

    bool bold = args.contains("-b");
    bool italic = args.contains("-i");

    QStringList families = QFontDatabase::applicationFontFamilies(fontID);
    QFont f(families.at(0));
    f.setBold(bold);
    f.setItalic(italic);
    DistanceFieldFontAtlas atlas(f);

    QMap<int, QImage> distfields;
    for (int i = 0; i < 0x8A; ++i) {
        distfields.insert(i, atlas.renderDistanceFieldGlyph(i));
        printProgress(float(i) / 0x8A * 100);
    }

    // Combine dist fields in one image
    QImage output(atlas.atlasSize(), QImage::Format_ARGB32_Premultiplied);
    output.fill(Qt::transparent);
    int i = 0;
    QPainter p(&output);
    foreach (const QImage &df, distfields.values()) {
        DistanceFieldFontAtlas::TexCoord c = atlas.glyphTexCoord(i);
        p.drawImage(qRound(c.x * atlas.atlasSize().width()), qRound(c.y * atlas.atlasSize().height()), df);
        ++i;
    }
    p.end();
    printProgress(100);
    printf("\n");

    // Save output
    output.save(QString("%1/%2.png").arg(destDir).arg(fi.baseName()));

    return 0;
}
