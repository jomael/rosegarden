
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

#ifndef ROSEGARDENGUI_H
#define ROSEGARDENGUI_H
 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for Qt
#include <qstrlist.h>

// include files for KDE 
#include <kapp.h>
#include <kmainwindow.h>
#include <kaccel.h>

#include "rosegardendcop.h"
#include "rosegardenguiiface.h"

// the sequencer interface
//
#include <MappedEvent.h>

class KURL;
class KRecentFilesAction;
class KToggleAction;

// forward declaration of the RosegardenGUI classes
class RosegardenGUIDoc;
class RosegardenGUIView;

/**
  * The base class for RosegardenGUI application windows. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar. An instance of RosegardenGUIView creates your center view, which is connected
  * to the window's Doc object.
  * RosegardenGUIApp reimplements the methods that KTMainWindow provides for main window handling and supports
  * full session management as well as keyboard accelerator configuration by using KAccel.
  * @see KTMainWindow
  * @see KApplication
  * @see KConfig
  * @see KAccel
  *
  * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
  * @version KDevelop version 0.4 code generation
  */
class RosegardenGUIApp : public KMainWindow, virtual public RosegardenGUIIface
{
  Q_OBJECT

  friend class RosegardenGUIView;

public:

    /**
     * construtor of RosegardenGUIApp, calls all init functions to
     * create the application.
     * @see initMenuBar initToolBar
     */
    RosegardenGUIApp();

    virtual ~RosegardenGUIApp();

    /**
     * opens a file specified by commandline option
     */
    void openDocumentFile(const char *_cmdl=0);

    /**
     * returns a pointer to the current document connected to the
     * KTMainWindow instance and is used by * the View class to access
     * the document object's methods
     */	
    RosegardenGUIDoc *getDocument() const; 	

    /**
     * open a file
     */
    virtual void openFile(const QString& url);

    /**
     * Works like openFile but is able to open remote files
     */
    void openURL(const KURL& url);

    /**
     * imports a Rosegarden 2.1 file
     */
    virtual void importRG21File(const QString &url);

    /**
     * imports a MIDI file
     */
    virtual void importMIDIFile(const QString &url);

    /**
     * the Sequencer accesses this method via DCOP
     */
    const Rosegarden::MappedComposition&
            getSequencerSlice(const Rosegarden::timeT &sliceStart,
                              const Rosegarden::timeT &sliceEnd);

protected:

    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

    /**
     * save general Options like all bar positions and status as well
     * as the geometry and the recent file list to the configuration
     * file
     */ 	
    void saveOptions();

    /**
     * read general Options again and initialize all variables like
     * the recent file list
     */
    void readOptions();

    /**
     * create menus and toolbars
     */
    void setupActions();

    /**
     * sets up the statusbar for the main window by initialzing a
     * statuslabel.
     */
    void initStatusBar();

    /**
     * initializes the document object of the main window that is
     * connected to the view in initView().
     * @see initView();
     */
    void initDocument();

    /**
     * creates the centerwidget of the KTMainWindow instance and sets
     * it as the view
     */
    void initView();

    /**
     * queryClose is called by KTMainWindow on each closeEvent of a
     * window. Against the default implementation (only returns true),
     * this calles saveModified() on the document object to ask if the
     * document shall be saved if Modified; on cancel the closeEvent
     * is rejected.
     *
     * @see KTMainWindow#queryClose
     * @see KTMainWindow#closeEvent
     */
    virtual bool queryClose();

    /**
     * queryExit is called by KTMainWindow when the last window of the
     * application is going to be closed during the closeEvent().
     * Against the default implementation that just returns true, this
     * calls saveOptions() to save the settings of the last window's
     * properties.
     *
     * @see KTMainWindow#queryExit
     * @see KTMainWindow#closeEvent
     */
    virtual bool queryExit();

    /**
     * saves the window properties for each open window during session
     * end to the session config file, including saving the currently
     * opened file by a temporary filename provided by KApplication.
     *
     * @see KTMainWindow#saveProperties
     */
    virtual void saveProperties(KConfig *_cfg);

    /**
     * reads the session config file and restores the application's
     * state including the last opened files and documents by reading
     * the temporary files saved by saveProperties()
     *
     * @see KTMainWindow#readProperties
     */
    virtual void readProperties(KConfig *_cfg);

public slots:
    /**
     * open a new application window by creating a new instance of
     * RosegardenGUIApp
     */
    void fileNewWindow();

