/***************************************************************************
 * eventaction.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "eventaction.h"
//#include "object.h"
//#include "variant.h"
//#include "eventmanager.h"
//#include "../main/scriptcontainer.h"

//#include <qobject.h>
//#include <qsignal.h>
//#include <kaction.h>

//#include <qglobal.h>
//#include <qobjectdefs.h>
//#include <qmetaobject.h>

using namespace Kross::Api;

EventAction::EventAction(KAction* action)
    : Event(action->name())
    , m_action(action)
{
    /*
    m_eventmanager = new EventManager(scriptcontainer, this);
    addFunction("propertyNames", &QtObject::propertyNames,
        Kross::Api::ArgumentList(),
        ""
    );
    */
}

EventAction::~EventAction()
{
}

const QString EventAction::getClassName() const
{
    return "Kross::Api::EventAction";
}

const QString EventAction::getDescription() const
{
    return ""; //TODO
}

