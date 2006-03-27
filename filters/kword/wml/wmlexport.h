/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __WMLEXPORT_H
#define __WMLEXPORT_H

#include <KoFilter.h>
#include <KoStore.h>
//Added by qt3to4:
#include <Q3CString>

class WMLExport : public KoFilter
{
    Q_OBJECT

  public:

    WMLExport(KoFilter *parent, const char *name, const QStringList&);
    WMLExport();

    virtual ~WMLExport() {}

    virtual KoFilter::ConversionStatus convert( const Q3CString& from, const Q3CString& to );
};

#endif // __WMLEXPORT_H










