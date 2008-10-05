/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
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

#ifndef KWPAGEREMOVECOMMAND_H
#define KWPAGEREMOVECOMMAND_H

#include <QUndoCommand>

#include <KWPage.h>

class KWDocument;
class KoShapeMoveCommand;

/// The undo / redo command for removing a page in a kword document.
class KWPageRemoveCommand : public QUndoCommand
{
public:
    /**
     * The constuctor for a command to insert a new page.
     * @param document the document that gets a new page.
     * @param page the page to remove.
     * @param parent the parent for command macros
     */
    explicit KWPageRemoveCommand(KWDocument *document, KWPage page, QUndoCommand *parent = 0);
    ~KWPageRemoveCommand();

    /// (re)do the command
    void redo();
    /// revert the actions done in redo
    void undo();

private:
    KWDocument *m_document;
    KWPage::PageSide m_pageSide;
    KoPageLayout m_pageLayout;
    KoPageFormat::Orientation m_orientation;
    int m_pageNumber;
    QString m_masterPageName;
    KoText::Direction m_direction;
    QList<QUndoCommand*> m_childcommands;
};

#endif
