/***************************************************************************
                          notationelement.h  -  description
                             -------------------
    begin                : Sun Sep 10 2000
    copyright            : (C) 2000 by Guillaume Laurent, Chris Cannam, Rich Bown
    email                : glaurent@telegraph-road.org, cannam@all-day-breakfast.com, bownie@bownie.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTATIONELEMENT_H
#define NOTATIONELEMENT_H

#include <multiset.h>
#include "Element2.h"

class QCanvasItem;
class NotationElementList;

/**
  *@author Guillaume Laurent, Chris Cannam, Rich Bown
  */

class NotationElement : public ViewElement
{

public:
    NotationElement(Event *event);

    ~NotationElement();

    double x() { return m_x; }
    double y() { return m_y; }

    void setX(double x) { m_x = x; }
    void setY(double y) { m_y = y; }

    bool isRest() const;
    bool isNote() const;

    /// The object takes ownership of its canvas item
    void setCanvasItem(QCanvasItem *e);
    QCanvasItem* canvasItem() { return m_canvasItem; }

protected:
    double m_x;
    double m_y;

    QCanvasItem *m_canvasItem;
};

class kdbgstream;
#ifndef NDEBUG
kdbgstream& operator<<(kdbgstream&, NotationElement&);
#else
inline kndgstream& operator<<(kdbgstream &e, NotationElement&)
{ return e; }
#endif

class NotationElementCmp
{
public:
    bool operator()(const NotationElement *e1, const NotationElement *e2) const
    {
        return  *e1 < *e2;
    }
};

/**
 * This class owns the NotationElements its items are pointing at
 */
class NotationElementList : public multiset<NotationElement*, NotationElementCmp>
{
public:
    NotationElementList() : multiset<NotationElement*, NotationElementCmp>() {};
    ~NotationElementList();

    void erase(iterator pos);

    iterator findPrevious(const string &package, const string &type, iterator i) {
        //!!! what to return on failure? I think probably
        // end(), as begin() could be a success case
        if (i == begin()) return end();
        --i;
        for (;;) {
            if ((*i)->event()->isa(package, type)) return i;
            if (i == begin()) return end();
            --i;
        }
    }

    iterator findNext(const string &package, const string &type, iterator i) {
        if (i == end()) return i;
        for (++i; i != end() && !(*i)->event()->isa(package, type); ++i);
        return i;
    }
};

#ifndef NDEBUG
kdbgstream& operator<<(kdbgstream&, NotationElementList&);
#else
inline kndgstream& operator<<(kdbgstream &e, NotationElementList&)
{ return e; }
#endif


// inline bool operator<(NotationElement &e1, NotationElement &e2)
// {
//     return e1.m_x < e2.m_x;
// }


#endif
