#ifndef CLOCKDISPLAY_H
#define CLOCKDISPLAY_H

#include <QObject>
#include <QGraphicsItem>
#include <QDateTime>

class ClockDisplay: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_ENUMS(DisplayMode)
    Q_PROPERTY(DisplayMode displayMode READ displayMode WRITE setDisplayMode)
    Q_PROPERTY(QDateTime currentTime READ currentTime WRITE setCurrentTime)
    Q_PROPERTY(QTime alarm READ alarm WRITE setAlarm)
    Q_PROPERTY(bool alarmEnabled READ alarmEnabled WRITE setAlarmEnabled)
public:
    enum DisplayMode {
        CurrentTimeMode,        
        EditSecondMode,
        EditMinuteMode,
        EditHourMode,

        CurrentDateMode,
        EditMonthMode,
        EditDayMode,
        EditYearMode,

        AlarmMode,
        EditAlarmHourMode,
        EditAlarmTenMinuteMode,
        EditAlarmMinuteMode,

        ChimeMode,
        StopWatchMode,
        ModeCount
    };

    ClockDisplay(QGraphicsItem *parent = 0);
    virtual ~ClockDisplay();

    void setDisplayMode(DisplayMode displayMode) { m_displayMode = displayMode; update(); }
    DisplayMode displayMode() const { return m_displayMode; }

    QDateTime currentTime() const { return m_currentTime; }
    void setCurrentTime(const QDateTime &time) { m_currentTime = time; update(); }

    QTime alarm() const { return m_alarm; }
    void setAlarm(const QTime &time) { m_alarm = time; update(); }

    bool alarmEnabled() const { return m_alarmEnabled; }
    void setAlarmEnabled(bool enabled) { m_alarmEnabled = enabled; update(); }

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

private slots:
    void toggleBlinkFlag();

private:
    void updateText();

    DisplayMode m_displayMode;

    QDateTime m_currentTime;
    QTime m_alarm;
    
    QGraphicsTextItem *m_text;
    QGraphicsPixmapItem *m_alarmSymbol;

    uint m_alarmEnabled : 1;
    uint m_blink : 1;
    uint m_reserved : 30; 
};

#endif