    /**
     * clears the document in the actual view to reuse it as the new
     * document
     */
    virtual void fileNew();

    /**
     * open a file and load it into the document
     */
    void fileOpen();

    /**
     * opens a file from the recent files menu
     */
    void fileOpenRecent(const KURL&);

    /**
     * save a document
     */
    virtual void fileSave();

    /**
     * save a document by a new filename
     */
    void fileSaveAs();

    /**
     * asks for saving if the file is modified, then closes the actual
     * file and window
     */
    virtual void fileClose();

    /**
     * print the actual file
     */
    void filePrint();

    /**
     * Let the user select a MIDI file for import
     */
    void importMIDI();

    /**
     * Let the user select a Rosegarden 2.1 file for import 
     */
    void importRG21();

    /**
     * closes all open windows by calling close() on each memberList
     * item until the list is empty, then quits the application.  If
     * queryClose() returns false because the user canceled the
     * saveModified() dialog, the closing breaks.
     */
    virtual void quit();
    
    /**
     * undo
     */
    void editUndo();

    /**
     * redo
     */
    void editRedo();
    
    /**
     * put the marked text/object into the clipboard and remove * it
     * from the document
     */
    void editCut();

    /**
     * put the marked text/object into the clipboard
     */
    void editCopy();

    /**
     * paste the clipboard into the document
     */
    void editPaste();

    /**
     * toggles the toolbar
     */
    void toggleToolBar();

    /**
     * toggles the tracks toolbar
     */
    void toggleTracksToolBar();

    /**
     * toggles the statusbar
     */
    void toggleStatusBar();

    /**
     * changes the statusbar contents for the standard label
     * permanently, used to indicate current actions.
     *
     * @param text the text that is displayed in the statusbar
     */
    void statusMsg(const QString &text);

    /**
     * changes the status message of the whole statusbar for two
     * seconds, then restores the last status. This is used to display
     * statusbar messages that give information about actions for
     * toolbar icons and menuentries.
     *
     * @param text the text that is displayed in the statusbar
     */
    void statusHelpMsg(const QString &text);

    /**
     * track eraser tool is selected
     */
    void eraseSelected();
    
    /**
     * track draw tool is selected
     */
    void drawSelected();
    
    /**
     * track move tool is selected
     */
    void moveSelected();

    /**
     * track resize tool is selected
     */
    void resizeSelected();

    /**
     * track resize tool is selected
     */
    void changeTimeResolution();

    /**
     * Set the song position pointer
     */
    void setPointerPosition(const int &position);

    /**
     * Transport controls
     */
    void play();
    void stop();
    void rewind();
    void fastforward();

    // Set the sequencer status - pass through DCOP as an int
    //
    void notifySequencerStatus(const int &status);


    // Convenience function for sending positional updates to
    // the sequencer if we're ffwding, rwding or just jumping
    // about on the Composition.
    //
    void sendSequencerJump(const Rosegarden::timeT &position);

private:

    /**
     * the configuration object of the application
     */
    KConfig* m_config;

    KRecentFilesAction* m_fileRecent;

    /**
     * view is the main widget which represents your working area. The
     * View class should handle all events of the view widget.  It is
     * kept empty so you can create your view according to your
     * application's needs by changing the view class.
     */
    RosegardenGUIView* m_view;

    /**
     * doc represents your actual document and is created only
     * once. It keeps information such as filename and does the
     * serialization of your files.
     */
    RosegardenGUIDoc* m_doc;

    /**
     * The default tool selected on startup
     */
    KAction* m_selectDefaultTool;

    /**
     * KAction pointers to enable/disable actions
     */
//     KAction* m_fileNewWindow;
    KAction* m_fileNew;
    KAction* m_fileOpen;
    KRecentFilesAction* m_fileOpenRecent;
    KAction* m_fileSave;
    KAction* m_fileSaveAs;
    KAction* m_fileClose;
    KAction* m_filePrint;
    KAction* m_fileQuit;
    KAction* m_editCut;
    KAction* m_editCopy;
    KAction* m_editPaste;
    KToggleAction* m_viewToolBar;
    KToggleAction* m_viewTracksToolBar;
    KToggleAction* m_viewStatusBar;

    /**
     * Transport (Playback and Recording) status
     */
    TransportStatus m_transportStatus;
 
};
 
#endif // ROSEGARDENGUI_H
