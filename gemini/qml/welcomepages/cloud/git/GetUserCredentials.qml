/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick 2.11
import QtQuick.Layouts 1.11 as QtLayouts
import QtQuick.Controls 2.11 as QtControls
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0
import Calligra.Gemini.Git 1.0
import "../../../components"

Kirigami.FormLayout {
    id: base;
    signal accepted();
    signal cancelled();
    property alias userForRemote: userName.text;
    property alias privateKeyFile: privateKey.text;
    property alias needsPrivateKeyPassphrase: needsPassphrase.checked;
    property alias publicKeyFile: publicKey.text;

    QtControls.TextField {
        id: userName;
        Kirigami.FormData.label: "Username";
    }

    QtControls.TextField {
        id: privateKey;
        Kirigami.FormData.label: "Private Key File";
        QtControls.Button {
            id: privateKeyBrowse;
            anchors {
                verticalCenter: privateKey.verticalCenter;
                right: parent.right;
            }
            text: "Browse...";
            onClicked: {
                var newFile = GitCheckoutCreator.getFile("Private Key File", "*", ".ssh");
                if(newFile !== "") {
                    privateKey.text = newFile;
                }
            }
        }
    }

    QtControls.TextField {
        id: publicKey;
        Kirigami.FormData.label: "Public Key File";
        QtControls.Button {
            id: publicKeyBrowse;
            anchors {
                verticalCenter: publicKey.verticalCenter;
                right: parent.right;
            }
            text: "Browse...";
            onClicked: {
                var newFile = GitCheckoutCreator.getFile("Public Key File", "*.pub", ".ssh");
                if(newFile !== "") {
                    publicKey.text = newFile;
                }
            }
        }
    }

    QtControls.CheckBox {
        id: needsPassphrase;
        Kirigami.FormData.label: "Does the private key require a password to unlock it?";
    }

    Kirigami.Separator {
    }

    QtControls.Button {
        id: acceptButton;
        text: "Accept";
        onClicked: base.accepted();
    }
}

