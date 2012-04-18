/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A sequencer and musical notation editor.
    Copyright 2000-2012 the Rosegarden development team.
    See the AUTHORS file for more details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include <cstring>

#include "ControlBlock.h"

#include "base/AllocateChannels.h"
#include "base/Instrument.h"
#include "document/RosegardenDocument.h"
#include "gui/studio/StudioControl.h"

namespace Rosegarden
{

ControlBlock *
ControlBlock::getInstance()
{
    static ControlBlock *instance = 0;
    if (!instance) instance = new ControlBlock();
    return instance;
}

ControlBlock::ControlBlock() :
    m_doc(0),
    m_maxTrackId(0),
    m_solo(false),
    m_routing(true),
    m_thruFilter(0),
    m_recordFilter(0),
    m_selectedTrack(0)
{
    m_metronomeInfo.muted = true;
    m_metronomeInfo.instrumentId = 0;
    for (unsigned int i = 0; i < CONTROLBLOCK_MAX_NB_TRACKS; ++i) {
        m_trackInfo[i].muted = true;
        m_trackInfo[i].deleted = true;
        m_trackInfo[i].armed = true;
        m_trackInfo[i].selected = false;
        m_trackInfo[i].hasThruChannel = false;
        m_trackInfo[i].instrumentId = 0;
        // We don't set thruChannel or isThruChannelReady because they
        // mean nothing when hasThruChannel is false.
    }
    setSelectedTrack(0);
}

void
ControlBlock::setDocument(RosegardenDocument *doc)
{
    m_doc = doc;
    m_maxTrackId = m_doc->getComposition().getMaxTrackId();
    
    Composition& comp = m_doc->getComposition();

    for (Composition::trackiterator i = comp.getTracks().begin();
	 i != comp.getTracks().end(); ++i) {
        Track *track = i->second;
        if (!track) continue;
	updateTrackData(track);
    }

    setMetronomeMuted(!comp.usePlayMetronome());

    setThruFilter(m_doc->getStudio().getMIDIThruFilter());
    setRecordFilter(m_doc->getStudio().getMIDIRecordFilter());
}

void
ControlBlock::updateTrackData(Track* t)
{
    if (t) {
        setInstrumentForTrack(t->getId(), t->getInstrument());
        setTrackArmed(t->getId(), t->isArmed());
        setTrackMuted(t->getId(), t->isMuted());
        setTrackDeleted(t->getId(), false);
        setTrackChannelFilter(t->getId(), t->getMidiInputChannel());
        setTrackDeviceFilter(t->getId(), t->getMidiInputDevice());
        if (t->getId() > m_maxTrackId)
            m_maxTrackId = t->getId();
    }
}

void
ControlBlock::setInstrumentForTrack(TrackId trackId, InstrumentId instId)
{
    if (trackId >= CONTROLBLOCK_MAX_NB_TRACKS) { return; }
    TrackInfo &track = m_trackInfo[trackId];

    track.releaseThruChannel(m_doc->getStudio());
    track.instrumentId = instId;
}

InstrumentId 
ControlBlock::getInstrumentForTrack(TrackId trackId) const
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        return m_trackInfo[trackId].instrumentId;
    return 0;
}

void
ControlBlock::setTrackMuted(TrackId trackId, bool mute)
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        m_trackInfo[trackId].muted = mute;
}

bool ControlBlock::isTrackMuted(TrackId trackId) const
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        return m_trackInfo[trackId].muted;
    return true;
}

void
ControlBlock::setTrackArmed(TrackId trackId, bool armed)
{
    if (trackId >= CONTROLBLOCK_MAX_NB_TRACKS) { return; }

    TrackInfo &track = m_trackInfo[trackId];
    track.armed = armed;
    track.conform(m_doc->getStudio());
}

bool 
ControlBlock::isTrackArmed(TrackId trackId) const
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        return m_trackInfo[trackId].armed;
    return false;
}

void
ControlBlock::setTrackDeleted(TrackId trackId, bool deleted)
{
    if (trackId >= CONTROLBLOCK_MAX_NB_TRACKS) { return; }

    TrackInfo &track = m_trackInfo[trackId];
    track.deleted = deleted;
    track.conform(m_doc->getStudio());
}

