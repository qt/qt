#include "clockdisplay.h"

#include <QPainter>
#include <QTimer>

ClockDisplay::ClockDisplay(QGraphicsItem *parent)
    : QGraphicsItem(parent), 
      m_displayMode(CurrentTimeMode),
      m_currentTime(QDate(1970, 1, 1), QTime(0, 0)),
      m_alarm(0, 0),
      m_alarmEnabled(false),
      m_blink(false)

{
    m_text = new QGraphicsTextItem(this);
    
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(toggleBlinkFlag()));
    timer->start(500);
    
    QFont font = m_text->font();
    font.setPointSizeF(20.0);
    m_text->setFont(font);

    m_text->translate(-100.0, -20.0);

    QPixmap pm(":/images/alarm.png");
    m_alarmSymbol = new QGraphicsPixmapItem(pm, this);
    m_alarmSymbol->translate(-100.0, -60.0);
    m_alarmSymbol->setOffset(5.0, 5.0);
    m_alarmSymbol->scale(0.5, 0.5);
}

ClockDisplay::~ClockDisplay()
{
}

void ClockDisplay::toggleBlinkFlag()
{
    m_blink = !m_blink;
    update();
}

void ClockDisplay::updateText() 
{
    switch (m_displayMode) {
    case EditSecondMode:
        if (m_blink) {
            m_text->setHtml(m_currentTime.toString("hh:mm:'<font color=\"transparent\">'ss'</font>'"));
            break;
        } 
        // fall throoough
    case EditMinuteMode:
        if (m_blink) {
            m_text->setHtml(m_currentTime.toString("hh:'<font color=\"transparent\">'mm'</font>':ss"));
            break;
        }
        
        // fall throoough
    case EditHourMode:
        if (m_blink) {
            m_text->setHtml(m_currentTime.toString("'<font color=\"transparent\">'hh'</font>':mm:ss"));
            break;
        }

        // fall throoough
    case CurrentTimeMode:
        m_text->setHtml(m_currentTime.toString("hh:mm:ss"));
        break;

    case EditMonthMode:
        if (m_blink) {
            m_text->setHtml(m_currentTime.toString("yyyy.'<font color=\"transparent\">'MM'</font>'.dd"));
            break;
        }
        
        // fall throoough
    case EditDayMode:
        if (m_blink) {
            m_text->setHtml(m_currentTime.toString("yyyy.MM.'<font color=\"transparent\">'dd'</font>'"));
            break;
        }

        // fall throoough
    case EditYearMode:
        if (m_blink) {
            m_text->setHtml(m_currentTime.toString("'<font color=\"transparent\">'yyyy'</font>'.MM.dd"));
            break;
        }

        // fall throoough
    case CurrentDateMode:
        m_text->setHtml(m_currentTime.toString("yyyy.MM.dd"));
        break;

    case EditAlarmHourMode:
        if (m_blink) {
            m_text->setHtml(m_alarm.toString("'<font color=\"transparent\">'hh'</font>':mm"));
            break;        
        }

        // fall throooough
    case EditAlarmTenMinuteMode:
    case EditAlarmMinuteMode:
        if (m_blink) {
            m_text->setHtml(m_alarm.toString("hh:'<font color=\"transparent\">'mm'</font>'"));
            break;
        }

        // fall throoough
    case AlarmMode:
        m_text->setHtml(m_alarm.toString("hh:mm"));
        break;

    default:
        m_text->setHtml("Not implemented");
    };
}

QRectF ClockDisplay::boundingRect() const 
{
    return QRectF(-100.0, -60.0, 200.0, 120.0);
}

void ClockDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (m_alarmEnabled)
        m_alarmSymbol->setVisible(true);

    updateText(); 

    // Screen
    painter->drawRoundedRect(boundingRect(), 15.0, 15.0, Qt::RelativeSize);

    // Button grid
    painter->drawLine(QPointF(-100.0, -20.0), QPointF(100.0, -20.0));
    painter->drawLine(QPointF(-50.0, -60.0), QPointF(-50.0, -20.0));
    painter->drawLine(QPointF(0.0, -60.0), QPointF(0.0, -20.0));
    painter->drawLine(QPointF(50.0, -60.0), QPointF(50.0, -20.0));
}
