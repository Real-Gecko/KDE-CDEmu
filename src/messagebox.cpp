/***************************************************************************
 *   Copyright (C) 2009-2013 by Marcel Hasler                              *
 *   mahasler@gmail.com                                                    *
 *                                                                         *
 *   This file is part of KDE CDEmu Manager.                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "messagebox.h"

#include <QLocale>
#include <klocalizedstring.h>
#include <KMessageBox>

void MessageBox::error(CDEmu::Result result)
{
    QString text = QString::null;

    switch (result)
    {
        case CDEmu::Success:
            text = i18n("Everything OK.");
            break;

        case CDEmu::DeviceInUse:
            text = i18n("The selected virtual device is in use.");
            break;

        case CDEmu::DeviceNotAvailable:
            text = i18n("The selected virtual device is not available.");
            break;

        case CDEmu::NoDeviceFree:
            text = i18n("All virtual devices are in use.");
            break;

        case CDEmu::FileNotFound:
            text = i18n("The file doesn't exist.");
            break;

        case CDEmu::DaemonNotRunning:
            text = i18n("Unable to connect to the CDEmu daemon.");
            break;

        case CDEmu::UnknownError:
            text = i18n("An unknown error occured.");
            break;

        default:
            break;
    }

    if (!text.isNull())
        KMessageBox::information(0, text, i18n("Error"));
}