bool 
ControlBlock::isTrackDeleted(TrackId trackId) const
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        return m_trackInfo[trackId].deleted;
    return true;
}

void
ControlBlock::setTrackChannelFilter(TrackId trackId, char channel)
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        m_trackInfo[trackId].channelFilter = channel;
}

char
ControlBlock::getTrackChannelFilter(TrackId trackId) const
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        return m_trackInfo[trackId].channelFilter;
    return -1;
}

void
ControlBlock::setTrackDeviceFilter(TrackId trackId, DeviceId device)
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        m_trackInfo[trackId].deviceFilter = device;
}

DeviceId 
ControlBlock::getTrackDeviceFilter(TrackId trackId) const
{
    if (trackId < CONTROLBLOCK_MAX_NB_TRACKS)
        return m_trackInfo[trackId].deviceFilter;
    return Device::ALL_DEVICES;
}

bool 
ControlBlock::isInstrumentMuted(InstrumentId instrumentId) const
{
    for (unsigned int i = 0; i <= m_maxTrackId; ++i) {
        if (m_trackInfo[i].instrumentId == instrumentId &&
                !m_trackInfo[i].deleted && !m_trackInfo[i].muted)
            return false;
    }
    return true;
}

bool 
ControlBlock::isInstrumentUnused(InstrumentId instrumentId) const
{
    for (unsigned int i = 0; i <= m_maxTrackId; ++i) {
        if (m_trackInfo[i].instrumentId == instrumentId &&
                !m_trackInfo[i].deleted)
            return false;
    }
    return true;
}

void
ControlBlock::
setSelectedTrack(TrackId track)
{
    // Undo the old selected track.  Safe even if it referred to the
    // same track or to no track.
    if (m_selectedTrack < CONTROLBLOCK_MAX_NB_TRACKS) {
        TrackInfo &oldTrack = m_trackInfo[m_selectedTrack];
        oldTrack.selected = false;
        oldTrack.conform(m_doc->getStudio());
    }

    // Set up the new selected track
    if (track < CONTROLBLOCK_MAX_NB_TRACKS) {
        TrackInfo &newTrack = m_trackInfo[m_selectedTrack];
        newTrack.selected = true;
        newTrack.conform(m_doc->getStudio());
    }
    // What's selected is recorded both here and in the trackinfo
    // objects.
    m_selectedTrack = track;
}

// Return the instrument id and channel number for the selected track,
// preparing the channel if needed.  If impossible, return an invalid
// instrument and channel.
// @author Tom Breton (Tehom)
InstrumentAndChannel
ControlBlock::
getInsAndChanForSelectedTrack(void) 
{
    if (!m_doc)
        { return InstrumentAndChannel(); }

    TrackId trackId = getSelectedTrack();
    
    if (trackId >= CONTROLBLOCK_MAX_NB_TRACKS)
        { return InstrumentAndChannel(); }

    TrackInfo &track = m_trackInfo[trackId];
    return track.getChannelAsReady(m_doc->getStudio());
}

// Return the instrument id and channel number for the given DeviceId
// and input Channel, preparing the channel if needed.  If impossible,
// return an invalid instrument and channel.
InstrumentAndChannel
ControlBlock::
getInsAndChanForEvent(unsigned int dev, 
                      unsigned int chan) 
{
    for (unsigned int i = 0; i <= m_maxTrackId; ++i) {
        TrackInfo &track = m_trackInfo[i];
        if (!track.deleted && track.armed) {
            if (((track.deviceFilter == Device::ALL_DEVICES) ||
		 (track.deviceFilter == dev)) &&
		((track.channelFilter == -1) ||
		 (track.channelFilter == int(chan)))) {
                return track.getChannelAsReady(m_doc->getStudio());
            }
        }
    }

    // There is no matching filter so use the selected track.
    return getInsAndChanForSelectedTrack();    
}

