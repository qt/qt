/**
 * Copyright (c) 2011 Nokia Corporation. All rights reserved.
 *
 * Nokia and Nokia Connecting People are registered trademarks of Nokia
 * Corporation. Oracle and Java are registered trademarks of Oracle and/or its
 * affiliates. Other product and company names mentioned herein may be
 * trademarks or trade names of their respective owners.
 *
 *
 * Subject to the conditions below, you may, without charge:
 *
 *  *  Use, copy, modify and/or merge copies of this software and associated
 *     documentation files (the "Software")
 *
 *  *  Publish, distribute, sub-licence and/or sell new software derived from
 *     or incorporating the Software.
 *
 *
 *
 * This file, unmodified, shall be included with all copies or substantial
 * portions of the Software that are distributed in source code form.
 *
 * The Software cannot constitute the primary value of any new software derived
 * from or incorporating the Software.
 *
 * Any person dealing with the Software shall not misrepresent the source of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "button.h"
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

Button::Button(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent, f)
{
    m_downPixmap = 0;
    m_disabled = false;
}

Button::~Button()
{
}

void Button::disableBtn(bool b)
{
    m_disabled = b;
    if (m_disabled) {
        setPixmap(m_downPixmap);
    } else {
        setPixmap(m_upPixmap);
    }
}

void Button::mousePressEvent(QMouseEvent *event)
{
    if (!m_disabled) {
        event->accept();
        setPixmap(m_downPixmap);
        repaint();
        // Lift button back to up after 300ms
        QTimer::singleShot(300, this, SLOT(backToUp()));
    }
}

void Button::backToUp()
{
    setPixmap(m_upPixmap);
    repaint();
    emit pressed();
}

void Button::setPixmap(const QPixmap& p)
{
    // Set up and down picture for the button
    // Set pixmap
    if (!p.isNull())
        QLabel::setPixmap(p);

    // Make down pixmap if it does not exists
    if (m_downPixmap.isNull()) {
        // Store up pixmap
        m_upPixmap = *pixmap();

        // Create down pixmap
        // Make m_downPixmap as a transparent m_upPixmap
        QPixmap transparent(m_upPixmap.size());
        transparent.fill(Qt::transparent);
        QPainter painter(&transparent);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(0, 0, m_upPixmap);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.fillRect(transparent.rect(), QColor(0, 0, 0, 150));
        painter.end();
        m_downPixmap = transparent;
    }

}
