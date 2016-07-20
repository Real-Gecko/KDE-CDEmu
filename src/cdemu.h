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

#ifndef CDEMU_H
#define CDEMU_H

#include <QString>
#include <QtDBus>

class CDEmu : public QObject
{
    Q_OBJECT

public:
    enum Result
    {
        Success,
        DeviceInUse,
        DeviceNotAvailable,
        NoDeviceFree,
        FileNotFound,
        DaemonNotRunning,
        UnknownError
    };

    static CDEmu* instance();

    bool isDaemonRunning() const;

    int deviceCount() const;
    int nextFreeDevice() const;

    bool isLoaded(unsigned int device) const;
    QString filename(unsigned int device) const;

    Result mount(QString filename, unsigned int device) const;
    Result unmount(unsigned int device) const;
    Result addDevice() const;
    Result removeDevice() const;

signals:
    void daemonChanged(bool running);
    void deviceChanged(unsigned int device);
    void deviceMappingReady(unsigned int device);

private:
    CDEmu();
    ~CDEmu();

private slots:
    void slotServiceRegistered(const QString &service);
    void slotServiceUnregistered(const QString &service);

    void slotDeviceChanged(int);
    void slotDeviceMappingReady(int);

private:
    QDBusServiceWatcher m_watcher;
};

#endif // CDEMU_H