// Kick all tracks' thru-channels off channel and arrange to find new
// homes for them.  This is called by AllocateChannels when a fixed
// channel has commandeered the channel.
// @author Tom Breton (Tehom)
void
ControlBlock::
vacateThruChannel(int channel)
{
    for (unsigned int i = 0; i <= m_maxTrackId; ++i) {
        TrackInfo &track = m_trackInfo[i];
        if(track.hasThruChannel && (track.thruChannel == channel)) {
            // Setting this flag invalidates the channel as far as
            // track knows, and AllocateChannels already removed it.
            track.hasThruChannel = false;
            track.conform(m_doc->getStudio());
        }
    }
}

// React to an instrument having changed its program.
// @author Tom Breton (Tehom)
void
ControlBlock::
instrumentChangedProgram(InstrumentId instrumentId)
{
    for (unsigned int i = 0; i <= m_maxTrackId; ++i) {
        TrackInfo &track = m_trackInfo[i];
        if(track.hasThruChannel && (track.instrumentId == instrumentId)) {
            track.isThruChannelReady = false;
        }
    }
}

    /** TrackInfo members **/

// Make track info conformant to its situation.  In particular,
// acquire or release a channel for thru events to play on.
// @author Tom Breton (Tehom)
void
TrackInfo::
conform(Studio &studio)
{
    bool shouldHaveThru = (armed || selected) && !deleted;
    
    if (!hasThruChannel && shouldHaveThru)
        { allocateThruChannel(studio); }
    else if (hasThruChannel && !shouldHaveThru)
        { releaseThruChannel(studio); }
}

// Return the instrument id and channel number that this track plays on,
// preparing the channel if needed.  If impossible, return an invalid
// instrument and channel.
// @author Tom Breton (Tehom)
InstrumentAndChannel
TrackInfo::getChannelAsReady(Studio &studio)
{
    if (!hasThruChannel)
        { return InstrumentAndChannel(); }

    // If our channel might not have the right program, send it now.
    if (!isThruChannelReady) {
        Instrument *instrument =
            studio.getInstrumentById(instrumentId);
        assert(instrument);

        // Unreadiness should only occur with a Midi device.
        assert(instrument->getType() == Instrument::Midi);

        Device* device = instrument->getDevice();
        assert(device);

        // This is how MidiInstrument readies a fixed
        // channel.
        StudioControl::sendChannelSetup(instrument, thruChannel);
        isThruChannelReady = true;
    }

    return InstrumentAndChannel(instrumentId, thruChannel);    
}

// Allocate a channel for thru MIDI events to play on.
// @author Tom Breton (Tehom)
void
TrackInfo::allocateThruChannel(Studio &studio)
{
    Instrument *instrument =
        studio.getInstrumentById(instrumentId);
    assert(instrument);

    // We can't use a fixed channel for this because we're not
    // notified if it becomes auto.
    
    /// if (instrument->hasFixedChannel()) {
    /// thruChannel = instrument->getNaturalChannel();
    /// isThruChannelReady = true;
    /// return;
    ///     }

    Device* device = instrument->getDevice();
    assert(device);
    AllocateChannels *allocator = device->getAllocator();

    // Device is not a channel-managing device, so instrument's
    // natural channel is correct and requires no further setup.
    if (!allocator)
        {
            thruChannel = instrument->getNaturalChannel();
            isThruChannelReady = true;
            return;
        }

    // Get a suitable channel.
    thruChannel = allocator->allocateThruChannel();

    // Right now the channel is probably playing the wrong program.
    isThruChannelReady = false;
    hasThruChannel = true;
}
    
// Release the channel that thru MIDI events played on.
// @author Tom Breton (Tehom)
void
TrackInfo::releaseThruChannel(Studio &studio)
{
    if (!hasThruChannel) { return; }

    Instrument *instrument =
        studio.getInstrumentById(instrumentId);
    assert(instrument);

    // We don't use fixed channels, so we don't need to be careful
    // about releasing one.

    Device* device = instrument->getDevice();
    assert(device);
    AllocateChannels *allocator = device->getAllocator();

    // Device is a channel-managing device (Midi), so release the
    // channel.
    if (allocator)
        { allocator->releaseThruChannel(thruChannel); }
    
    thruChannel = -1;
    // Channel wants no setup if we somehow encounter it in this
    // state.
    isThruChannelReady = true;
    hasThruChannel = false;
}



}
