/* This file is part of the KDE project
 * Copyright (C) 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "PropertyContainer.h"

PropertyContainer::PropertyContainer(QString name, QObject* parent)
    : m_name(name)
    , QObject(parent)
{
}

PropertyContainer::~PropertyContainer()
{
}

void PropertyContainer::writeProperty(QString name, QVariant value)
{
    setProperty(name.toAscii(), value);
}

QVariant PropertyContainer::readProperty(QString name)
{
    return property(name.toAscii());
}

QString PropertyContainer::name()
{
    return m_name;
}
