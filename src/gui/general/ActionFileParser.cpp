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

#include "ActionFileParser.h"

#include <iostream>

using std::cerr;
using std::endl;

namespace Rosegarden
{
   
ActionFileParser::ActionFileParser(QObject *actionOwner)
{
}

ActionFileParser::~ActionFileParser()
{
}

bool
ActionFileParser::load(QString actionRcFile)
{
    QFile f(actionRcFile); //!!! find it
    QXmlInputSource is(&f);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    return reader.parse(is);
}

bool
ActionFileParser::startDocument()
{
    return true;
}

bool
ActionFileParser::startElement(const QString& namespaceURI,
                               const QString& localName,
                               const QString& qName,
                               const QXmlAttributes& atts)
{
    QString name = qName.toLower();

    if (name == "menubar") {
        
        m_inMenuBar = true;
        
    } else if (name == "menu") {

        QString menuName = atts.value("name");
        if (menuName == "") {
            cerr << "WARNING: ActionFileParser::startElement: No menu name provided in menu element" << endl;
        }
        m_currentMenu = menuName;

    } else if (name == "toolbar") {

        QString toolbarName = atts.value("name");
        if (toolbarName == "") {
            cerr << "WARNING: ActionFileParser::startElement: No toolbar name provided in toolbar element" << endl;
        }
        m_currentToolbar = toolbarName;
        
    } else if (name == "text") {

        //!!! used to provide label for menu

    } else if (name == "action") {

        QString actionName = atts.value("name");
        if (actionName == "") {
            cerr << "WARNING: ActionFileParser::startElement: No action name provided in action element" << endl;
        }

        if (m_currentMenu == "" && m_currentToolbar == "" && m_currentState == "") {
            cerr << "WARNING: ActionFileParser::startElement: Action appears outside (valid) menu, toolbar or state element" << endl;
        }

        QString text = atts.value("text");
        QString icon = atts.value("icon");
        QString shortcut = atts.value("shortcut");
        QString group = atts.value("group");

        //!!! return values
        if (text != "") setActionText(actionName, text);
        if (icon != "") setActionIcon(actionName, icon);
        if (shortcut != "") setActionShortcut(actionName, shortcut);
        if (group != "") setActionGroup(actionName, group);
        
        //!!! can appear in menu, toolbar, state/enable, state/disable

        if (m_currentMenu != "") addActionToMenu(m_currentMenu, actionName);
        if (m_currentToolbar != "") addActionToToolbar(m_currentToolbar, actionName);

        if (m_currentState != "") {
            //!!!
        }

    } else if (name == "separator") {

        if (m_currentMenu != "") addSeparatorToMenu(m_currentMenu);
        if (m_currentToolbar != "") addSeparatorToToolbar(m_currentToolbar);

    } else if (name == "state") {

        QString stateName = atts.value("name");
        if (stateName == "") {
            cerr << "WARNING: ActionFileParser::startElement: No state name provided in state element" << endl;
        }
        m_currentState = stateName;

    } else if (name == "enable") {

        //!!! in state

    } else if (name == "disable") {

        //!!! in state
    }

    return true;
}

bool
ActionFileParser::endElement(const QString& namespaceURI,
                             const QString& localName,
                             const QString& qName)
{
    QString name = qName.toLower();

    if (name == "menubar") {
        
        m_inMenuBar = false;
        
    } else if (name == "menu") {

        m_currentMenu = "";

    } else if (name == "toolbar") {

        m_currentToolbar = "";

    } else if (name == "state") {
        
        m_currentState = "";
    }
        
    return true;
}

bool
ActionFileParser::characters(const QString &ch)
{
    return true;
}

bool
ActionFileParser::endDocument()
{
    return true;
}

bool
ActionFileParser::error(const QXmlParseException &exception)
{
    QString errorString =
	QString("ERROR: ActionFileParser: %1 at line %2, column %3")
	.arg(exception.message())
	.arg(exception.lineNumber())
	.arg(exception.columnNumber());
    std::cerr << errorString.toLocal8Bit().data() << std::endl;
    return QXmlDefaultHandler::error(exception);
}

bool
ActionFileParser::fatalError(const QXmlParseException &exception)
{
    QString errorString =
	QString("FATAL ERROR: ActionFileParser: %1 at line %2, column %3")
	.arg(exception.message())
	.arg(exception.lineNumber())
	.arg(exception.columnNumber());
    std::cerr << errorString.toLocal8Bit().data() << std::endl;
    return QXmlDefaultHandler::fatalError(exception);
}

bool
ActionFileParser::setActionText(QString actionName, QString text)
{
    if (actionName == "" || text == "") return false;
    return true;
}

bool
ActionFileParser::setActionIcon(QString actionName, QString icon)
{
    if (actionName == "" || icon == "") return false;
    return true;
}

bool
ActionFileParser::setActionShortcut(QString actionName, QString shortcut)
{
    if (actionName == "" || shortcut == "") return false;
    return true;
}

bool
ActionFileParser::setActionGroup(QString actionName, QString group)
{
    if (actionName == "" || group == "") return false;
    return true;
}

bool
ActionFileParser::setMenuText(QString name, QString text)
{
    if (name == "" || text == "") return false;
    return true;
}

bool
ActionFileParser::addMenuToMenu(QString parent, QString child)
{
    if (parent == "" || child == "") return false;
    return true;
}

bool
ActionFileParser::addActionToMenu(QString menuName, QString actionName)
{
    if (menuName == "" || actionName == "") return false;
    return true;
}

bool
ActionFileParser::addSeparatorToMenu(QString menuName)
{
    if (menuName == "") return false;
    return true;
}

bool
ActionFileParser::setToolbarText(QString name, QString text)
{
    if (name == "" || text == "") return false;
    return true;
}

bool
ActionFileParser::addActionToToolbar(QString toolbarName, QString actionName)
{
    if (toolbarName == "" || actionName == "") return false;
    return true;
}

bool
ActionFileParser::addSeparatorToToolbar(QString toolbarName)
{
    if (toolbarName == "") return false;
    return true;
}

bool
ActionFileParser::addState(QString name)
{
    if (name == "") return false;
    return true;
}

bool
ActionFileParser::enableActionInState(QString stateName, QString actionName)
{
    if (stateName == "" || actionName == "") return false;
    return true;
}

bool
ActionFileParser::disableActionInState(QString stateName, QString actionName)
{
    if (stateName == "" || actionName == "") return false;
    return true;
}

}

