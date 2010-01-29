/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "mediaobject.h"

#include "mmf_medianode.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::MediaNode
  \internal
*/

MMF::MediaNode::MediaNode(QObject *parent)
    :   QObject(parent)
    ,   m_mediaObject(qobject_cast<MediaObject *>(this))
    ,   m_input(0)
{

}

MMF::MediaNode::~MediaNode()
{
    // Phonon framework ensures nodes are disconnected before being destroyed.
    Q_ASSERT_X(!m_mediaObject, Q_FUNC_INFO,
        "Media node not disconnected before destruction");
}

bool MMF::MediaNode::connectOutput(MediaNode *output)
{
    Q_ASSERT_X(output, Q_FUNC_INFO, "Null output pointer");

    bool connected = false;

    // Check that this connection will not result in a graph which
    // containing more than one MediaObject
    const bool mediaObjectMisMatch =
            m_mediaObject
        &&  output->m_mediaObject
        &&  m_mediaObject != output->m_mediaObject;

    const bool canConnect =
            !output->isMediaObject()
        &&  !output->m_input
        &&  !m_outputs.contains(output);

    if (canConnect && !mediaObjectMisMatch) {
        output->m_input = this;
        m_outputs += output;
        updateMediaObject();
        connected = true;
    }

    return connected;
}

bool MMF::MediaNode::disconnectOutput(MediaNode *output)
{
    Q_ASSERT_X(output, Q_FUNC_INFO, "Null output pointer");

    bool disconnected = false;

    if (m_outputs.contains(output) && this == output->m_input) {
        output->m_input = 0;
        const bool removed = m_outputs.removeOne(output);
        Q_ASSERT_X(removed, Q_FUNC_INFO, "Output removal failed");

        Q_ASSERT_X(!m_outputs.contains(output), Q_FUNC_INFO,
            "Output list contains duplicate entries");

        // Perform traversal across each of the two graphs separately
        updateMediaObject();
        output->updateMediaObject();

        disconnected = true;
    }

    return disconnected;
}

bool MMF::MediaNode::isMediaObject() const
{
    return (qobject_cast<const MediaObject *>(this) != 0);
}

void MMF::MediaNode::updateMediaObject()
{
    QList<MediaNode *> nodes;
    MediaObject *mediaObject = 0;

    // Traverse the graph, collecting a list of nodes, and locating
    // the MediaObject node, if present
    visit(nodes, mediaObject);

    MediaNode *node = 0;
    foreach(node, nodes)
        node->setMediaObject(mediaObject);
}

void MMF::MediaNode::setMediaObject(MediaObject *mediaObject)
{
    if(!isMediaObject() && m_mediaObject != mediaObject) {
        if (!mediaObject)
            disconnectMediaObject(m_mediaObject);
        else {
            Q_ASSERT_X(!m_mediaObject, Q_FUNC_INFO, "MediaObject already set");
            connectMediaObject(mediaObject);
        }
        m_mediaObject = mediaObject;
    }
}

void MMF::MediaNode::visit(QList<MediaNode *>& visited, MediaObject*& mediaObject)
{
    if (isMediaObject()) {
        // There can never be more than one MediaObject per graph, due to the
        // mediaObjectMisMatch test in connectOutput().
        Q_ASSERT_X(!mediaObject, Q_FUNC_INFO, "MediaObject already found");
        mediaObject = static_cast<MediaObject *>(this);
    }

    visited += this;

    if (m_input && !visited.contains(m_input))
        m_input->visit(visited, mediaObject);

    MediaNode *output = 0;
    foreach (output, m_outputs)
        if (!visited.contains(output))
            output->visit(visited, mediaObject);
}

QT_END_NAMESPACE

