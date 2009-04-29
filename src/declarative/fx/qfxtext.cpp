/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qfxtext.h"
#include "qfxtext_p.h"

#include <private/qtextcontrol_p.h>

#if defined(QFX_RENDER_OPENGL2)
#include "glbasicshaders.h"
#endif

#include <qfxperf.h>
#include <QTextLayout>
#include <QTextLine>
#include <QTextDocument>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>


QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(QFxText,Text);

/*!
    \qmlclass Text QFxText
    \brief The Text element allows you to add formatted text to a scene.
    \inherits Item

    It can display both plain and rich text. For example:

    \code
    <Text text="Hello World!" font.family="Helvetica" font.size="24" color="red"/>
    <Text>
        <![CDATA[<b>Hello</b> <i>World!</i>]]]]><![CDATA[>
    </Text>
    \endcode

    \image declarative-text.png

    Additional examples can be found in examples/poc/text.xml

    If height and width are not explicitly set, Text will attempt to determine how 
    much room is needed and set it accordingly. Unless \c wrap is set, it will always
    prefer width to height (all text will be placed on a single line).

    The \c elide property can alternatively be used to fit a single line of
    plain text to a set width.

    Text provides read-only text. For editable text, see \l TextEdit.
*/

/*!
    \internal
    \class QFxText
    \qmlclass Text
    \ingroup group_coreitems

    \brief The QFxText class provides a formatted text item that you can add to a QFxView.

    Text was designed for read-only text; it does not allow for any text editing. 
    It can display both plain and rich text. For example:

    \code
    <Text text="Hello World!" font.family="Helvetica" font.size="24" color="red"/>
    <Text>
        <![CDATA[<b>Hello</b> <i>World!</i>]]>
    </Text>
    \endcode

    \image text.png

    Note that the 'styling' properties such as color and outline are ignored for rich text, styling
    of rich text should be done within the text itself.

    Additional examples can be found in examples/poc/text.xml

    If height and width are not explicitly set, Text will attempt to determine how 
    much room is needed and set it accordingly. Unless \c wrap is set, it will always
    prefer width to height (all text will be placed on a single line).

    The \c elideMode can alternatively be used to fit a line of plain text to a set width.

    A QFxText object can be instantiated in Qml using the tag \c &lt;Text&gt;.
*/
QFxText::QFxText(QFxItem *parent)
  : QFxItem(*(new QFxTextPrivate), parent)
{
    Q_D(QFxText);
    d->init();
    setAcceptedMouseButtons(Qt::LeftButton);
    setOptions(SimpleItem | HasContents | QFxText::MouseEvents);
}

QFxText::QFxText(QFxTextPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    Q_D(QFxText);
    d->init();
    setAcceptedMouseButtons(Qt::LeftButton);
    setOptions(SimpleItem | HasContents | QFxText::MouseEvents);
}

QFxText::~QFxText()
{
}

/*!
    \qmlproperty font Text::font

    Set the Text's font attributes.  \c font.size sets the font's point size.
*/

/*!
    \property QFxText::font
    \brief the font used to display the text.
*/

QmlFont *QFxText::font()
{
    Q_D(QFxText);
    return d->font();
}

void QFxText::setText(const QString &n)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::QFxText_setText> st;
#endif
    Q_D(QFxText);
    if (d->text == n)
        return;

    d->richText = Qt::mightBeRichText(n);   // ### what's the cost?
    if (d->richText) {
        if (!d->doc)
        {
            d->control = new QTextControl(this); 
            d->control->setTextInteractionFlags(Qt::TextBrowserInteraction);
            d->doc = d->control->document(); 
            d->doc->setDocumentMargin(0);
        }    
        d->doc->setHtml(n);
    }

    d->text = n;
    d->imgDirty = true;
    d->updateSize();
    emit textChanged(d->text);
    update();
}

