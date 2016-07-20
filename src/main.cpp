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

#include <KAboutData>
#include <KCmdLineArgs>
#include <KUniqueApplication>
#include <klocalizedstring.h>
//kdelibs4support
#include <k4aboutdata.h>

#include "kdecdemu.h"
#include "kdecdemuversion.h"

int main (int argc, char *argv[])
{
    KLocalizedString::setApplicationDomain("kde_cdemu");
    K4AboutData aboutData("kde_cdemu", 0, ki18n("KDE CDEmu Manager"), KDE_CDEMU_VERSION, ki18n("A KDE Frontend to CDEmu"), K4AboutData::License_GPL_V3,
                         ki18n("Copyright (c) 2009-2013 Marcel Hasler"), KLocalizedString(), 
                          "http://kde-apps.org/content/show.php?content=99752&forumpage=5");
    
    aboutData.addAuthor(ki18n("Marcel Hasler"), ki18n("Developer & Maintainer"), "mahasler@gmail.com");
    aboutData.setProgramIconName("media-optical");

    KLocalizedString::setApplicationDomain("kde_cdemu");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("mount <file>", ki18n("Mount an image"));
    options.add("unmount <deviceNumber>", ki18n("Unmount an image"));
    options.add("status", ki18n("Show information about devices"));
    KCmdLineArgs::addCmdLineOptions(options);
    KUniqueApplication::addCmdLineOptions();

    if (!KUniqueApplication::start())
        return 0;

    KDECDEmu app;
    return app.exec();
}
