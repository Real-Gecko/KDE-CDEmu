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
#include "mainwindow.h"
#include "messagebox.h"

#include <QHeaderView>
#include <QLabel>
#include <QtDBus>

#include <KFileDialog>
#include <KMenu>
#include <KMenuBar>
#include <KMessageBox>
#include <KStandardAction>
#include <KStatusBar>
#include <KUrl>

static QString fileTypes("*.iso *.cue *.toc *.nrg *.mds *.daa *.cif *.b6t *.c2d *.cdi *.ccd");

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent),
      m_trayIcon(this)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    m_ui.setupUi(this);

    m_trayIcon.setIconByName("media-optical");
    m_trayIcon.setCategory(KStatusNotifierItem::ApplicationStatus);
    m_trayIcon.setStatus(KStatusNotifierItem::Active);
    m_trayIcon.setToolTip("media-optical", "KDE CDEmu Manager", "");

    QObject::connect(CDEmu::instance(), SIGNAL(deviceChanged(unsigned int)), this, SLOT(slotDeviceChanged(unsigned int)));
    QObject::connect(CDEmu::instance(), SIGNAL(daemonChanged(bool)), this, SLOT(slotDaemonChanged(bool)));

    QObject::connect(m_ui.SelectFileButton, SIGNAL(clicked()), this, SLOT(slotSelectFile()));
    QObject::connect(m_ui.MountButton, SIGNAL(clicked()), this, SLOT(slotMount()));
    QObject::connect(m_ui.UnmountButton, SIGNAL(clicked()), this, SLOT(slotUnmount()));

    KMenu *fileMenu = new KMenu(i18n("File"), this);
    menuBar()->addMenu(fileMenu);

    KStandardAction::quit(qApp, SLOT(quit()), (QObject*)actionCollection());

    m_statusLabel = new QLabel(this);
    statusBar()->insertWidget(0, m_statusLabel);
    slotDaemonChanged(CDEmu::instance()->isDaemonRunning());

    setupGUI(Save | Create);

    m_ui.DeviceList->header()->setStretchLastSection(false);

    m_ui.DeviceList->header()->setResizeMode(0, QHeaderView::Fixed);
    m_ui.DeviceList->header()->setResizeMode(1, QHeaderView::Stretch);

    int width = QFontMetrics(font()).width(i18n(m_ui.DeviceList->headerItem()->text(0).toAscii()) + "xxx");
    m_ui.DeviceList->header()->resizeSection(0, width);

    show();
}


MainWindow::~MainWindow()
{
}


void MainWindow::slotDeviceChanged(unsigned int device)
{
    QString filename = CDEmu::instance()->filename(device);
    m_ui.DeviceList->topLevelItem(device)->setText(1, filename);
}


void MainWindow::slotDaemonChanged(bool running)
{
    m_ui.DeviceList->setEnabled(running);
    m_ui.SelectImageBox->setEnabled(running);
    m_ui.MountButton->setEnabled(running);
    m_ui.UnmountButton->setEnabled(running);

    if (running)
    {
        updateDeviceList();
        m_statusLabel->setText(i18n("CDEmu daemon running."));
    }
    else
        m_statusLabel->setText(i18n("CDEmu daemon not running."));
}


void MainWindow::slotSelectFile()
{
    QString file = KFileDialog::getOpenFileName(KUrl(), fileTypes, this, QString());
    m_ui.FileNameEdit->setText(file);
}


void MainWindow::slotMount()
{
    if (m_ui.FileNameEdit->text().isEmpty())
    {
        KMessageBox::information(this, i18n("Please select a file first."), i18n("Error"));
        return;
    }

    int device = m_ui.DeviceList->indexOfTopLevelItem(m_ui.DeviceList->currentItem());
    if (device < 0)
    {
        KMessageBox::information(this, i18n("Please select a device first."), i18n("Error"));
        return;
    }

    CDEmu::Result result = CDEmu::instance()->mount(m_ui.FileNameEdit->text(), (unsigned int)device);
    if (result == CDEmu::Success)
        m_ui.FileNameEdit->clear();
    else
        MessageBox::error(result);
}


void MainWindow::slotUnmount()
{
    int device = m_ui.DeviceList->indexOfTopLevelItem(m_ui.DeviceList->currentItem());
    if (device < 0)
    {
        KMessageBox::information(this, i18n("Please select a device first."), i18n("Error"));
        return;
    }

    CDEmu::Result result = CDEmu::instance()->unmount((unsigned int)device);
    if (result != CDEmu::Success)
        MessageBox::error(result);
}


void MainWindow::updateDeviceList()
{
    m_ui.DeviceList->clear();

    int deviceCount = CDEmu::instance()->deviceCount();
    for (int i = 0; i < deviceCount; ++i)
    {
        QStringList strings;
        strings << QString().setNum(i) << CDEmu::instance()->filename(i);

        QTreeWidgetItem *item = new QTreeWidgetItem(strings);
        m_ui.DeviceList->addTopLevelItem(item);
    }
}