/*!
    \qmlproperty string Text::text
    \default

    The text to display.  Text supports both plain and rich text strings.

    The item will try to automatically determine whether the text should
    be treated as rich text. This determination is made using Qt::mightBeRichText().
*/
QString QFxText::text() const
{
    Q_D(const QFxText);
    return d->text;
}

void QFxText::setColor(const QColor &color)
{
    Q_D(QFxText);
    if (d->color == color)
        return;

    d->imgDirty = true;
    d->color = color;
    update();
}

/*!
    \qmlproperty color Text::color

    The text color.

    \code
    <!-- green text using hexadecimal notation -->
    <Text color="#00FF00" .../>

    <!-- steelblue text using SVG color name-->
    <Text color="steelblue" .../>
    \endcode
*/

QColor QFxText::color() const
{
    Q_D(const QFxText);
    return d->color;
}

/*!
    \qmlproperty enumeration Text::style

    Set an additional text style.

    Supported text styles are \c Normal, \c Outline, \c Raised and \c Sunken.

    \code
    <HorizontalLayout>
        <Text font.size="24" text="Normal" />
        <Text font.size="24" text="Raised" style="Raised" styleColor="#AAAAAA"/>
        <Text font.size="24" text="Outline" style="Outline" styleColor="red"/>
        <Text font.size="24" text="Sunken" style="Sunken" styleColor="#AAAAAA"/>
    </HorizontalLayout>
    \endcode

    \image declarative-textstyle.png
*/

/*!
    \property QFxText::style
    \brief an additional style of the text to display.

    By default, the text style is Normal.

    \note This property is used to support text styles not natively
    handled by QFont or QPainter::drawText().
*/
QFxText::TextStyle QFxText::style() const
{
    Q_D(const QFxText);
    return d->style;
}

void QFxText::setStyle(QFxText::TextStyle style)
{
    Q_D(QFxText);
    if (d->style == style)
        return;

    d->imgDirty = true;
    d->style = style;
    update();
}

void QFxText::setStyleColor(const QColor &color)
{
    Q_D(QFxText);
    if (d->styleColor == color)
        return;

    d->imgDirty = true;
    d->styleColor = color;
    update();
}

/*!
    \qmlproperty color Text::styleColor

    Defines the secondary color used by text styles.

    \c styleColor is used as the outline color for outlined text, and as the
    shadow color for raised or sunken text. If no style has been set, it is not
    used at all.
 */
QColor QFxText::styleColor() const
{
    Q_D(const QFxText);
    return d->styleColor;
}

/*!
    \qmlproperty enumeration Text::hAlign
    \qmlproperty enumeration Text::vAlign

    Sets the horizontal and vertical alignment of the text within the Text elements
    width and height.  By default, the text is top-left aligned.

    The valid values for \c hAlign are \c AlignLeft, \c AlignRight and 
    \c AlignHCenter.  The valid values for \c vAlign are \c AlignTop, \c AlignBottom 
    and \c AlignVCenter.
*/

/*!
    \property QFxText::hAlign
    \brief the horizontal alignment of the text.

    Valid values are \c AlignLeft, \c AlignRight, and \c AlignHCenter. The default value is \c AlignLeft.
*/
QFxText::HAlignment QFxText::hAlign() const
{
    Q_D(const QFxText);
    return d->hAlign;
}

void QFxText::setHAlign(HAlignment align)
{
    Q_D(QFxText);
    d->hAlign = align;
}

/*!
    \property QFxText::vAlign
    \brief the vertical alignment of the text.

    Valid values are \c AlignTop, \c AlignBottom, and \c AlignVCenter. The default value is \c AlignTop.
*/
QFxText::VAlignment QFxText::vAlign() const
{
    Q_D(const QFxText);
    return d->vAlign;
}

void QFxText::setVAlign(VAlignment align)
{
    Q_D(QFxText);
    d->vAlign = align;
}

