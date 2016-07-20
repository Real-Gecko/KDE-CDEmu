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

#include <QApplication>
#include <KAboutData>
#include <QCommandLineParser>
#include <QSettings>
#include <KDBusService>
#include <QSessionManager>

#include "mainwindow.h"
#include "kdecdemuversion.h"

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto disableSessionManagement = [](QSessionManager &sm) {
        sm.setRestartHint(QSessionManager::RestartNever);
    };

    QObject::connect(&app, &QGuiApplication::commitDataRequest, disableSessionManagement);
    QObject::connect(&app, &QGuiApplication::saveStateRequest, disableSessionManagement);
    app.setQuitOnLastWindowClosed( false );

    KDBusService service(KDBusService::Unique);

    KLocalizedString::setApplicationDomain("kde_cdemu");
    KAboutData aboutData(
        QStringLiteral("kde_cdemu"),
        i18n("KDE CDEmu Manager"),
        QStringLiteral(KDE_CDEMU_VERSION),
        i18n("A KDE Frontend to CDEmu"),
        KAboutLicense::LGPL_V3,
        i18n("Copyright (c) 2009-2013 Marcel Hasler"),
        QStringLiteral(""),
        QStringLiteral("http://kde-apps.org/content/show.php?content=99752&forumpage=5")
    );
    
    aboutData.addAuthor(i18n("Marcel Hasler"), i18n("Developer & Maintainer"), QStringLiteral("mahasler@gmail.com"));
    KAboutData::setApplicationData(aboutData);

    KLocalizedString::setApplicationDomain("kde_cdemu");

    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("mount <file>"), i18n("Mount an image"));
    parser.addPositionalArgument(QStringLiteral("unmount <deviceNumber>"), i18n("Unmount an image"));
    parser.addPositionalArgument(QStringLiteral("status"), i18n("Show information about devices"));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

//    app.setWindowIcon(QIcon::fromTheme("media-optical"));
    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();
    return app.exec();
}
