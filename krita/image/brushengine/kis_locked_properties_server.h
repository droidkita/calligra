/*
 *  Copyright (c) 2014 Dmitry Kazakov <dimula73@gmail.com>
 *  Copyright (c) 2014 Mohit Goyal <mohit.bits2011@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KIS_LOCKED_PROPERTIES_SERVER_H
#define KIS_LOCKED_PROPERTIES_SERVER_H

#include"kis_locked_properties.h"
#include"kis_locked_properties_proxy.h"
#include"kis_properties_configuration.h"

class KisLockedPropertiesProxy;

class KRITAIMAGE_EXPORT KisLockedPropertiesServer: public QObject
{
public:
    KisLockedPropertiesServer();
    static KisLockedPropertiesServer* instance();
    KisLockedProperties* lockedProperties();
    void addToLockedProperties(KisPropertiesConfiguration *p);
    void removeFromLockedProperties(KisPropertiesConfiguration *p);
    void setPropertiesFromLocked(bool value);
    bool propertiesFromLocked();
    KisLockedPropertiesProxy* createLockedPropertiesProxy(const KisPropertiesConfiguration*);



private:
    KisLockedProperties* m_lockedProperties;
    bool m_propertiesFromLocked;
};

#endif // KIS_LOCKED_PROPERTIES_SERVER_H
