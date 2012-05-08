/* This file is part of the KDE project
 * Copyright (C) 2011 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef BIBLIOGRAPHYDB_H
#define BIBLIOGRAPHYDB_H

#include <QObject>

class QSqlDatabase;
class QSqlTableModel;
class QSqlError;
class QDir;

class BibliographyDb : public QObject
{
    Q_OBJECT
public:
    explicit BibliographyDb(QObject *parent = 0, QString path = QString(), QString dbName = QString());
    bool openDb();
    bool deleteDb();
    QSqlError lastError() const;
    // creates bibliography table in SQLITE database
    bool createTable();
    QSqlTableModel* tableModel();
private:
    QSqlTableModel *m_model;
    QSqlDatabase *m_db;
    QString m_dbName;
    QString m_fullPath;
};

#endif // BIBLIOGRAPHYDB_H
