// -*- c-basic-offset: 4 -*-

/*
    Rosegarden-4 v0.1
    A sequencer and musical notation editor.

    This program is Copyright 2000-2001
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <bownie@bownie.com>

    The moral right of the authors to claim authorship of this work
    has been asserted.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include <algorithm>

#include "trackeditor.h"
#include "segmentcanvas.h"
#include "rosegardenguidoc.h"

#include "rosedebug.h"

#include <qlayout.h>
#include <qcanvas.h>

#include <kmessagebox.h>

using Rosegarden::Composition;

static double canvasPosition;

TrackEditor::TrackEditor(RosegardenGUIDoc* doc,
                           QWidget* parent, const char* name,
                           WFlags) :
    QWidget(parent, name),
    DCOPObject("TrackEditorIface"),
    m_document(doc),
    m_segmentsCanvas(0),
    m_hHeader(0),
    m_vHeader(0)
{
    int segments(doc->getNbSegments());
    int bars(doc->getNbBars());
/*
    if (segments == 0) segments = 64;
    if (bars == 0) bars = 100;
*/
    segments = 64;
    bars = 100;

    init(segments, bars);
}


TrackEditor::TrackEditor(unsigned int nbSegments,
                           unsigned int nbBars,
                           QWidget *parent,
                           const char *name,
                           WFlags) :
    QWidget(parent, name),
    m_document(0),
    m_segmentsCanvas(0),
    m_hHeader(0),
    m_vHeader(0)
{
    init(nbSegments, nbBars);
}


void
TrackEditor::init(unsigned int nbSegments, unsigned int nbBars)
{
    kdDebug(KDEBUG_AREA) << "TrackEditor::init(nbSegments = "
                         << nbSegments << ", nbBars = " << nbBars
                         << ")" << endl;

    QGridLayout *grid = new QGridLayout(this, 2, 2);

    grid->addWidget(m_hHeader = new QHeader(nbBars, this), 0, 1);
    grid->addWidget(m_vHeader =
         new Rosegarden::TrackHeader(nbSegments, this), 1, 0);
    m_vHeader->setOrientation(Qt::Vertical);

    setupHorizontalHeader();

    // set up vert. header
    for (int i = 0; i < m_vHeader->count(); ++i) {
        m_vHeader->resizeSection(i, 25);
        m_vHeader->setLabel(i, QString("Instr. %1").arg(i));
    }

    m_vHeader->setMinimumWidth(100);
    m_vHeader->setResizeEnabled(false);

    QObject::connect(m_vHeader, SIGNAL(indexChange(int,int,int)),
                     this, SLOT(segmentOrderChanged(int,int,int)));

    QCanvas *canvas = new QCanvas(this);
    canvas->resize(m_hHeader->sectionSize(0) * nbBars,
                   m_vHeader->sectionSize(0) * nbSegments);

    canvas->setBackgroundColor(Qt::lightGray);

    m_segmentsCanvas = new SegmentCanvas(m_hHeader->sectionSize(0),
                                      m_vHeader->sectionSize(0),
                                      *canvas, this);

    grid->addWidget(m_segmentsCanvas, 1,1);
    connect(this, SIGNAL(needUpdate()),
            m_segmentsCanvas, SLOT(update()));

    QObject::connect(m_segmentsCanvas, SIGNAL(addSegment(SegmentItem*)),
                     this,           SLOT(addSegment(SegmentItem*)));

    QObject::connect(m_segmentsCanvas, SIGNAL(deleteSegment(Rosegarden::Segment*)),
                     this,           SLOT(deleteSegment(Rosegarden::Segment*)));

    QObject::connect(m_segmentsCanvas, SIGNAL(updateSegmentDuration(SegmentItem*)),
                     this,           SLOT(updateSegmentDuration(SegmentItem*)));

    QObject::connect(m_segmentsCanvas, SIGNAL(updateSegmentInstrumentAndStartIndex(SegmentItem*)),
                     this,           SLOT(updateSegmentInstrumentAndStartIndex(SegmentItem*)));

    // create the position pointer
    m_pointer = new QCanvasLine(canvas);
    m_pointer->setPen(Qt::darkBlue);
    m_pointer->setPoints(0, 0, 0, canvas->height());
    m_pointer->setZ(10);
    m_pointer->show();

}

void
TrackEditor::setupSegments()
{
    kdDebug(KDEBUG_AREA) << "TrackEditor::setupSegments() begin" << endl;

    if (!m_document) return; // sanity check
    
    Composition &comp = m_document->getComposition();

    for (Composition::iterator i = comp.begin(); i != comp.end(); ++i) {

        if ((*i)) {

            kdDebug(KDEBUG_AREA) << "TrackEditor::setupSegments() add segment"
                                 << " - start idx : " << (*i)->getStartIndex()
                                 << " - nb time steps : " << (*i)->getDuration()
                                 << " - instrument : " << (*i)->getInstrument()
                                 << endl;

	    int startBar = comp.getBarNumber((*i)->getStartIndex());
	    int barCount = comp.getBarNumber((*i)->getEndIndex()) - startBar +1;

            int y = m_vHeader->sectionPos((*i)->getInstrument());
	    int x = m_hHeader->sectionPos(startBar);

            SegmentItem *newItem = m_segmentsCanvas->addPartItem(x, y, barCount);
            newItem->setSegment(*i);
        }
        
    }
}


