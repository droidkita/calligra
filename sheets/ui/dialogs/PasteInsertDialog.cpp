/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "PasteInsertDialog.h"

#include <QApplication>
#include <QClipboard>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>

#include "core/Map.h"
#include "core/Sheet.h"
#include "../commands/PasteCommand.h"
#include "../Selection.h"

using namespace Calligra::Sheets;

PasteInsertDialog::PasteInsertDialog(QWidget* parent, Selection* selection)
        : KoDialog(parent)
{
    setCaption(i18n("Paste Inserting Cells"));
    setObjectName(QLatin1String("PasteInsertDialog"));
    setModal(true);
    setButtons(Ok | Cancel);
    m_selection = selection;
    rect = selection->lastRange();

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

    QGroupBox *grp = new QGroupBox(i18n("Insert"), page);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(rb1 = new QRadioButton(i18n("Move towards right")));
    vbox->addWidget(rb2 = new QRadioButton(i18n("Move towards bottom")));
    rb1->setChecked(true);
    grp->setLayout(vbox);
    lay1->addWidget(grp);

    connect(this, &KoDialog::okClicked, this, &PasteInsertDialog::slotOk);
}

void PasteInsertDialog::slotOk()
{
    QClipboard *clipboard = QApplication::clipboard();
    PasteCommand *const command = new PasteCommand();
    command->setSheet(m_selection->activeSheet());
    command->add(*m_selection);
    command->setMimeData(clipboard->mimeData());
    command->setSameApp (clipboard->ownsClipboard());
    if (rb1->isChecked()) {
        command->setInsertionMode(PasteCommand::ShiftCellsRight);
    } else if (rb2->isChecked()) {
        command->setInsertionMode(PasteCommand::ShiftCellsDown);
    }
    m_selection->activeSheet()->fullMap()->addCommand(command);
    accept();
}
