/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
 
    This program is Copyright 2000-2007
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <richard.bown@ferventsoftware.com>
 
    The moral rights of Guillaume Laurent, Chris Cannam, and Richard
    Bown to claim authorship of this work have been asserted.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#include "ChangeStyleCommand.h"

#include "misc/Strings.h"
#include "base/NotationTypes.h"
#include "base/Selection.h"
#include "document/BasicSelectionCommand.h"
#include "gui/editors/notation/NotationProperties.h"
#include "gui/editors/notation/NoteStyleFactory.h"
#include "document/CommandRegistry.h"
#include <qstring.h>


namespace Rosegarden
{

void
ChangeStyleCommand::registerCommand(CommandRegistry *r)
{
    std::vector<NoteStyleName> styles =
        NoteStyleFactory::getAvailableStyleNames();

    for (std::vector<NoteStyleName>::iterator i = styles.begin();
         i != styles.end(); ++i) {

        QString styleQName(strtoqstr(*i));
        r->registerCommand(styleQName, 0, "", "style_" + styleQName,
                           new ArgumentAndSelectionCommandBuilder<ChangeStyleCommand>(),

                           i18n("Note &Style"), "note_style_actionmenu");
    }
}

NoteStyleName
ChangeStyleCommand::getArgument(QString actionName, CommandArgumentQuerier &)
{
    QString pfx = "style_";
    if (actionName.startsWith(pfx)) {
        QString remainder = actionName.right(actionName.length() - pfx.length());
        return qstrtostr(remainder);
    }
    return "";
}

QString
ChangeStyleCommand::getGlobalName(NoteStyleName style)
{
    return strtoqstr(style);
}

void
ChangeStyleCommand::modifySegment()
{
    EventSelection::eventcontainer::iterator i;

    for (i = m_selection->getSegmentEvents().begin();
         i != m_selection->getSegmentEvents().end(); ++i) {

        if ((*i)->isa(Note::EventType)) {
            if (m_style == NoteStyleFactory::DefaultStyle) {
                (*i)->unset(NotationProperties::NOTE_STYLE);
            } else {
                (*i)->set<String>(NotationProperties::NOTE_STYLE, m_style);
            }
        }
    }
}

}
