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

    class iterator {
    public:
        iterator(Graph *graph, bool begin) : g(graph){
            if (begin) {
              row = g->m_graph.begin();
              //test if the graph is empty
              if (row != g->m_graph.end())
              {
                 column = (*row)->begin();
              }
            } else {
               row = g->m_graph.end();
            }
        }

        inline Vertex *operator*() {
            return column.key();
        }

        inline bool operator==(const iterator &o) const { return !(*this != o); }
        inline bool operator!=(const iterator &o) const {
           if (row ==  g->m_graph.end()) {
                return row != o.row;}
           else {
                return row != o.row || column != o.column;
           }
        }
        inline iterator& operator=(const iterator &o) const { row = o.row; column = o.column; return *this;}

        // prefix
        iterator &operator++() {
            if (row != g->m_graph.end()) {
                ++column;
                if (column == (*row)->end()) {
                    ++row;
                    if (row != g->m_graph.end()) {
                        column = (*row)->begin();
                    }
                }
            }
            return *this;
        }

    private:
        Graph *g;
        Q_TYPENAME QHash<Vertex *, QHash<Vertex *, EdgeData *> * >::iterator row;
        Q_TYPENAME QHash<Vertex *, EdgeData *>::iterator column;
    };

    iterator begin() {
        return iterator(this,true);
    }

    iterator end() {
        return iterator(this,false);
    }

    EdgeData *edgeData(Vertex *first, Vertex *second) {
        return m_graph.value(first)->value(second);
    }

    void createEdge(Vertex *first, Vertex *second, EdgeData *data)
    {
        // Creates a bidirectional edge
        createDirectedEdge(first, second, data);
        createDirectedEdge(second, first, data);
    }

    void removeEdge(Vertex *first, Vertex *second)
    {
        // Creates a bidirectional edge
        EdgeData *data = edgeData(first, second);
        if (data) delete data;
        removeDirectedEdge(first, second);
        removeDirectedEdge(second, first);
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

    QString serializeToDot() {   // traversal
        QString vertices;
        QString edges;
        QQueue<Vertex *> queue;
        QSet<Vertex *> visited;
        bool ok;
        Vertex *v = firstVertex(&ok);
        if (ok) {
            queue.enqueue(v);
        }
        while (!queue.isEmpty()) {
            Vertex *v = queue.dequeue();
            vertices += QString::fromAscii("%1 [label=\"%2\"]\n").arg(v->toString()).arg(v->toString());
            visited.insert(v);
            // visit it here
            QList<Vertex *> vertices = adjacentVertices(v);
            for (int i = 0; i < vertices.count(); ++i) {
                Vertex *v1 = vertices.at(i);
                EdgeData *data = edgeData(v, v1);
                edges+=QString::fromAscii("%1->%2 [label=\"[%3,%4]\"]\n").arg(v->toString()).arg(v1->toString()).arg(data->minSize).arg(data->maxSize);
                if (!queue.contains(v1) && !visited.contains(v1)) {
                    queue.enqueue(v1);
                } else {
                    // a cycle....
                }
            }
        }
        return QString::fromAscii("digraph anchorlayout {\nnode [shape=\"rect\"]\n%1%2}").arg(vertices).arg(edges);
    }

    Vertex *firstVertex(bool *ok)
    {
        if (userVertex) {
            *ok = true;
            return userVertex;
        }

        Vertex *v = 0;
        *ok = false;
        Q_TYPENAME Graph::iterator it = Graph::begin();
        if (it != Graph::end()) {
            v = *it;
            *ok = true;
        }
        return v;
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
        adjacentToFirst->remove(to);
        if (adjacentToFirst->isEmpty()) {
           //nobody point to 'from' so we can remove it from the graph
           QHash<Vertex *, EdgeData *> *adjacentToFirst = m_graph.take(from);
           delete adjacentToFirst;
           delete from;
        }
    }

private:
    Vertex *userVertex;

    QHash<Vertex *, QHash<Vertex *, EdgeData *> *> m_graph;
};
