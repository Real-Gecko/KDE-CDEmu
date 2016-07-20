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

#include <QFile>
#include <QtDBus>

CDEmu::CDEmu()
    : m_watcher(this)
{
    connect(&m_watcher, SIGNAL(serviceRegistered(QString)), this, SLOT(slotServiceRegistered(QString)));
    connect(&m_watcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(slotServiceUnregistered(QString)));

    m_watcher.setConnection(QDBusConnection::sessionBus());
    m_watcher.addWatchedService("net.sf.cdemu.CDEmuDaemon");

    QDBusConnection::sessionBus().interface()->startService("net.sf.cdemu.CDEmuDaemon");
    QDBusConnection::sessionBus().connect("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                          "net.sf.cdemu.CDEmuDaemon", "DeviceStatusChanged",
                                          this, SLOT(slotDeviceChanged(int)));
    QDBusConnection::sessionBus().connect("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                          "net.sf.cdemu.CDEmuDaemon", "DeviceMappingReady",
                                          this, SLOT(slotDeviceMappingReady(int)));
}


CDEmu::~CDEmu()
{
}


bool CDEmu::isDaemonRunning() const
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered("net.sf.cdemu.CDEmuDaemon");
}


int CDEmu::deviceCount() const
{
    QDBusMessage m = QDBusMessage::createMethodCall("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                                    "net.sf.cdemu.CDEmuDaemon", "GetNumberOfDevices");

    QDBusReply<int> reply = QDBusConnection::sessionBus().call(m);
    if (reply.isValid())
        return reply.value();
    else
        return -1;
}


int CDEmu::nextFreeDevice() const
{
    int count = deviceCount();
    for (int i = 0; i < count; ++i)
    {
        if (!isLoaded(i))
            return i;
    }

    return -1;
}


bool CDEmu::isLoaded(unsigned int device) const
{
    QDBusMessage m = QDBusMessage::createMethodCall("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                                    "net.sf.cdemu.CDEmuDaemon", "DeviceGetStatus");
    m << (int)device;

    QDBusMessage reply = QDBusConnection::sessionBus().call(m);
    if (reply.type() != QDBusMessage::ErrorMessage)
    {
        QList<QVariant> args = reply.arguments();
        return args[0].toBool();
    }

    return false;
}


QString CDEmu::filename(unsigned int device) const
{
    QDBusMessage m = QDBusMessage::createMethodCall("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                                    "net.sf.cdemu.CDEmuDaemon", "DeviceGetStatus");
    m << (int)device;

    QDBusMessage reply = QDBusConnection::sessionBus().call(m);
    if (reply.type() != QDBusMessage::ErrorMessage)
    {
        QList<QVariant> args = reply.arguments();
        bool loaded = args[0].toBool();

        if (loaded)
            return args[1].toList()[0].toString();
    }

    return QString::null;
}


CDEmu::Result CDEmu::mount(QString filename, unsigned int device) const
{
    if (!QFile::exists(filename))
        return FileNotFound;

    if (!isDaemonRunning())
        return DaemonNotRunning;

    if ((int)device >= deviceCount())
        return DeviceNotAvailable;

    if (isLoaded(device))
        return DeviceInUse;

    QStringList filenames;
    filenames << filename;

    QVariantMap parameters; // Unused for now

    QDBusMessage m = QDBusMessage::createMethodCall("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                                    "net.sf.cdemu.CDEmuDaemon", "DeviceLoad");
    m << (int)device << filenames << parameters;

    QDBusMessage reply = QDBusConnection::sessionBus().call(m);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return UnknownError;

    return Success;
}


CDEmu::Result CDEmu::unmount(unsigned int device) const
{
    if (!isDaemonRunning())
        return DaemonNotRunning;

    if ((int)device >= deviceCount())
        return DeviceNotAvailable;

    QDBusMessage m = QDBusMessage::createMethodCall("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                                    "net.sf.cdemu.CDEmuDaemon", "DeviceUnload");
    m << (int)device;

    QDBusMessage reply = QDBusConnection::sessionBus().call(m);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return UnknownError;

    return Success;
}


void CDEmu::slotServiceRegistered(const QString &service)
{
    if (service == "net.sf.cdemu.CDEmuDaemon")
        emit daemonChanged(true);
}


void CDEmu::slotServiceUnregistered(const QString &service)
{
    if (service == "net.sf.cdemu.CDEmuDaemon")
        emit daemonChanged(false);
}


void CDEmu::slotDeviceChanged(int device)
{
    // Shouldn't happen
    if (device < 0)
        return;

    emit deviceChanged((unsigned int)device);
}


void CDEmu::slotDeviceMappingReady(int device) {
    emit deviceMappingReady((unsigned int)device);
}


CDEmu::Result CDEmu::addDevice() const {
    if (!isDaemonRunning())
        return DaemonNotRunning;
    QDBusMessage m = QDBusMessage::createMethodCall("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                                    "net.sf.cdemu.CDEmuDaemon", "AddDevice");

    QDBusMessage reply = QDBusConnection::sessionBus().call(m);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return UnknownError;

    return Success;
}


CDEmu::Result CDEmu::removeDevice() const {
    if (!isDaemonRunning())
        return DaemonNotRunning;

    QDBusMessage m = QDBusMessage::createMethodCall("net.sf.cdemu.CDEmuDaemon", "/Daemon",
                                                    "net.sf.cdemu.CDEmuDaemon", "RemoveDevice");

    QDBusMessage reply = QDBusConnection::sessionBus().call(m);
    if (reply.type() == QDBusMessage::ErrorMessage)
        return UnknownError;

    return Success;
}


CDEmu* CDEmu::instance()
{
    static CDEmu emu;
    return &emu;
}
