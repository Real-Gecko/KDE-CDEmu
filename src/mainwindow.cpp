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
#include <QSettings>

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <KStandardAction>
#include <QStatusBar>
#include <QUrl>
#include <KMessageBox>

static QString fileTypes("CD images (*.mds *.mdx *.b5t *.b6t *.ccd *.sub *.img *.cue *.bin *.toc *.cdi *.cif *.c2d *.iso *.nrg *.udf *.dmg *.cdr *.cso *.ecm *.gz *.gbi *.daa *.isz *.xz)");

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent),
      m_trayIcon(this)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    m_ui.setupUi(this);

    m_ui.historyList->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_ui.historyList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotShowHistoryContextMenu(QPoint)));

    QObject::connect(CDEmu::instance(), SIGNAL(deviceChanged(unsigned int)), this, SLOT(slotDeviceChanged(unsigned int)));
    QObject::connect(CDEmu::instance(), SIGNAL(daemonChanged(bool)), this, SLOT(slotDaemonChanged(bool)));
    QObject::connect(CDEmu::instance(), SIGNAL(deviceMappingReady(unsigned int)), this, SLOT(slotDeviceMappingReady(unsigned int)));

//    QObject::connect(m_ui.SelectFileButton, SIGNAL(clicked()), this, SLOT(slotSelectFile()));
//    QObject::connect(m_ui.MountButton, SIGNAL(clicked()), this, SLOT(slotMount()));
    QObject::connect(m_ui.unmountButton, SIGNAL(clicked()), this, SLOT(slotUnmount()));
    QObject::connect(m_ui.deviceList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(slotSelectAndMount()));
    QObject::connect(m_ui.historyList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotHistoryClicked()));
    QObject::connect(m_ui.addDevice, SIGNAL(clicked()), this, SLOT(slotAddDevice()));
    QObject::connect(m_ui.removeDevice, SIGNAL(clicked()), this, SLOT(slotRemoveDevice()));

    m_trayIcon.setIconByName("media-optical");
    m_trayIcon.setCategory(KStatusNotifierItem::ApplicationStatus);
    m_trayIcon.setStatus(KStatusNotifierItem::Active);
    m_trayIcon.setToolTip("media-optical", "KDE CDEmu Manager", "");

    QMenu *fileMenu = new QMenu(i18n("File"), this);
    menuBar()->addMenu(fileMenu);

    KStandardAction::quit(qApp, SLOT(quit()), (QObject*)actionCollection());

    m_statusLabel = new QLabel(this);
    statusBar()->insertWidget(0, m_statusLabel);
    slotDaemonChanged(CDEmu::instance()->isDaemonRunning());

    setupGUI(Save | Create);

    QSettings settings("kde.org", "kde_cdemu");
    int historySize = settings.beginReadArray("History");
    for(int i = 0; i < historySize; ++i) {
        settings.setArrayIndex(i);
        m_ui.historyList->addItem(settings.value("file").toString());
    }
    settings.endArray();

    show();
}


MainWindow::~MainWindow()
{
}


void MainWindow::slotDeviceChanged(unsigned int device)
{
    QString filename = CDEmu::instance()->filename(device);
    m_ui.deviceList->topLevelItem(device)->setText(1, filename);
}


void MainWindow::slotDaemonChanged(bool running)
{
    m_ui.deviceList->setEnabled(running);
//    m_ui.SelectImageBox->setEnabled(running);
//    m_ui.MountButton->setEnabled(running);
//    m_ui.UnmountButton->setEnabled(running);

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
//    QString file = QFileDialog::getOpenFileName(QUrl(), fileTypes, this, QString());
    QString file = QFileDialog::getOpenFileName();
//    m_ui.FileNameEdit->setText(file);
}


void MainWindow::slotMount()
{/*
    if (m_ui.FileNameEdit->text().isEmpty())
    {
        KMessageBox::information(this, i18n("Please select a file first."), i18n("Error"));
        return;
    }
*/
    int device = m_ui.deviceList->indexOfTopLevelItem(m_ui.deviceList->currentItem());
    if (device < 0)
    {
        KMessageBox::information(this, i18n("Please select a device first."), i18n("Error"));
        return;
    }

//    CDEmu::Result result = CDEmu::instance()->mount(m_ui.FileNameEdit->text(), (unsigned int)device);
//    if (result == CDEmu::Success) {}
//        m_ui.FileNameEdit->clear();
//    else
//        MessageBox::error(result);
}