void TrackEditor::addSegment(int instrument, int start,
                            unsigned int nbTimeSteps)
{
    if (!m_document) return; // sanity check

    Composition &comp = m_document->getComposition();

    Rosegarden::Segment* segment = new Rosegarden::Segment(start);
    segment->setInstrument(instrument);
    comp.addSegment(segment);
    segment->setDuration(nbTimeSteps);

    int startBar = comp.getBarNumber(start);
    int barCount = comp.getBarNumber(start + nbTimeSteps) - startBar + 1;

    int y = m_vHeader->sectionPos(instrument);
    int x = m_hHeader->sectionPos(startBar);

    SegmentItem *newItem = m_segmentsCanvas->addPartItem(x, y, barCount);
    newItem->setSegment(segment);

    emit needUpdate();
}


void TrackEditor::segmentOrderChanged(int section, int fromIdx, int toIdx)
{
    kdDebug(KDEBUG_AREA) << QString("TrackEditor::segmentOrderChanged(section : %1, from %2, to %3)")
        .arg(section).arg(fromIdx).arg(toIdx) << endl;

    updateSegmentOrder();
    emit needUpdate();
}


void
TrackEditor::addSegment(SegmentItem *p)
{
    // first find instrument for part, as it is used for indexing
    //
    int instrument = m_vHeader->sectionAt(static_cast<int>(p->y()));

    emit createNewSegment(p, instrument);

    kdDebug(KDEBUG_AREA) << QString("TrackEditor::addSegment() : segment instr is %1 at y=%2")
        .arg(instrument).arg(p->y())
                         << ", p = " << p << endl;

}


void TrackEditor::deleteSegment(Rosegarden::Segment *p)
{
    Composition& composition = m_document->getComposition();

    if (!composition.deleteSegment(p)) {
        KMessageBox::error(0, QString("TrackEditor::deleteSegment() : part %1 not found").arg(long(p), 0, 16));
        
        kdDebug(KDEBUG_AREA) << "TrackEditor::deleteSegment() : segment "
                             << p << " not found" << endl;
    }
}


void TrackEditor::updateSegmentDuration(SegmentItem *i)
{
    Composition& composition = m_document->getComposition();

    int startBar = m_hHeader->sectionAt(int(i->x()));
    int barCount = i->getItemNbBars();

    timeT startIndex = composition.getBarRange(startBar).first;
    timeT duration = composition.getBarRange(startBar + barCount).first -
	startIndex;

    kdDebug(KDEBUG_AREA) << "TrackEditor::updateSegmentDuration() : set duration to "
                         << duration << endl;

    i->getSegment()->setDuration(duration);
}


void TrackEditor::updateSegmentInstrumentAndStartIndex(SegmentItem *i)
{
    Composition& composition = m_document->getComposition();

    int instrument = m_vHeader->sectionAt(int(i->y()));
    int startBar = m_hHeader->sectionAt(int(i->x()));
    timeT startIndex = composition.getBarRange(startBar).first;

    kdDebug(KDEBUG_AREA) << "TrackEditor::updateSegmentInstrumentAndStartIndex() : set instrument to "
                         << instrument
                         << " - start Index to : " << startIndex << endl;

    i->setInstrument(instrument);
    i->getSegment()->setStartIndex(startIndex);
}


void TrackEditor::updateSegmentOrder()
{
    QCanvasItemList itemList = canvas()->canvas()->allItems();
    QCanvasItemList::Iterator it;

    for (it = itemList.begin(); it != itemList.end(); ++it) {
        QCanvasItem *item = *it;
        SegmentItem *segmentItem = dynamic_cast<SegmentItem*>(item);
        
        if (segmentItem) {
            segmentItem->setY(m_vHeader->sectionPos(segmentItem->getInstrument()));
        }
    }
}


void TrackEditor::clear()
{
    m_segmentsCanvas->clear();
}


void TrackEditor::setupHorizontalHeader()
{
    QString num;

    Composition &comp = m_document->getComposition();

    for (int i = 0; i < m_hHeader->count(); ++i) {

	std::pair<timeT, timeT> times = comp.getBarRange(i);

	//!!! ??? s'pose we should divide by some resolution-dependent thing
	int width = (times.second - times.first) / 7; // 384 -> 54
	if (width == 0) width = 10;
        m_hHeader->resizeSection(i, width);

	//!!! ??? I guess this is related to the bar resolution of the
	//trackscanvas?? ... and in any case bar numbers should
	//probably be 1-based
        m_hHeader->setLabel(i, num.setNum(i));
    }
}


// Move the position pointer
void
TrackEditor::setPointerPosition(int position)
{
//    kdDebug(KDEBUG_AREA) << "TrackEditor::setPointerPosition: time is " << position << endl;
    if (!m_pointer) return;

    Composition &comp = m_document->getComposition();

    int barNo = comp.getBarNumber(position);
    pair<timeT, timeT> times = comp.getBarStartAndEnd(position);

    int canvasPosition = m_hHeader->sectionPos(barNo);

//    kdDebug(KDEBUG_AREA) << "TrackEditor::setPointerPosition: canvas pos is " << canvasPosition << "; range is (" << times.first << "," << times.second << "); barno is " << barNo << "; section size is " << m_hHeader->sectionSize(barNo) << endl;

    if (times.first != times.second) {
	canvasPosition +=
	    m_hHeader->sectionSize(barNo) * (position - times.first) /
	    (times.second - times.first);
    }

//    kdDebug(KDEBUG_AREA) << "TrackEditor::setPointerPosition: canvas pos is now " << canvasPosition << endl;

    //??? I'm having to do the distance check because it seems to be
    //lagging otherwise -- not quite sure why

    double distance = (double)canvasPosition - m_pointer->x();
    if (distance < 0.0) distance = -distance;
    if (distance >= 1.0) {
	m_pointer->setX(canvasPosition);
	emit needUpdate();
    }
}


