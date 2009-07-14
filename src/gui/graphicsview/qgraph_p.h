#include <QtCore/QHash>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <float.h>

template <typename Vertex, typename EdgeData>
class Graph
{
public:
    Graph() {}

    class const_iterator {
    public:
        const_iterator(const Graph *graph, bool begin) : g(graph){
            if (begin) {
                row = g->m_graph.constBegin();
                //test if the graph is empty
                if (row != g->m_graph.constEnd())
                {
                    column = (*row)->constBegin();
                }
            } else {
                row = g->m_graph.constEnd();
            }
        }

        inline Vertex *operator*() {
            return column.key();
        }

        inline bool operator==(const const_iterator &o) const { return !(*this != o); }
        inline bool operator!=(const const_iterator &o) const {
           if (row ==  g->m_graph.end()) {
                return row != o.row;
           } else {
                return row != o.row || column != o.column;
           }
        }
        inline const_iterator& operator=(const const_iterator &o) const { row = o.row; column = o.column; return *this;}

        // prefix
        const_iterator &operator++() {
            if (row != g->m_graph.constEnd()) {
                ++column;
                if (column == (*row)->constEnd()) {
                    ++row;
                    if (row != g->m_graph.constEnd()) {
                        column = (*row)->constBegin();
                    }
                }
            }
            return *this;
        }

    private:
        const Graph *g;
        Q_TYPENAME QHash<Vertex *, QHash<Vertex *, EdgeData *> * >::const_iterator row;
        Q_TYPENAME QHash<Vertex *, EdgeData *>::const_iterator column;
    };

    const_iterator constBegin() const {
        return const_iterator(this,true);
    }

    const_iterator constEnd() const {
        return const_iterator(this,false);
    }

    /*!
     * \internal
     *
     * If there is an edge between \a first and \a second, it will return a structure
     * containing the data associated with the edge, otherwise it will return 0.
     *
     */
    EdgeData *edgeData(Vertex* first, Vertex* second) {
        QHash<Vertex *, EdgeData *> *row = m_graph.value(first);
        return row ? row->value(second) : 0;
    }

    void createEdge(Vertex *first, Vertex *second, EdgeData *data)
    {
        // Creates a bidirectional edge
        createDirectedEdge(first, second, data);
        createDirectedEdge(second, first, data);
    }

    void removeEdge(Vertex *first, Vertex *second)
    {
        // Removes a bidirectional edge
        EdgeData *data = edgeData(first, second);
        removeDirectedEdge(first, second);
        removeDirectedEdge(second, first);
        if (data) delete data;
    }

    EdgeData *takeEdge(Vertex* first, Vertex* second)
    {
        // Removes a bidirectional edge
        EdgeData *data = edgeData(first, second);
        if (data) {
            removeDirectedEdge(first, second);
            removeDirectedEdge(second, first);
        }
        return data;
    }

    QList<Vertex *> adjacentVertices(Vertex *vertex) const
    {
        QHash<Vertex *, EdgeData *> *row = m_graph.value(vertex);
        QList<Vertex *> l;
        if (row)
            l = row->keys();
        return l;
    }

    void setRootVertex(Vertex *vertex)
    {
        userVertex = vertex;
    }

    QSet<Vertex*> vertices() const {
        QSet<Vertex *> setOfVertices;
        for (const_iterator it = constBegin(); it != constEnd(); ++it) {
            setOfVertices.insert(*it);
        }
        return setOfVertices;
    }

    QString serializeToDot() {   // traversal
        QString strVertices;
        QString edges;

        QSet<Vertex *> setOfVertices = vertices();
        for (Q_TYPENAME QSet<Vertex*>::const_iterator it = setOfVertices.begin(); it != setOfVertices.end(); ++it) {
            Vertex *v = *it;
            QList<Vertex*> adjacents = adjacentVertices(v);
            for (int i = 0; i < adjacents.count(); ++i) {
                Vertex *v1 = adjacents.at(i);
                EdgeData *data = edgeData(v, v1);
                bool forward = data->origin == v;
                if (forward) {
                    edges += QString::fromAscii("%1->%2 [label=\"[%3,%4]\" dir=both color=\"#000000:#a0a0a0\"] \n")
                        .arg(v->toString())
                        .arg(v1->toString())
                        .arg(data->minSize)
                        .arg(data->maxSize)
                        ;
                }
            }
            strVertices += QString::fromAscii("%1 [label=\"%2\"]\n").arg(v->toString()).arg(v->toString());
        }
        return QString::fromAscii("%1\n%2\n").arg(strVertices).arg(edges);
    }

    Vertex *rootVertex() const
    {
        return userVertex;
    }

protected:
    void createDirectedEdge(Vertex *from, Vertex *to, EdgeData *data)
    {
        QHash<Vertex *, EdgeData *> *adjacentToFirst = m_graph.value(from);
        if (!adjacentToFirst) {
            adjacentToFirst = new QHash<Vertex *, EdgeData *>();
            m_graph.insert(from, adjacentToFirst);
        }
        adjacentToFirst->insert(to, data);
    }

    void removeDirectedEdge(Vertex *from, Vertex *to)
    {
        QHash<Vertex *, EdgeData *> *adjacentToFirst = m_graph.value(from);
        Q_ASSERT(adjacentToFirst);

        adjacentToFirst->remove(to);
        if (adjacentToFirst->isEmpty()) {
            //nobody point to 'from' so we can remove it from the graph
            m_graph.remove(from);
            delete adjacentToFirst;
        }
    }

private:
    Vertex *userVertex;

    QHash<Vertex *, QHash<Vertex *, EdgeData *> *> m_graph;
};
