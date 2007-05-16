/* This file is part of the KDE project
   Copyright 2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _TESTLOADANDSAVE_H_
#define _TESTLOADANDSAVE_H_

#include <QtCore/QObject>

class TestLoadAndSave : public QObject
{
    Q_OBJECT
private slots:
    void identifierElement_data();
    void identifierElement();
    void numberElement_data();
    void numberElement();
    void mathVariant_data();
    void mathVariant();
};

#endif // _TESTLOADANDSAVE_H_
