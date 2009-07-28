/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2009 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_CONTROLLEREVENTSRULER_H_
#define _RG_CONTROLLEREVENTSRULER_H_

//#include <Q3Canvas>
//#include <Q3CanvasLine>
#include "ControlRuler.h"
#include "base/Event.h"
#include "base/Segment.h"
#include <QString>

class QWidget;
class QMouseEvent;
//class Q3CanvasLine;
//class Q3Canvas;


namespace Rosegarden
{

class Segment;
class RulerScale;
class Event;
//class EditViewBase;
class ControlParameter;
class ControlItem;


/**
 * ControllerEventsRuler : edit Controller events
 */
class ControllerEventsRuler : public ControlRuler, public SegmentObserver
{
public:
    ControllerEventsRuler(MatrixViewSegment*,
                          RulerScale*,
//                          EditViewBase* parentView,
//                          Q3Canvas*,
                          QWidget* parent=0,
                          const ControlParameter *controller = 0,
                          const char* name=0 );	//, WFlags f=0);

    virtual ~ControllerEventsRuler();

    virtual void paintEvent(QPaintEvent *);

    virtual QString getName();
    int getDefaultItemWidth() { return m_defaultItemWidth; }

    // Allow something external to reset the selection of Events
    // that this ruler is displaying
    //
    virtual void setViewSegment(MatrixViewSegment *);
    virtual void setSegment(Segment *);

    // SegmentObserver interface
//    virtual void elementAdded(const ViewSegment *, ViewElement*);
    virtual void eventAdded(const Segment *, Event *);
//    virtual void elementRemoved(const ViewSegment *, ViewElement*);
    virtual void eventRemoved(const Segment *, Event *);
//    virtual void viewSegmentDeleted(const ViewSegment *);
    virtual void segmentDeleted(const Segment *);

    virtual void insertControllerEvent(float,float);
    virtual void eraseControllerEvent();
    virtual void clearControllerEvents();
//    virtual void startControlLine();

    ControlParameter* getControlParameter() { return m_controller; }

public slots:
    virtual void slotSetTool(const QString&);
    virtual void slotSetPannedRect(QRectF pr);

protected:
    virtual void init();
    virtual bool isOnThisRuler(Event *);
    virtual void addControlItem(Event *);

//    virtual void drawBackground(); Implemented in paintEvent

    // Let's override these again here
    //
    virtual void contentsMousePressEvent(QMouseEvent*);
    virtual void contentsMouseReleaseEvent(QMouseEvent*);
    virtual void contentsMouseMoveEvent(QMouseEvent*);

    virtual void layoutItem(ControlItem*);

    void drawControlLine(timeT startTime,
                         timeT endTime,
                         int startValue,
                         int endValue);

    //--------------- Data members ---------------------------------
    int                           m_defaultItemWidth;

    ControlParameter  *m_controller;
//    Q3CanvasLine                   *m_controlLine;

//    bool                           m_controlLineShowing;
//    int                            m_controlLineX;
//    int                            m_controlLineY;
};


}

#endif
