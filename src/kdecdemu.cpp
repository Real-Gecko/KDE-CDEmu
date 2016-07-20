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

#include "cdemu.h"
#include "kdecdemu.h"
#include "messagebox.h"

#include <KCmdLineArgs>
#include <KMessageBox>
#include <KProcess>

#include <QTextStream>
QTextStream cout(stdout, QIODevice::WriteOnly);

KDECDEmu::KDECDEmu()
    : KUniqueApplication("kde_cdemu"),
      m_mainWindow(0)
{
}


KDECDEmu::~KDECDEmu()
{
}


int KDECDEmu::newInstance()
{
    CDEmu *cdemu = CDEmu::instance();
    if (cdemu->isDaemonRunning())
    {
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->isSet("mount"))
        {
            int device = cdemu->nextFreeDevice();
            if (device < 0)
                MessageBox::error(CDEmu::NoDeviceFree);
            else
            {
                CDEmu::Result result = cdemu->mount(QDir().absoluteFilePath(args->getOption("mount")), device);
                if (result != CDEmu::Success)
                    MessageBox::error(result);
            }
        }
        else if (args->isSet("unmount"))
        {
            CDEmu::Result result = cdemu->unmount(args->getOption("unmount").toUInt());
            if (result != CDEmu::Success)
                MessageBox::error(result);
        }
        else if (args->isSet("status"))
        {
            int deviceCount = cdemu->deviceCount();
            if (deviceCount < 0)
                MessageBox::error(CDEmu::UnknownError);
            else
            {
                cout << "Device" << "\t\t" << "Loaded" << "\t\t" << "File" << endl;
                for (int i = 0; i < deviceCount; ++i)
                {
                    if (cdemu->isLoaded(i))
                        cout << i << "\t\t" << "Yes" << "\t\t" << CDEmu::instance()->filename(i) << endl;
                    else
                        cout << i << "\t\t" << "No" << "\t\t" << "None" << endl;
                }
            }
        }
        else
        {
            if (!m_mainWindow)
                m_mainWindow = new MainWindow();
        }

        args->clear();
    }
    else
        MessageBox::error(CDEmu::DaemonNotRunning);

    if (!m_mainWindow)
        quit();

    return 0;
}
