/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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

#ifndef PARSEDPRESENTATION_H
#define PARSEDPRESENTATION_H

#include "generated/api.h"
#include "pole.h"
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QByteArray>

class ParsedPresentation
{
public:
    QByteArray currentUserStreamData;
    MSO::CurrentUserStream currentUserStream;
    QByteArray presentationData;
    MSO::PowerPointStructs presentation;
    QByteArray picturesData;
    MSO::PicturesStream pictures;
    // map persistObjectIds to stream offsets
    QMap<quint32, quint32> persistDirectory;
    MSO::DocumentContainer documentContainer;
    MSO::NotesContainer notesMaster;
    MSO::HandoutContainer handoutMaster;
    QVector<MSO::MasterOrSlideContainer> masters;
    QVector<MSO::SlideContainer> slides;
    QVector<MSO::NotesContainer> notes;

    ParsedPresentation() {
    }

    MSO::MasterOrSlideContainer getMaster(const MSO::SlideContainer& slide) const;
    bool parse(POLE::Storage& storage);
};

#endif // PARSEDPRESENTATION_H
