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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KStatusNotifierItem>
#include <KXmlGuiWindow>
#include <QLabel>

//#include <QDBusConnection>

#include "ui_mainwindow.h"

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotDeviceChanged(unsigned int);
    void slotDaemonChanged(bool);

    void slotSelectFile();
    void slotMount();
    void slotUnmount();
    void slotSelectAndMount();
    void slotHistoryClicked();
    void slotShowHistoryContextMenu(QPoint);
    void slotRemoveFromHistory();
    void slotAddDevice();
    void slotRemoveDevice();
    void slotDeviceMappingReady(unsigned int);

private:
    void updateDeviceList();

private:
    QLabel *m_statusLabel;
    KStatusNotifierItem m_trayIcon;
    Ui::MainWindow m_ui;
};

#endif // MAINWINDOW_H
