/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// This class is the base class for all the filters (excel97, powerpoint97,
// winword97)
// If you want to write a filter you'll have to derive your class from
// this one.

#ifndef FILTERBASE_H
#define FILTERBASE_H

#include <qobject.h>
#include <qdom.h>
#include <myfile.h>

class QCString;

// Attention: The nameOUT Strings are allocated with new[] in the
//            slots. Therefore you have to delete [] them!
//            If you call slotGetStream you have to delete [] the
//            stream.data prt after use!
class FilterBase : public QObject {

    Q_OBJECT

public:
    FilterBase();
    virtual ~FilterBase() {}

    // Manages the filtering process
    virtual const bool filter();

    // override this to return true if you want to return a plain QCString
    virtual bool plainString() const { return false; }
    // okay -- let's get the QDomDocument
    virtual const QDomDocument * const part() { return &m_part; }
    // or get the plain QCString ;)
    virtual QCString CString() const { return QCString(); }

signals:
    // See olefilter.h for information
    void signalSavePic(const QString &extension, unsigned int length, const unsigned char *data, const QString &key, QString &id);
    void signalPart(const char *nameIN, char **nameOUT);
    void signalGetStream(const int &handle, myFile &stream);
    // Note: might return wrong stream as names are NOT unique!!!
    // (searching only in current dir)
    void signalGetStream(const QString &name, myFile &stream);
    void sigProgress(int value);

protected:
    bool m_success;        // ok, the filtering process was successful
    bool m_ready;          // filtering took place, you may fetch the file now
    QDomDocument m_part;   // this represents the part (document)

private:
    // Don't copy or assign me...
    FilterBase(const FilterBase &);
    const FilterBase &operator=(const FilterBase &);
};
#endif // FILTERBASE_H