void MainWindow::slotUnmount()
{
    int device = m_ui.deviceList->indexOfTopLevelItem(m_ui.deviceList->currentItem());
    if (device < 0)
    {
        KMessageBox::information(this, i18n("Please select a device first."), i18n("Error"));
        return;
    }

    CDEmu::Result result = CDEmu::instance()->unmount((unsigned int)device);
    if (result != CDEmu::Success)
        MessageBox::error(result);
}


void MainWindow::slotSelectAndMount()
{
    QString file = QFileDialog::getOpenFileName(this, i18n("Select image file"), QString(), fileTypes);
    if(file == "") return;

    int device = m_ui.deviceList->indexOfTopLevelItem(m_ui.deviceList->currentItem());
    if (device < 0)
    {
        KMessageBox::information(this, i18n("Please select a device first."), i18n("Error"));
        return;
    }

    CDEmu::Result result = CDEmu::instance()->mount(file, (unsigned int)device);
    if (!result == CDEmu::Success)
    {
        MessageBox::error(result);
        return;
    }

    QList<QListWidgetItem* > historyItems = m_ui.historyList->findItems(file, Qt::MatchRecursive | Qt::MatchExactly);

    if(historyItems.count() == 0) {
        m_ui.historyList->addItem(file);

        QSettings settings("kde.org", "kde_cdemu");
        settings.beginWriteArray("History");
        for(int i = 0; i < m_ui.historyList->count(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("file", m_ui.historyList->item(i)->text());
        }
        settings.endArray();
    }
}


void MainWindow::slotHistoryClicked()
{
    int device = m_ui.deviceList->indexOfTopLevelItem(m_ui.deviceList->currentItem());
    if (device < 0)
    {
        KMessageBox::information(this, i18n("Please select a device first."), i18n("Error"));
        return;
    }

    int index = m_ui.historyList->currentRow();
    QString file = m_ui.historyList->item(index)->text();

    CDEmu::Result result = CDEmu::instance()->mount(file, (unsigned int)device);
    if (!result == CDEmu::Success)
    {
        MessageBox::error(result);
        return;
    }
}


void MainWindow::slotShowHistoryContextMenu(QPoint pos) {
    QPoint globalPos = m_ui.historyList->mapToGlobal(pos);

    QMenu historyContextMenu;
    historyContextMenu.addAction("Remove from history",  this, SLOT(slotRemoveFromHistory()));
    historyContextMenu.exec(globalPos);
}


void MainWindow::slotRemoveFromHistory() {
    QSettings settings("kde.org", "kde_cdemu");
    settings.beginWriteArray("History");
    for(int i = 0; i < m_ui.historyList->count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("file", m_ui.historyList->item(i)->text());
    }
    settings.endArray();
}


void MainWindow::slotAddDevice() {
    CDEmu::Result result = CDEmu::instance()->addDevice();
    if (!result == CDEmu::Success) {
        MessageBox::error(result);
        return;
    }
    updateDeviceList();
}


void MainWindow::slotRemoveDevice() {
    CDEmu::Result result = CDEmu::instance()->removeDevice();
    if (!result == CDEmu::Success) {
        MessageBox::error(result);
        return;
    }
    updateDeviceList();
}


void MainWindow::slotDeviceMappingReady(unsigned int) {
//    KMessageBox::information(this, i18n("Device mapping ready.") + QString::number(device), i18n("Error"));
}


void MainWindow::updateDeviceList()
{
    m_ui.deviceList->clear();

    int deviceCount = CDEmu::instance()->deviceCount();
    for (int i = 0; i < deviceCount; ++i)
    {
        QStringList strings;
        strings << QString().setNum(i) << CDEmu::instance()->filename(i);

        QTreeWidgetItem *item = new QTreeWidgetItem(strings);
        m_ui.deviceList->addTopLevelItem(item);
    }
}
