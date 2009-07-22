#include "qguard_p.h"
#include <private/qobject_p.h>

void q_guard_addGuard(QGuard<QObject> *g)
{
    QObjectPrivate *p = QObjectPrivate::get(g->o);
    if (p->wasDeleted) {
        qWarning("QGuard: cannot add guard to deleted object");
        g->o = 0;
        return;
    }

    g->next = p->objectGuards;
    p->objectGuards = g;
    g->prev = &p->objectGuards;
    if (g->next)
        g->next->prev = &g->next;
}

void q_guard_removeGuard(QGuard<QObject> *g)
{
    if (g->next) g->next->prev = g->prev;
    *g->prev = g->next;
    g->next = 0;
    g->prev = 0;
}

