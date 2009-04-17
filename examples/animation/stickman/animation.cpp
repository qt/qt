#include "animation.h"

#include <QPointF>
#include <QIODevice>
#include <QDataStream>

class Frame 
{
public:
    Frame() {
    }

    int nodeCount() const 
    {
        return m_nodePositions.size();
    }

    void setNodeCount(int nodeCount)
    {
        while (nodeCount > m_nodePositions.size())
            m_nodePositions.append(QPointF());            
        
        while (nodeCount < m_nodePositions.size())
            m_nodePositions.removeLast();
    }

    QPointF nodePos(int idx) const
    {
        return m_nodePositions.at(idx);
    }

    void setNodePos(int idx, const QPointF &pos)
    {
        m_nodePositions[idx] = pos;
    }
    
private:
    QList<QPointF> m_nodePositions;
};

Animation::Animation()
{
    m_currentFrame = 0;
    m_frames.append(new Frame);
}

Animation::~Animation() 
{
    qDeleteAll(m_frames);
}

void Animation::setTotalFrames(int totalFrames)
{
    while (m_frames.size() < totalFrames)
        m_frames.append(new Frame);    

    while (totalFrames < m_frames.size())
        delete m_frames.takeLast();    
}

int Animation::totalFrames() const
{
    return m_frames.size();
}

void Animation::setCurrentFrame(int currentFrame)
{
    m_currentFrame = qMax(qMin(currentFrame, totalFrames()-1), 0);
}

int Animation::currentFrame() const
{
    return m_currentFrame;
}

void Animation::setNodeCount(int nodeCount)
{
    Frame *frame = m_frames.at(m_currentFrame);
    frame->setNodeCount(nodeCount);
}

int Animation::nodeCount() const
{
    Frame *frame = m_frames.at(m_currentFrame);
    return frame->nodeCount();
}

void Animation::setNodePos(int idx, const QPointF &pos)
{
    Frame *frame = m_frames.at(m_currentFrame);
    frame->setNodePos(idx, pos);
}

QPointF Animation::nodePos(int idx) const
{
    Frame *frame = m_frames.at(m_currentFrame);
    return frame->nodePos(idx);
}

QString Animation::name() const
{
    return m_name;
}

void Animation::setName(const QString &name)
{
    m_name = name;
}

void Animation::save(QIODevice *device) const
{
    QDataStream stream(device);
    stream << m_name;
    stream << m_frames.size();
    foreach (Frame *frame, m_frames) {
        stream << frame->nodeCount();
        for (int i=0; i<frame->nodeCount(); ++i)
            stream << frame->nodePos(i);
    }
}

void Animation::load(QIODevice *device)
{
    if (!m_frames.isEmpty())
        qDeleteAll(m_frames);

    m_frames.clear();

    QDataStream stream(device);
    stream >> m_name;
    
    int frameCount;
    stream >> frameCount;

    for (int i=0; i<frameCount; ++i) {
        
        int nodeCount;
        stream >> nodeCount;
        
        Frame *frame = new Frame;
        frame->setNodeCount(nodeCount);

        for (int j=0; j<nodeCount; ++j) {
            QPointF pos;
            stream >> pos;

            frame->setNodePos(j, pos);
        }

        m_frames.append(frame);
    }
}