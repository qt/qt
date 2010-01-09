/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the utils of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtCore/QTextStream>
#include "lalr.h"
#include "parsetable.h"

ParseTable::ParseTable (QTextStream &o):
  out (o)
{
}

void ParseTable::operator () (Automaton *aut)
{
  Grammar *g = aut->_M_grammar;

  int rindex = 1;
  for (RulePointer rule = g->rules.begin (); rule != g->rules.end (); ++rule)
    out << rindex++ << ")\t" << *rule << endl;
  out << endl << endl;

  int index = 0;
  for (StatePointer state = aut->states.begin (); state != aut->states.end (); ++state)
    {
      out << "state " << index++ << endl << endl;

      for (ItemPointer item = state->kernel.begin (); item != state->kernel.end (); ++item)
        {
          out << " *  " << *item;

          if (item->dot == item->end_rhs ())
            out << " " << aut->lookaheads [item];

          out << endl;
        }

      bool first = true;
      for (Bundle::iterator arrow = state->bundle.begin (); arrow != state->bundle.end (); ++arrow)
        {
          if (! g->isTerminal (arrow.key ()))
            continue;

          if (first)
            out << endl;

          first = false;

          out << "    " << *arrow.key () << " shift, and go to state " << std::distance (aut->states.begin (), *arrow) << endl;
        }

      first = true;
      for (ItemPointer item = state->closure.begin (); item != state->closure.end (); ++item)
        {
          if (item->dot != item->end_rhs () || item->rule == state->defaultReduce)
            continue;

          if (first)
            out << endl;

          first = false;

          foreach (Name la, aut->lookaheads.value (item))
            out << "    " << *la << " reduce using rule " << aut->id (item->rule) << " (" << *item->rule->lhs << ")" << endl;
        }

      first = true;
      for (Bundle::iterator arrow = state->bundle.begin (); arrow != state->bundle.end (); ++arrow)
        {
          if (! g->isNonTerminal (arrow.key ()))
            continue;

          if (first)
            out << endl;

          first = false;

          out << "    " << *arrow.key () << " go to state " << std::distance (aut->states.begin (), *arrow) << endl;
        }

      if (state->defaultReduce != g->rules.end ())
        {
          out << endl
              << "    $default reduce using rule " << aut->id (state->defaultReduce) << " (" << *state->defaultReduce->lhs << ")" << endl;
        }

      out << endl;
    }
}
