/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2008 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_ROSEGARDENTRANSPORTDIALOG_H_
#define _RG_ROSEGARDENTRANSPORTDIALOG_H_

#include <map>
#include <QDockWidget>
#include <QColor>
#include <QPixmap>
#include "base/Composition.h" // for tempoT
#include <QtCore>
#include <QDialog>

//class RosegardenTransport;
class QWidget;
class QTimer;
class QPushButton;
class QCloseEvent;
class QShortcut;
class QDialog;

class Ui_RosegardenTransport;

namespace Rosegarden
{

class TimeSignature;
class RealTime;
class MappedEvent;


class TransportDialog : public QDialog
{
    Q_OBJECT
public:
    TransportDialog( QWidget *parent = 0,
                     const char *name = 0,
                     //!!! perhaps not any of this:
                     Qt::WindowFlags flags = Qt::Dialog 
                     | Qt::WindowStaysOnTopHint
                     | Qt::WStyle_NormalBorder
//!!!???						| Qt::WA_DeleteOnClose
        );
    ~TransportDialog();

    enum TimeDisplayMode { RealMode, SMPTEMode, BarMode, BarMetronomeMode, FrameMode };

    std::string getCurrentModeAsString();
    TimeDisplayMode getCurrentMode() { return m_currentMode; }
    void setNewMode(const std::string& newModeAsString);
    void setNewMode(const TimeDisplayMode& newMode);
    bool isShowingTimeToEnd();
    bool isExpanded();

    void displayRealTime(const RealTime &rt);
    void displaySMPTETime(const RealTime &rt);
    void displayFrameTime(const RealTime &rt);
    void displayBarTime(int bar, int beat, int unit);

    void setTempo(const tempoT &tempo);
    void setTimeSignature(const TimeSignature &timeSig);

    void setSMPTEResolution(int framesPerSecond, int bitsPerFrame);
    void getSMPTEResolution(int &framesPerSecond, int &bitsPerFrame);

    // Called indirectly from the sequencer and from the GUI to
    // show incoming and outgoing MIDI events on the Transport
    //
    void setMidiInLabel(const MappedEvent *mE);
    void setMidiOutLabel(const MappedEvent *mE);

    // Return the shortcut object
    //
    QShortcut* getShortcuts() { return m_shortcuts; }

    // RosegardenTransport member accessors
    QPushButton* MetronomeButton()   { return m_transport->MetronomeButton; }
    QPushButton* SoloButton()        { return m_transport->SoloButton; }
    QPushButton* LoopButton()        { return m_transport->LoopButton; }
    QPushButton* PlayButton()        { return m_transport->PlayButton; }
    QPushButton* StopButton()        { return m_transport->StopButton; }
    QPushButton* FfwdButton()        { return m_transport->FfwdButton; }
    QPushButton* RewindButton()      { return m_transport->RewindButton; }
    QPushButton* RecordButton()      { return m_transport->RecordButton; }
    QPushButton* RewindEndButton()   { return m_transport->RewindEndButton; }
    QPushButton* FfwdEndButton()     { return m_transport->FfwdEndButton; }
    QPushButton* TimeDisplayButton() { return m_transport->TimeDisplayButton; }
    QPushButton* ToEndButton()       { return m_transport->ToEndButton; }

    virtual void show();
    virtual void hide();

protected:
    virtual void closeEvent(QCloseEvent * e);
    void computeSampleRate();
    void cycleThroughModes();
    void displayTime();

public slots:

    // These two slots are activated by QTimers
    //
    void slotClearMidiInLabel();
    void slotClearMidiOutLabel();

    // These just change the little labels that say what
    // mode we're in, nothing else
    //
    void slotChangeTimeDisplay();
    void slotChangeToEnd();

    void slotLoopButtonClicked();

    void slotPanelOpenButtonClicked();
    void slotPanelCloseButtonClicked();

    void slotEditTempo();
    void slotEditTimeSignature();
    void slotEditTime();

    void slotSetBackground(QColor);
    void slotResetBackground();

    void slotSetStartLoopingPointAtMarkerPos();
    void slotSetStopLoopingPointAtMarkerPos();

signals:
    void closed();

    // Set and unset the loop at the RosegardenGUIApp
    //
    void setLoop();
    void unsetLoop();
    void setLoopStartTime();
    void setLoopStopTime();

    void editTempo(QWidget *);
    void editTimeSignature(QWidget *);
    void editTransportTime(QWidget *);
    void scrollTempo(int);
    void panic();

private:
    void loadPixmaps();
    void resetFonts();
    void resetFont(QWidget *);
    void initModeMap();

    //--------------- Data members ---------------------------------

    Ui_RosegardenTransport* m_transport;

    std::map<int, QPixmap> m_lcdList;
    QPixmap m_lcdNegative;

    int m_lastTenHours;
    int m_lastUnitHours;
    int m_lastTenMinutes;
    int m_lastUnitMinutes;
    int m_lastTenSeconds;
    int m_lastUnitSeconds;
    int m_lastTenths;
    int m_lastHundreths;
    int m_lastThousandths;
    int m_lastTenThousandths;

    bool m_lastNegative;
    TimeDisplayMode m_lastMode;
    TimeDisplayMode m_currentMode;

    int m_tenHours;
    int m_unitHours;
    int m_tenMinutes;
    int m_unitMinutes;
    int m_tenSeconds;
    int m_unitSeconds;
    int m_tenths;
    int m_hundreths;
    int m_thousandths;
    int m_tenThousandths;

    tempoT m_tempo;
    int m_numerator;
    int m_denominator;

    int m_framesPerSecond;
    int m_bitsPerFrame;

    QTimer *m_midiInTimer;
    QTimer *m_midiOutTimer;
    QTimer *m_clearMetronomeTimer;

    QPixmap m_panelOpen;
    QPixmap m_panelClosed;

    void updateTimeDisplay();

    QShortcut *m_shortcuts;
    bool    m_isExpanded;

    bool m_haveOriginalBackground;
    bool m_isBackgroundSet;
    QColor m_originalBackground;

    int m_sampleRate;

    std::map<std::string, TimeDisplayMode> m_modeMap;
};

 



}

#endif
