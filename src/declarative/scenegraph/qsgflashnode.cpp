#include "qsgflashnode_p.h"

QSGFlashNode::QSGFlashNode()
    : m_counter(1)
{
    setFlag(UsePreprocess);
    setColor(QColor(rand()%56 + 200, rand()%56 + 200, rand()%156 + 100)); // A random, mostly yellow, color
}

void QSGFlashNode::preprocess()
{
    if (m_counter) {
        --m_counter;
    } else {
        delete this;
    }
}
