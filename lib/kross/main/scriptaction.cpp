/***************************************************************************
 * scriptaction.cpp
 * This file is part of the KDE project
 * copyright (C) 2005 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "scriptaction.h"
#include "manager.h"
#include "scriptcontainer.h"
//#include "../api/object.h"
//#include "../api/list.h"
//#include "../api/interpreter.h"
//#include "../api/script.h"
//#include "mainmodule.h"

//#include <qfile.h>
#include <kstandarddirs.h>
#include <kdebug.h>

using namespace Kross::Api;

namespace Kross { namespace Api {

    /// @internal
    class ScriptActionPrivate
    {
        public:

            /**
            * The \a ScriptContainer the \a ScriptAction instance
            * uses internaly to evaluate the scripting code which
            * is hidden behind the action.
            */
            ScriptContainer::Ptr scriptcontainer;

    };

}}

ScriptAction::ScriptAction(const char* name, const QString& text)
    : KAction()
    , d( new ScriptActionPrivate() ) // initialize d-pointer class
{
    kdDebug() << QString("Kross::Api::ScriptAction::ScriptAction(const char*, const QString&) name='%1' text='%2'").arg(name).arg(text) << endl;

    setName( name );
    setText( text.isEmpty() ? name : text );

    d->scriptcontainer = Manager::scriptManager()->getScriptContainer(name);
}

ScriptAction::ScriptAction(const QDomElement& element)
    : KAction()
    , d( new ScriptActionPrivate() ) // initialize d-pointer class
{
    kdDebug() << "Kross::Api::ScriptAction::ScriptAction(const QDomElement&)" << endl;

    QString name = element.attribute("name");
    QString text = element.attribute("text");
    QString file = element.attribute("file");

    if(file.isEmpty()) {
        if(text.isEmpty())
            text = name;
    }
    else {
        if(name.isEmpty())
            name = file;
        if(text.isEmpty())
            text = file;
    }

    setText( text );
    setIcon( element.attribute("icon") );

    d->scriptcontainer = Manager::scriptManager()->getScriptContainer(name);

    QString interpreter = element.attribute("interpreter");
    if(! interpreter.isNull()) {
        setInterpreterName( interpreter );
    }

    if(file.isNull()) {
        setCode( element.text().stripWhiteSpace() );
    }
    else {
        QString res = element.attribute("resource");
        if(! res.isNull()) {
            QString f = KGlobal::dirs()->findResource(res.latin1(), file);
            if(f.isNull()) { // if we failed to find the resource just pass both to provide later a better errormessage.
                file = QString("%1://%2").arg(res).arg(file);
                setEnabled(false);
            }
            else // else everything seems to be fine.
                file = f;

        }
        setFile( file );
    }

    // connect signal
    connect(this, SIGNAL(activated()), this, SLOT(activate()));
}

ScriptAction::~ScriptAction()
{
    kdDebug() << QString("Kross::Api::ScriptAction::~ScriptAction() name='%1' text='%2'").arg(name()).arg(text()) << endl;

    // disconnect signal
    //disconnect(this, SIGNAL(activated()), this, SLOT(activate()));

    // We don't need the ScriptContainer any longer. Cause the 
    // ScriptContainer is a shared pointer we can't just delete it 
    // because if may still be used somewhere else. So, we just 
    // deligate cleanup to the ScriptContainer.
    d->scriptcontainer->finalize();

    delete d;
}

const QString& ScriptAction::getInterpreterName() const
{
    return d->scriptcontainer->getInterpreterName();
}

void ScriptAction::setInterpreterName(const QString& name)
{
    d->scriptcontainer->setInterpreterName(name);
}

const QString& ScriptAction::getCode() const
{
    return d->scriptcontainer->getCode();
}

void ScriptAction::setCode(const QString& code)
{
    d->scriptcontainer->setCode(code);
}

const QString& ScriptAction::getFile() const
{
    return d->scriptcontainer->getFile();
}

void ScriptAction::setFile(const QString& scriptfile)
{
    d->scriptcontainer->setFile(scriptfile);
}

ScriptContainer* ScriptAction::getScriptContainer() const
{
    return d->scriptcontainer;
}

void ScriptAction::activate()
{
    d->scriptcontainer->execute();
    if( d->scriptcontainer->hadException() ) {
        QString errormessage = d->scriptcontainer->getException()->getError();
        QString tracedetails = d->scriptcontainer->getException()->getTrace();
        emit failed(errormessage, tracedetails);
    }
    else {
        emit success();
    }
}

bool ScriptAction::activate(QString& errormessage, QString& tracedetails)
{
    d->scriptcontainer->execute();
    if( d->scriptcontainer->hadException() ) {
        errormessage = d->scriptcontainer->getException()->getError();
        tracedetails = d->scriptcontainer->getException()->getTrace();
        return false;
    }
    return true;
}

#include "scriptaction.moc"
