/* $Id$ */
/** @file
 *
 * VBox frontends: Qt4 GUI ("VirtualBox"):
 * VBoxGLSettingsGeneral class implementation
 */

/*
 * Copyright (C) 2006-2008 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#include "VBoxGLSettingsGeneral.h"
#include "VBoxGlobal.h"

#include <QDir>

VBoxGLSettingsGeneral::VBoxGLSettingsGeneral()
{
    /* Apply UI decorations */
    Ui::VBoxGLSettingsGeneral::setupUi (this);

#ifndef VBOX_GUI_WITH_SYSTRAY
    mCbCheckTrayIcon->hide();
#endif /* VBOX_GUI_WITH_SYSTRAY */
#ifndef QT_MAC_USE_COCOA
    mCbCheckPresentationMode->hide();
#endif /* QT_MAC_USE_COCOA */
    if (   mCbCheckTrayIcon->isHidden()
        && mCbCheckPresentationMode->isHidden())
        mLnSeparator2->hide();

    mPsHardDisk->setHomeDir (vboxGlobal().virtualBox().GetHomeFolder());
    mPsMach->setHomeDir (vboxGlobal().virtualBox().GetHomeFolder());
    mPsVRDP->setHomeDir (vboxGlobal().virtualBox().GetHomeFolder());
    mPsVRDP->setMode (VBoxFilePathSelectorWidget::Mode_File_Open);

    /* Applying language settings */
    retranslateUi();
}

void VBoxGLSettingsGeneral::getFrom (const CSystemProperties &aProps,
                                     const VBoxGlobalSettings &aGs)
{
    mPsHardDisk->setPath (aProps.GetDefaultHardDiskFolder());
    mPsMach->setPath (aProps.GetDefaultMachineFolder());
    mPsVRDP->setPath (aProps.GetRemoteDisplayAuthLibrary());
    mCbCheckTrayIcon->setChecked (aGs.trayIconEnabled());
#ifdef Q_WS_MAC
    mCbCheckPresentationMode->setChecked (aGs.presentationModeEnabled());
#endif /* Q_WS_MAC */
}

void VBoxGLSettingsGeneral::putBackTo (CSystemProperties &aProps,
                                       VBoxGlobalSettings &aGs)
{
    if (mPsHardDisk->isModified())
        aProps.SetDefaultHardDiskFolder (mPsHardDisk->path());
    if (aProps.isOk() && mPsMach->isModified())
        aProps.SetDefaultMachineFolder (mPsMach->path());
    if (aProps.isOk() && mPsVRDP->isModified())
        aProps.SetRemoteDisplayAuthLibrary (mPsVRDP->path());
    aGs.setTrayIconEnabled (mCbCheckTrayIcon->isChecked());
#ifdef Q_WS_MAC
    aGs.setPresentationModeEnabled (mCbCheckPresentationMode->isChecked());
#endif /* Q_WS_MAC */
}

void VBoxGLSettingsGeneral::setOrderAfter (QWidget *aWidget)
{
    setTabOrder (aWidget, mPsHardDisk);
    setTabOrder (mPsHardDisk, mPsMach);
    setTabOrder (mPsMach, mPsVRDP);
    setTabOrder (mPsVRDP, mCbCheckTrayIcon);
}

void VBoxGLSettingsGeneral::retranslateUi()
{
    /* Translate uic generated strings */
    Ui::VBoxGLSettingsGeneral::retranslateUi (this);

    mPsHardDisk->setWhatsThis (tr ("Displays the path to the default hard disk "
                                   "folder. This folder is used, if not explicitly "
                                   "specified otherwise, when adding existing or "
                                   "creating new virtual hard disks."));
    mPsMach->setWhatsThis (tr ("Displays the path to the default virtual "
                               "machine folder. This folder is used, if not "
                               "explicitly specified otherwise, when creating "
                               "new virtual machines."));
    mPsVRDP->setWhatsThis (tr ("Displays the path to the library that "
                               "provides authentication for Remote Display "
                               "(VRDP) clients."));
}