/*!
    \qmlproperty bool Text::wrap

    Set this property to wrap the text to the Text element's width.  The text will only
    wrap if an explicit width has been set.

    Wrapping is done on word boundaries (i.e. it is a "word-wrap"). If the text cannot be
    word-wrapped to the specified width it will be partially drawn outside of the item's bounds.
    If this is undesirable then enable clipping on the item (Item::clip).

    Wrapping is off by default.
*/
//### Future may provide choice of wrap modes, such as QTextOption::WrapAtWordBoundaryOrAnywhere
bool QFxText::wrap() const
{
    Q_D(const QFxText);
    return d->wrap;
}

void QFxText::setWrap(bool w)
{
    Q_D(QFxText);
    if (w == d->wrap)
        return;

    d->wrap = w;

    d->imgDirty = true;
    d->updateSize();
}

/*!
    \qmlproperty Qt::TextElideMode Text::elideMode

    Set this property to elide parts of the text fit to the Text element's width.
    The text will only elide if an explicit width has been set.

    This property cannot be used with wrap enabled or with rich text.

    Eliding can be ElideNone, ElideLeft, ElideMiddle, or ElideRight.

    ElideNone is the default.
*/
Qt::TextElideMode QFxText::elideMode() const
{
    Q_D(const QFxText);
    return d->elideMode;
}

void QFxText::setElideMode(Qt::TextElideMode mode)
{
    Q_D(QFxText);
    if (mode == d->elideMode)
        return;

    d->elideMode = mode;

    d->imgDirty = true;
    d->updateSize();
}


QString QFxText::activeLink() const
{
    Q_D(const QFxText);
    return d->activeLink;
}

void QFxText::geometryChanged(const QRectF &newGeometry, 
                              const QRectF &oldGeometry)
{
    Q_D(QFxText);
    if (newGeometry.width() != oldGeometry.width()) {
        if (d->wrap || d->elideMode != Qt::ElideNone) {
            d->imgDirty = true;
            d->updateSize();
        }
    }
    QFxItem::geometryChanged(newGeometry, oldGeometry);
}


void QFxText::fontChanged()
{
    Q_D(QFxText);
    d->imgDirty = true;
    d->updateSize();
    emit update();
}

void QFxTextPrivate::updateSize()
{
    Q_Q(QFxText);
    if (q->isComponentComplete()) {
        if (text.isEmpty()) {
            return;
        }
        QFont f; if (_font) f = _font->font();
        QFontMetrics fm(f);
        int dy = q->height();

        QString tmp;
        QSize size(0, 0);

        //setup instance of QTextLayout for all cases other than richtext
        if (!richText) 
            {
                tmp = text;
                tmp.replace(QLatin1Char('\n'), QChar::LineSeparator);
                singleline = !tmp.contains(QChar::LineSeparator);
                if (singleline && elideMode != Qt::ElideNone && q->widthValid())
                    tmp = fm.elidedText(tmp,elideMode,q->width()); // XXX still worth layout...?
                QTextLayout layout;
                layout.setFont(f);
                layout.setText(tmp);
                size = setupTextLayout(&layout);
            }
        if (richText) {
            singleline = false; // richtext can't elide or be optimized for single-line case
            doc->setDefaultFont(f);
            QTextOption option((Qt::Alignment)int(hAlign | vAlign));
            if (wrap)
                option.setWrapMode(QTextOption::WordWrap);
            else
                option.setWrapMode(QTextOption::NoWrap);
            doc->setDefaultTextOption(option);
            if (wrap && !q->heightValid() && q->widthValid())
                doc->setTextWidth(q->width());
            else
                doc->setTextWidth(doc->idealWidth()); // ### Text does not align if width is not set (QTextDoc bug)
            dy -= (int)doc->size().height();
        } else {
            dy -= size.height();
        } 
        int yoff = 0;

        if (q->heightValid()) {
            if (vAlign == QFxText::AlignBottom)
                yoff = dy;
            else if (vAlign == QFxText::AlignVCenter)
                yoff = dy/2;
        }
        q->setBaselineOffset(fm.ascent() + yoff);
        
        if (!q->widthValid()) {
            int newWidth = (richText ? (int)doc->idealWidth() : size.width());
            q->setImplicitWidth(newWidth);
        }
        if (!q->heightValid()) {
            if (richText) {
                q->setImplicitHeight((int)doc->size().height());
            } else {
                q->setImplicitHeight(size.height());
            }
        }
    } else {
        dirty = true;
    }
}

