/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koDocument.h"
#include "KoDocumentIface.h"
#include "koDocumentInfoDlg.h"
#include "koDocumentInfo.h"
#include "koView.h"
#include <kapplication.h>
#include <dcopclient.h>
#include <kdcopactionproxy.h>
#include <kaction.h>
#include <kdebug.h>
#include <kdcoppropertyproxy.h>

//static
QCString KoDocumentIface::newIfaceName()
{
    static int s_docIFNumber = 0;
    QCString name; name.setNum( s_docIFNumber++ ); name.prepend("Document-");
    return name;
}

KoDocumentIface::KoDocumentIface( KoDocument * doc, const char * name )
    : DCOPObject( name ? QCString(name) : newIfaceName() )
{
  m_pDoc = doc;
  m_actionProxy = new KDCOPActionProxy( doc->actionCollection(), this );
}

KoDocumentIface::~KoDocumentIface()
{
    delete m_actionProxy;
}

void KoDocumentIface::openURL( QString url )
{
  m_pDoc->openURL( KURL( url ) );
}

QString KoDocumentIface::url()
{
  return m_pDoc->url().url();
}

bool KoDocumentIface::isModified()
{
  return m_pDoc->isModified();
}

int KoDocumentIface::viewCount()
{
  return m_pDoc->viewCount();
}

DCOPRef KoDocumentIface::view( int idx )
{
  QPtrList<KoView> views = m_pDoc->views();
  KoView *v = views.at( idx );
  if ( !v )
    return DCOPRef();

  DCOPObject *obj = v->dcopObject();

  if ( !obj )
    return DCOPRef();

  return DCOPRef( kapp->dcopClient()->appId(), obj->objId() );
}

DCOPRef KoDocumentIface::action( const QCString &name )
{
    return DCOPRef( kapp->dcopClient()->appId(), m_actionProxy->actionObjectId( name ) );
}

QCStringList KoDocumentIface::actions()
{
    QCStringList res;
    QValueList<KAction *> lst = m_actionProxy->actions();
    QValueList<KAction *>::ConstIterator it = lst.begin();
    QValueList<KAction *>::ConstIterator end = lst.end();
    for (; it != end; ++it )
        res.append( (*it)->name() );

    return res;
}

QMap<QCString,DCOPRef> KoDocumentIface::actionMap()
{
    return m_actionProxy->actionMap();
}

void KoDocumentIface::saveAs( const QString & url )
{
    m_pDoc->saveAs( KURL( url ) );
}

void KoDocumentIface::setOutputMimeType( const QCString & mimetype )
{
    m_pDoc->setOutputMimeType( mimetype );
}

QString KoDocumentIface::documentInfoAuthorName() const
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->fullName();
}

QString KoDocumentIface::documentInfoEmail() const
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->email();
}

QString KoDocumentIface::documentInfoCompanyName() const
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->company();
}

QString KoDocumentIface::documentInfoTelephone() const
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->telephone();

}
QString KoDocumentIface::documentInfoFax() const
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->fax();

}
QString KoDocumentIface::documentInfoCountry() const
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->country();

}
QString KoDocumentIface::documentInfoPostalCode() const
{
        KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->postalCode();

}
QString KoDocumentIface::documentInfoCity() const
{
        KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->city();

}
QString KoDocumentIface::documentInfoStreet() const
{
        KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return authorPage->street();

}

QString KoDocumentIface::documentInfoTitle() const
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    if ( !aboutPage )
    {
        kdWarning() << "'About' page not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return aboutPage->title();

}

QString KoDocumentIface::documentInfoAbstract() const
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    if ( !aboutPage )
    {
        kdWarning() << "'About' page not found in documentInfo !" << endl;
        return QString::null;
    }
    else
        return aboutPage->abstract();
}

void KoDocumentIface::setDocumentInfoAuthorName(const QString & text)
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setFullName(text);

}

void KoDocumentIface::setDocumentInfoEmail(const QString &text)
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setEmail(text);
}

void KoDocumentIface::setDocumentInfoCompanyName(const QString &text)
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setCompany(text);
}

void KoDocumentIface::setDocumentInfoTelephone(const QString &text)
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setTelephone(text);

}

void KoDocumentIface::setDocumentInfoFax(const QString &text)
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setFax(text);
}

void KoDocumentIface::setDocumentInfoCountry(const QString &text)
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setCountry(text);

}

void KoDocumentIface::setDocumentInfoTitle(const QString & text)
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    if ( !aboutPage )
    {
        kdWarning() << "'About' page not found in documentInfo !" << endl;
    }
    else
        aboutPage->setTitle(text);
}

void KoDocumentIface::setDocumentInfoPostalCode(const QString &text)
{
        KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setPostalCode(text);

}


void KoDocumentIface::setDocumentInfoCity(const QString & text)
{
        KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setCity(text);

}
void KoDocumentIface::setDocumentInfoStreet(const QString &text)
{
        KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
    {
        kdWarning() << "Author information not found in documentInfo !" << endl;
    }
    else
        authorPage->setStreet(text);

}


void KoDocumentIface::setDocumentInfoAbstract(const QString &text)
{
    KoDocumentInfo * info = m_pDoc->documentInfo();
    KoDocumentInfoAbout * aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    if ( !aboutPage )
    {
        kdWarning() << "'About' page not found in documentInfo !" << endl;
    }
    else
       aboutPage->setAbstract(text);
}

QCStringList KoDocumentIface::functionsDynamic()
{
    return DCOPObject::functionsDynamic() + KDCOPPropertyProxy::functions( m_pDoc );
}

bool KoDocumentIface::processDynamic( const QCString &fun, const QByteArray &data,
                                      QCString& replyType, QByteArray &replyData )
{
    if ( KDCOPPropertyProxy::isPropertyRequest( fun, m_pDoc ) )
        return KDCOPPropertyProxy::processPropertyRequest( fun, data, replyType, replyData, m_pDoc );

    return DCOPObject::processDynamic( fun, data, replyType, replyData );
}

