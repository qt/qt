#ifndef ANIMATION_H
#define ANIMATION_H

#include <QPointF>
#include <QList>
#include <QString>

class Frame;
class QIODevice;
class Animation
{
public:
    Animation();
    ~Animation();

    void setTotalFrames(int totalFrames);
    int totalFrames() const;

    void setCurrentFrame(int currentFrame);
    int currentFrame() const;

    void setNodeCount(int nodeCount);
    int nodeCount() const;

    void setNodePos(int idx, const QPointF &pos);
    QPointF nodePos(int idx) const;

    QString name() const;
    void setName(const QString &name);

    void save(QIODevice *device) const;
    void load(QIODevice *device);

private:
    QString m_name;
    QList<Frame *> m_frames;
    int m_currentFrame;
};

#endif