// ### text layout handling should be profiled and optimized as needed
// what about QStackTextEngine engine(tmp, d->font.font()); QTextLayout textLayout(&engine);

void QFxText::dump(int depth)
{
    QByteArray ba(depth * 4, ' ');
    qWarning() << ba.constData() << propertyInfo();
    QFxItem::dump(depth);
}

QString QFxText::propertyInfo() const
{
    Q_D(const QFxText);
    return QChar(QLatin1Char('\"')) + d->text + QChar(QLatin1Char('\"'));
}

void QFxTextPrivate::drawOutline()
{
    QImage img = QImage(imgCache.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter ppm(&img);
    img.fill(qRgba(0, 0, 0, 0));

    QPoint pos(imgCache.rect().topLeft());
    pos += QPoint(-1, 0);
    ppm.drawImage(pos, imgStyleCache);
    pos += QPoint(2, 0);
    ppm.drawImage(pos, imgStyleCache);
    pos += QPoint(-1, -1);
    ppm.drawImage(pos, imgStyleCache);
    pos += QPoint(0, 2);
    ppm.drawImage(pos, imgStyleCache);

    pos += QPoint(0, -1);
    QPainter &p = ppm;
    p.drawImage(pos, imgCache);

    imgCache = QSimpleCanvasConfig::toImage(img);
}

void QFxTextPrivate::drawOutline(int yOffset)
{
    QImage img = QImage(imgCache.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter ppm(&img);
    img.fill(qRgba(0, 0, 0, 0));

    QPoint pos(imgCache.rect().topLeft());
    pos += QPoint(0, yOffset);
    ppm.drawImage(pos, imgStyleCache);

    pos += QPoint(0, -yOffset);
    QPainter &p = ppm;
    p.drawImage(pos, imgCache);

    imgCache = QSimpleCanvasConfig::toImage(img);
}

QSize QFxTextPrivate::setupTextLayout(QTextLayout *layout)
{
    Q_Q(QFxText);
    layout->setCacheEnabled(true);

    QFont f; if (_font) f = _font->font();
    QFontMetrics fm = QFontMetrics(f);

    int leading = fm.leading();
    int height = 0;
    qreal widthUsed = 0;
    qreal lineWidth = 0;

    //set manual width
    if ((wrap || elideMode != Qt::ElideNone) && q->widthValid())
        lineWidth = q->width();

    layout->beginLayout();

    while (1) {
        QTextLine line = layout->createLine();
        if (!line.isValid())
            break;
        
        if ((wrap || elideMode != Qt::ElideNone) && q->widthValid())
            line.setLineWidth(lineWidth);
    }
    layout->endLayout();

    if (layout->lineCount() == 1)
        height -= leading;

    for (int i = 0; i < layout->lineCount(); ++i) {
        QTextLine line = layout->lineAt(i);
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
        line.setPosition(QPointF(0, height));
        height += int(line.height());
    }
    return QSize((int)widthUsed, height);
}

QImage QFxTextPrivate::wrappedTextImage(bool drawStyle)
{
    //do layout
    Q_Q(const QFxText);
    QFont f; if (_font) f = _font->font();
    QString tmp = text;
    if (singleline && elideMode != Qt::ElideNone && q->widthValid()) {
        QFontMetrics fm(f);
        tmp = fm.elidedText(tmp,elideMode,q->width()); // XXX still worth layout...?
    }
    tmp.replace(QLatin1Char('\n'), QChar::LineSeparator);
    QTextLayout textLayout(tmp, f);
    QSize size = setupTextLayout(&textLayout);

    int x = 0;
    for (int i = 0; i < textLayout.lineCount(); ++i) {
        QTextLine line = textLayout.lineAt(i);
        if (hAlign == QFxText::AlignLeft) {
            x = 0;
        } else if (hAlign == QFxText::AlignRight) {
            x = size.width() - (int)line.naturalTextWidth();
        } else if (hAlign == QFxText::AlignHCenter) {
            x = (size.width() - (int)line.naturalTextWidth()) / 2;
        }
        line.setPosition(QPoint(x, (int)line.y()));
    }

    //paint text
    QImage img(size, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
    QPainter p(&img);
    if (drawStyle) {
        p.setPen(styleColor);
    }
    else 
        p.setPen(color);
    p.setFont(f);
    textLayout.draw(&p, QPointF(0, 0));
    return img;
}

QImage QFxTextPrivate::richTextImage(bool drawStyle)
{
    QSize size = doc->size().toSize();

    //paint text
    QImage img(size, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);
    QPainter p(&img);

    if (drawStyle) {
        QPalette pal = control->palette();
        pal.setColor(QPalette::Text, styleColor);
        control->setPalette(pal);
        QTextOption colorOption;
        colorOption.setFlags(QTextOption::SuppressColors);
        doc->setDefaultTextOption(colorOption);
    } else {
        QPalette pal = control->palette();
        pal.setColor(QPalette::Text, color);
        control->setPalette(pal);
    }
    control->drawContents(&p, QRectF(QPointF(0, 0), QSizeF(size)));
    if (drawStyle)
        doc->setDefaultTextOption(QTextOption());
    return img;
}

void QFxTextPrivate::checkImgCache()
{
    if (!imgDirty)
        return;

    bool empty = text.isEmpty();
    if (empty) {
        imgCache = QSimpleCanvasConfig::Image();
        imgStyleCache = QImage();
    } else if (richText) {
        imgCache = QSimpleCanvasConfig::toImage(richTextImage(false));
        if (style != QFxText::Normal)
            imgStyleCache = richTextImage(true); //### should use styleColor
    } else {
        imgCache = QSimpleCanvasConfig::toImage(wrappedTextImage(false));
        if (style != QFxText::Normal)
            imgStyleCache = wrappedTextImage(true); //### should use styleColor
    }
    if (!empty)
        switch (style) {
        case QFxText::Outline:
            drawOutline();
            break;
        case QFxText::Sunken:
            drawOutline(-1);
            break;
        case QFxText::Raised:
            drawOutline(1);
            break;
        default:
            break;
        }

#if defined(QFX_RENDER_OPENGL)
    tex.setImage(imgCache);
#endif

    imgDirty = false;
}

#if defined(QFX_RENDER_QPAINTER)
void QFxText::paintContents(QPainter &p)
{
    Q_D(QFxText);
    d->checkImgCache();
    if (d->imgCache.isNull())
        return;

    int w = width();
    int h = height();

    int x = 0;
    int y = 0;

    switch (d->hAlign) {
    case AlignLeft:
        x = 0;
        break;
    case AlignRight:
        x = w - d->imgCache.width();
        break;
    case AlignHCenter:
        x = (w - d->imgCache.width()) / 2;
        break;
    }

    switch (d->vAlign) {
    case AlignTop:
        y = 0;
        break;
    case AlignBottom:
        y = h - d->imgCache.height();
        break;
    case AlignVCenter:
        y = (h - d->imgCache.height()) / 2;
        break;
    }

    p.drawImage(x, y, d->imgCache);
}

#elif defined(QFX_RENDER_OPENGL2)
void QFxText::paintGLContents(GLPainter &p)
{
    Q_D(QFxText);
    d->checkImgCache();
    if (d->imgCache.isNull())
        return;

    int w = width();
    int h = height();

    float x = 0;
    float y = 0;

    switch (d->hAlign) {
    case AlignLeft:
        x = 0;
        break;
    case AlignRight:
        x = w - d->imgCache.width();
        break;
    case AlignHCenter:
        x = (w - d->imgCache.width()) / 2;
        break;
    }

    switch (d->vAlign) {
    case AlignTop:
        y = 0;
        break;
    case AlignBottom:
        y = h - d->imgCache.height();
        break;
    case AlignVCenter:
        y = (h - d->imgCache.height()) / 2;
        break;
    }

    float widthV = d->imgCache.width();
    float heightV = d->imgCache.height();

    QGLShaderProgram *shader = p.useTextureShader();

    GLfloat vertices[] = { x, y + heightV,
        x + widthV, y + heightV,
        x, y, 
        x + widthV, y };

    GLfloat texVertices[] = { 0, 0, 
                              1, 0, 
                              0, 1,
                              1, 1 };

    shader->setAttributeArray(SingleTextureShader::Vertices, vertices, 2);
    shader->setAttributeArray(SingleTextureShader::TextureCoords, texVertices, 2);

    glBindTexture(GL_TEXTURE_2D, d->tex.texture());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    shader->disableAttributeArray(SingleTextureShader::Vertices);
    shader->disableAttributeArray(SingleTextureShader::TextureCoords);
}

#elif defined(QFX_RENDER_OPENGL)
void QFxText::paintGLContents(GLPainter &p)
{
    Q_D(QFxText);
    d->checkImgCache();
    if (d->imgCache.isNull())
        return;

    int w = width();
    int h = height();

    float x = 0;
    float y = 0;

    switch (d->hAlign) {
    case AlignLeft:
        x = 0;
        break;
    case AlignRight:
        x = w - d->imgCache.width();
        break;
    case AlignHCenter:
        x = (w - d->imgCache.width()) / 2;
        break;
    }

    switch (d->vAlign) {
    case AlignTop:
        y = 0;
        break;
    case AlignBottom:
        y = h - d->imgCache.height();
        break;
    case AlignVCenter:
        y = (h - d->imgCache.height()) / 2;
        break;
    }

    float widthV = d->imgCache.width();
    float heightV = d->imgCache.height();

    GLfloat vertices[] = { x, y + heightV,
        x + widthV, y + heightV,
        x, y, 
        x + widthV, y };

    GLfloat texVertices[] = { 0, 0, 
                              1, 0, 
                              0, 1,
                              1, 1 };

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(p.activeTransform.data());
    glEnable(GL_TEXTURE_2D);
    if (p.activeOpacity == 1.) {
        GLint i = GL_REPLACE;
        glTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &i);
    } else {
        GLint i = GL_MODULATE;
        glTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &i);
        glColor4f(1, 1, 1, p.activeOpacity);
    }

    glBindTexture(GL_TEXTURE_2D, d->tex.texture());

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texVertices);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

#endif

void QFxText::componentComplete()
{
    Q_D(QFxText);
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::TextComponentComplete> cc;
#endif
    QFxItem::componentComplete();
    if (d->dirty) {
        d->updateSize();
        d->dirty = false;
    }
}

/*!
  \overload
  Handles the given mouse \a event.
 */
void QFxText::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxText);

    if (!d->richText || !d->doc || d->control->anchorAt(event->pos()).isEmpty()) {
        event->setAccepted(false);
        d->activeLink = QString();
    } else {
        d->activeLink = d->control->anchorAt(event->pos());
    }

    // ### may malfunction if two of the same links are clicked & dragged onto each other)

    if (!event->isAccepted())
        QFxItem::mousePressEvent(event);
 
}

/*!
  \overload
  Handles the given mouse \a event.
 */
void QFxText::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxText);

        // ### confirm the link, and send a signal out
    if (d->richText && d->doc && d->activeLink == d->control->anchorAt(event->pos()))
        emit linkActivated(d->activeLink);
    else
        event->setAccepted(false);

    if (!event->isAccepted())
        QFxItem::mouseReleaseEvent(event);
}
QT_END_NAMESPACE
