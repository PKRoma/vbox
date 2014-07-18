/* $Id$ */
/** @file
 * VBox Qt GUI - UIIndicatorsPool class implementation.
 */

/*
 * Copyright (C) 2010-2014 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/* Qt includes: */
#include <QTimer>
#include <QPainter>
#include <QHBoxLayout>

/* GUI includes: */
#include "UIIndicatorsPool.h"
#include "QIWithRetranslateUI.h"
#include "UIExtraDataManager.h"
#include "UIMachineDefs.h"
#include "UIConverter.h"
#include "UIAnimationFramework.h"
#include "UISession.h"
#include "UIMedium.h"
#include "UIIconPool.h"
#include "UIHostComboEditor.h"
#include "QIStatusBarIndicator.h"
#include "VBoxGlobal.h"

/* COM includes: */
#include "CConsole.h"
#include "CMachine.h"
#include "CSystemProperties.h"
#include "CMachineDebugger.h"
#include "CGuest.h"
#include "CStorageController.h"
#include "CMediumAttachment.h"
#include "CNetworkAdapter.h"
#include "CUSBController.h"
#include "CUSBDeviceFilters.h"
#include "CUSBDevice.h"
#include "CSharedFolder.h"
#include "CVRDEServer.h"

/* Other VBox includes: */
#include <iprt/time.h>


/** QIStateStatusBarIndicator extension for Runtime UI. */
class UISessionStateStatusBarIndicator : public QIWithRetranslateUI<QIStateStatusBarIndicator>
{
    Q_OBJECT;

public:

    /** Constructor which remembers passed @a session object. */
    UISessionStateStatusBarIndicator(CSession &session) : m_session(session) {}

    /** Abstract update routine. */
    virtual void updateAppearance() = 0;

protected:

    /** Holds the session reference. */
    CSession &m_session;
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Hard-drive indicator. */
class UIIndicatorHardDrive : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, passes @a session to the UISessionStateStatusBarIndicator constructor. */
    UIIndicatorHardDrive(CSession &session)
        : UISessionStateStatusBarIndicator(session)
    {
        /* Assign state-icons: */
        setStateIcon(KDeviceActivity_Idle,    UIIconPool::iconSet(":/hd_16px.png"));
        setStateIcon(KDeviceActivity_Reading, UIIconPool::iconSet(":/hd_read_16px.png"));
        setStateIcon(KDeviceActivity_Writing, UIIconPool::iconSet(":/hd_write_16px.png"));
        setStateIcon(KDeviceActivity_Null,    UIIconPool::iconSet(":/hd_disabled_16px.png"));
        /* Translate finally: */
        retranslateUi();
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        updateAppearance();
    }

    /** Update routine. */
    void updateAppearance()
    {
        /* Get machine: */
        const CMachine machine = m_session.GetMachine();

        /* Prepare tool-tip: */
        QString strToolTip = QApplication::translate("UIIndicatorsPool",
                                                     "<p style='white-space:pre'><nobr>Indicates the activity "
                                                     "of the virtual hard disks:</nobr>%1</p>", "HDD tooltip");
        QString strFullData;

        /* Enumerate all the controllers: */
        bool fAttachmentsPresent = false;
        foreach (const CStorageController &controller, machine.GetStorageControllers())
        {
            QString strAttData;
            /* Enumerate all the attachments: */
            foreach (const CMediumAttachment &attachment, machine.GetMediumAttachmentsOfController(controller.GetName()))
            {
                /* Skip unrelated attachments: */
                if (attachment.GetType() != KDeviceType_HardDisk)
                    continue;
                /* Append attachment data: */
                strAttData += QString("<br>&nbsp;<nobr>%1:&nbsp;%2</nobr>")
                    .arg(gpConverter->toString(StorageSlot(controller.GetBus(), attachment.GetPort(), attachment.GetDevice())))
                    .arg(UIMedium(attachment.GetMedium(), UIMediumType_HardDisk).location());
                fAttachmentsPresent = true;
            }
            /* Append controller data: */
            if (!strAttData.isNull())
                strFullData += QString("<br><nobr><b>%1</b></nobr>").arg(controller.GetName()) + strAttData;
        }

        /* Hide indicator if there are no attachments: */
        if (!fAttachmentsPresent)
            hide();

        /* Update tool-tip: */
        setToolTip(strToolTip.arg(strFullData));
        /* Update indicator state: */
        setState(fAttachmentsPresent ? KDeviceActivity_Idle : KDeviceActivity_Null);
    }
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Optical-drive indicator. */
class UIIndicatorOpticalDisks : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, passes @a session to the UISessionStateStatusBarIndicator constructor. */
    UIIndicatorOpticalDisks(CSession &session)
        : UISessionStateStatusBarIndicator(session)
    {
        /* Assign state-icons: */
        setStateIcon(KDeviceActivity_Idle,    UIIconPool::iconSet(":/cd_16px.png"));
        setStateIcon(KDeviceActivity_Reading, UIIconPool::iconSet(":/cd_read_16px.png"));
        setStateIcon(KDeviceActivity_Writing, UIIconPool::iconSet(":/cd_write_16px.png"));
        setStateIcon(KDeviceActivity_Null,    UIIconPool::iconSet(":/cd_disabled_16px.png"));
        /* Translate finally: */
        retranslateUi();
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        updateAppearance();
    }

    /** Update routine. */
    void updateAppearance()
    {
        /* Get machine: */
        const CMachine machine = m_session.GetMachine();

        /* Prepare tool-tip: */
        QString strToolTip = QApplication::translate("UIIndicatorsPool",
                                                     "<p style='white-space:pre'><nobr>Indicates the activity "
                                                     "of the CD/DVD devices:</nobr>%1</p>", "CD/DVD tooltip");
        QString strFullData;

        /* Enumerate all the controllers: */
        bool fAttachmentsPresent = false;
        bool fAttachmentsMounted = false;
        foreach (const CStorageController &controller, machine.GetStorageControllers())
        {
            QString strAttData;
            /* Enumerate all the attachments: */
            foreach (const CMediumAttachment &attachment, machine.GetMediumAttachmentsOfController(controller.GetName()))
            {
                /* Skip unrelated attachments: */
                if (attachment.GetType() != KDeviceType_DVD)
                    continue;
                /* Append attachment data: */
                UIMedium vboxMedium(attachment.GetMedium(), UIMediumType_DVD);
                strAttData += QString("<br>&nbsp;<nobr>%1:&nbsp;%2</nobr>")
                    .arg(gpConverter->toString(StorageSlot(controller.GetBus(), attachment.GetPort(), attachment.GetDevice())))
                    .arg(vboxMedium.isNull() || vboxMedium.isHostDrive() ? vboxMedium.name() : vboxMedium.location());
                fAttachmentsPresent = true;
                if (!vboxMedium.isNull())
                    fAttachmentsMounted = true;
            }
            /* Append controller data: */
            if (!strAttData.isNull())
                strFullData += QString("<br><nobr><b>%1</b></nobr>").arg(controller.GetName()) + strAttData;
        }

        /* Hide indicator if there are no attachments: */
        if (!fAttachmentsPresent)
            hide();

        /* Update tool-tip: */
        setToolTip(strToolTip.arg(strFullData));
        /* Update indicator state: */
        setState(fAttachmentsMounted ? KDeviceActivity_Idle : KDeviceActivity_Null);
    }
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Floppy-drive indicator. */
class UIIndicatorFloppyDisks : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, passes @a session to the UISessionStateStatusBarIndicator constructor. */
    UIIndicatorFloppyDisks(CSession &session)
        : UISessionStateStatusBarIndicator(session)
    {
        /* Assign state-icons: */
        setStateIcon(KDeviceActivity_Idle,    UIIconPool::iconSet(":/fd_16px.png"));
        setStateIcon(KDeviceActivity_Reading, UIIconPool::iconSet(":/fd_read_16px.png"));
        setStateIcon(KDeviceActivity_Writing, UIIconPool::iconSet(":/fd_write_16px.png"));
        setStateIcon(KDeviceActivity_Null,    UIIconPool::iconSet(":/fd_disabled_16px.png"));
        /* Translate finally: */
        retranslateUi();
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        updateAppearance();
    }

    /** Update routine. */
    void updateAppearance()
    {
        /* Get machine: */
        const CMachine machine = m_session.GetMachine();

        /* Prepare tool-tip: */
        QString strToolTip = QApplication::translate("UIIndicatorsPool",
                                                     "<p style='white-space:pre'><nobr>Indicates the activity "
                                                     "of the floppy devices:</nobr>%1</p>", "FD tooltip");
        QString strFullData;

        /* Enumerate all the controllers: */
        bool fAttachmentsPresent = false;
        bool fAttachmentsMounted = false;
        foreach (const CStorageController &controller, machine.GetStorageControllers())
        {
            QString strAttData;
            /* Enumerate all the attachments: */
            foreach (const CMediumAttachment &attachment, machine.GetMediumAttachmentsOfController(controller.GetName()))
            {
                /* Skip unrelated attachments: */
                if (attachment.GetType() != KDeviceType_Floppy)
                    continue;
                /* Append attachment data: */
                UIMedium vboxMedium(attachment.GetMedium(), UIMediumType_Floppy);
                strAttData += QString("<br>&nbsp;<nobr>%1:&nbsp;%2</nobr>")
                    .arg(gpConverter->toString(StorageSlot(controller.GetBus(), attachment.GetPort(), attachment.GetDevice())))
                    .arg(vboxMedium.isNull() || vboxMedium.isHostDrive() ? vboxMedium.name() : vboxMedium.location());
                fAttachmentsPresent = true;
                if (!vboxMedium.isNull())
                    fAttachmentsMounted = true;
            }
            /* Append controller data: */
            if (!strAttData.isNull())
                strFullData += QString("<br><nobr><b>%1</b></nobr>").arg(controller.GetName()) + strAttData;
        }

        /* Hide indicator if there are no attachments: */
        if (!fAttachmentsPresent)
            hide();

        /* Update tool-tip: */
        setToolTip(strToolTip.arg(strFullData));
        /* Update indicator state: */
        setState(fAttachmentsMounted ? KDeviceActivity_Idle : KDeviceActivity_Null);
    }
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: USB indicator. */
class UIIndicatorUSB : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, passes @a session to the UISessionStateStatusBarIndicator constructor. */
    UIIndicatorUSB(CSession &session)
        : UISessionStateStatusBarIndicator(session)
    {
        /* Assign state-icons: */
        setStateIcon(KDeviceActivity_Idle,    UIIconPool::iconSet(":/usb_16px.png"));
        setStateIcon(KDeviceActivity_Reading, UIIconPool::iconSet(":/usb_read_16px.png"));
        setStateIcon(KDeviceActivity_Writing, UIIconPool::iconSet(":/usb_write_16px.png"));
        setStateIcon(KDeviceActivity_Null,    UIIconPool::iconSet(":/usb_disabled_16px.png"));
        /* Translate finally: */
        retranslateUi();
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        updateAppearance();
    }

    /** Update routine. */
    void updateAppearance()
    {
        /* Get machine: */
        const CMachine machine = m_session.GetMachine();

        /* Prepare tool-tip: */
        QString strToolTip = QApplication::translate("UIIndicatorsPool",
                                                     "<p style='white-space:pre'><nobr>Indicates the activity of "
                                                     "the attached USB devices:</nobr>%1</p>", "USB device tooltip");
        QString strFullData;

        /* Check whether there is at least one USB controller with an available proxy. */
        bool fUSBEnabled =    !machine.GetUSBDeviceFilters().isNull()
                           && !machine.GetUSBControllers().isEmpty()
                           && machine.GetUSBProxyAvailable();
        if (fUSBEnabled)
        {
            /* Enumerate all the USB devices: */
            const CConsole &console = m_session.GetConsole();
            foreach (const CUSBDevice &usbDevice, console.GetUSBDevices())
                strFullData += QString("<br><b><nobr>%1</nobr></b>").arg(vboxGlobal().details(usbDevice));
            /* Handle 'no-usb-devices' case: */
            if (strFullData.isNull())
                strFullData = QApplication::translate("UIIndicatorsPool", "<br><nobr><b>No USB devices attached</b></nobr>", "USB device tooltip");
        }

        /* Hide indicator if there are USB controllers: */
        if (!fUSBEnabled)
            hide();

        /* Update tool-tip: */
        setToolTip(strToolTip.arg(strFullData));
        /* Update indicator state: */
        setState(fUSBEnabled ? KDeviceActivity_Idle : KDeviceActivity_Null);
    }
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Network indicator. */
class UIIndicatorNetwork : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, passes @a session to the UISessionStateStatusBarIndicator constructor. */
    UIIndicatorNetwork(CSession &session)
        : UISessionStateStatusBarIndicator(session)
        , m_pTimerAutoUpdate(0)
    {
        /* Assign state-icons: */
        setStateIcon(KDeviceActivity_Idle,    UIIconPool::iconSet(":/nw_16px.png"));
        setStateIcon(KDeviceActivity_Reading, UIIconPool::iconSet(":/nw_read_16px.png"));
        setStateIcon(KDeviceActivity_Writing, UIIconPool::iconSet(":/nw_write_16px.png"));
        setStateIcon(KDeviceActivity_Null,    UIIconPool::iconSet(":/nw_disabled_16px.png"));
        /* Create auto-update timer: */
        m_pTimerAutoUpdate = new QTimer(this);
        if (m_pTimerAutoUpdate)
        {
            /* Configure auto-update timer: */
            connect(m_pTimerAutoUpdate, SIGNAL(timeout()), SLOT(sltUpdateNetworkIPs()));
            m_pTimerAutoUpdate->start(5000);
        }
        /* Translate finally: */
        retranslateUi();
    }

private slots:

    /** Updates network IP addresses. */
    void sltUpdateNetworkIPs()
    {
        updateAppearance();
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        updateAppearance();
    }

    /** Update routine. */
    void updateAppearance()
    {
        /* Get machine: */
        const CMachine machine = m_session.GetMachine();

        /* Prepare tool-tip: */
        QString strToolTip = QApplication::translate("UIIndicatorsPool",
                                                     "<p style='white-space:pre'><nobr>Indicates the activity of the "
                                                     "network interfaces:</nobr>%1</p>", "Network adapters tooltip");
        QString strFullData;

        /* Gather adapter properties: */
        RTTIMESPEC time;
        uint64_t u64Now = RTTimeSpecGetNano(RTTimeNow(&time));
        QString strFlags, strCount;
        LONG64 iTimestamp;
        machine.GetGuestProperty("/VirtualBox/GuestInfo/Net/Count", strCount, iTimestamp, strFlags);
        bool fPropsValid = (u64Now - iTimestamp < UINT64_C(60000000000)); /* timeout beacon */
        ulong uMaxCount = vboxGlobal().virtualBox().GetSystemProperties().GetMaxNetworkAdapters(machine.GetChipsetType());
        QStringList ipList, macList;
        if (fPropsValid)
        {
            const int cAdapters = RT_MIN(strCount.toInt(), (int)uMaxCount);
            for (int i = 0; i < cAdapters; ++i)
            {
                ipList << machine.GetGuestPropertyValue(QString("/VirtualBox/GuestInfo/Net/%1/V4/IP").arg(i));
                macList << machine.GetGuestPropertyValue(QString("/VirtualBox/GuestInfo/Net/%1/MAC").arg(i));
            }
        }

        /* Enumerate up to uMaxCount adapters: */
        bool fAdaptersPresent = false;
        for (ulong uSlot = 0; uSlot < uMaxCount; ++uSlot)
        {
            const CNetworkAdapter &adapter = machine.GetNetworkAdapter(uSlot);
            if (adapter.GetEnabled())
            {
                fAdaptersPresent = true;
                QString strGuestIp;
                if (fPropsValid)
                {
                    const QString strGuestMac = adapter.GetMACAddress();
                    int iIp = macList.indexOf(strGuestMac);
                    if (iIp >= 0)
                        strGuestIp = ipList[iIp];
                }
                /* Append adapter data: */
                strFullData += QApplication::translate("UIIndicatorsPool",
                    "<br><nobr><b>Adapter %1 (%2)</b>: %3 cable %4</nobr>", "Network adapters tooltip")
                    .arg(uSlot + 1)
                    .arg(gpConverter->toString(adapter.GetAttachmentType()))
                    .arg(strGuestIp.isEmpty() ? "" : "IP " + strGuestIp + ", ")
                    .arg(adapter.GetCableConnected() ?
                         QApplication::translate("UIIndicatorsPool", "connected", "Network adapters tooltip") :
                         QApplication::translate("UIIndicatorsPool", "disconnected", "Network adapters tooltip"));
            }
        }
        /* Handle 'no-adapters' case: */
        if (strFullData.isNull())
            strFullData = QApplication::translate("UIIndicatorsPool",
                              "<br><nobr><b>All network adapters are disabled</b></nobr>", "Network adapters tooltip");

        /* Hide indicator if there are no enabled adapters: */
        if (!fAdaptersPresent)
            hide();

        /* Update tool-tip: */
        setToolTip(strToolTip.arg(strFullData));
        /* Update indicator state: */
        setState(fAdaptersPresent ? KDeviceActivity_Idle : KDeviceActivity_Null);
    }

    /** Holds the auto-update timer instance. */
    QTimer *m_pTimerAutoUpdate;
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Shared-folders indicator. */
class UIIndicatorSharedFolders : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, passes @a session to the UISessionStateStatusBarIndicator constructor. */
    UIIndicatorSharedFolders(CSession &session)
        : UISessionStateStatusBarIndicator(session)
    {
        /* Assign state-icons: */
        setStateIcon(KDeviceActivity_Idle,    UIIconPool::iconSet(":/sf_16px.png"));
        setStateIcon(KDeviceActivity_Reading, UIIconPool::iconSet(":/sf_read_16px.png"));
        setStateIcon(KDeviceActivity_Writing, UIIconPool::iconSet(":/sf_write_16px.png"));
        setStateIcon(KDeviceActivity_Null,    UIIconPool::iconSet(":/sf_disabled_16px.png"));
        /* Translate finally: */
        retranslateUi();
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        updateAppearance();
    }

    /** Update routine. */
    void updateAppearance()
    {
        /* Get machine: */
        const CMachine machine = m_session.GetMachine();
        const CConsole console = m_session.GetConsole();

        /* Prepare tool-tip: */
        QString strToolTip = QApplication::translate("UIIndicatorsPool",
                                                     "<p style='white-space:pre'><nobr>Indicates the activity of "
                                                     "the machine's shared folders:</nobr>%1</p>", "Shared folders tooltip");
        QString strFullData;

        /* Enumerate all the folders: */
        QMap<QString, QString> sfs;
        foreach (const CSharedFolder &sf, machine.GetSharedFolders())
            sfs.insert(sf.GetName(), sf.GetHostPath());
        foreach (const CSharedFolder &sf, console.GetSharedFolders())
            sfs.insert(sf.GetName(), sf.GetHostPath());

        /* Append attachment data: */
        for (QMap<QString, QString>::const_iterator it = sfs.constBegin(); it != sfs.constEnd(); ++it)
        {
            /* Select slashes depending on the OS type: */
            if (VBoxGlobal::isDOSType(console.GetGuest().GetOSTypeId()))
                strFullData += QString("<br><nobr><b>\\\\vboxsvr\\%1&nbsp;</b></nobr><nobr>%2</nobr>")
                                       .arg(it.key(), it.value());
            else
                strFullData += QString("<br><nobr><b>%1&nbsp;</b></nobr><nobr>%2</nobr>")
                                       .arg(it.key(), it.value());
        }
        /* Handle 'no-folders' case: */
        if (sfs.isEmpty())
            strFullData = QApplication::translate("UIIndicatorsPool", "<br><nobr><b>No shared folders</b></nobr>", "Shared folders tooltip");

        /* Update tool-tip: */
        setToolTip(strToolTip.arg(strFullData));
        /* Update indicator state: */
        setState(!sfs.isEmpty() ? KDeviceActivity_Idle : KDeviceActivity_Null);
    }
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Video-capture indicator. */
class UIIndicatorVideoCapture : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;
    Q_PROPERTY(double rotationAngleStart READ rotationAngleStart);
    Q_PROPERTY(double rotationAngleFinal READ rotationAngleFinal);
    Q_PROPERTY(double rotationAngle READ rotationAngle WRITE setRotationAngle);

    /** Video-capture states. */
    enum UIIndicatorStateVideoCapture
    {
        UIIndicatorStateVideoCapture_Disabled = 0,
        UIIndicatorStateVideoCapture_Enabled  = 1
    };

public:

    /** Constructor, passes @a session to the UISessionStateStatusBarIndicator constructor. */
    UIIndicatorVideoCapture(CSession &session)
        : UISessionStateStatusBarIndicator(session)
        , m_pAnimation(0)
        , m_dRotationAngle(0)
    {
        /* Assign state-icons: */
        setStateIcon(UIIndicatorStateVideoCapture_Disabled, UIIconPool::iconSet(":/video_capture_16px.png"));
        setStateIcon(UIIndicatorStateVideoCapture_Enabled,  UIIconPool::iconSet(":/movie_reel_16px.png"));
        /* Create *enabled* state animation: */
        m_pAnimation = UIAnimationLoop::installAnimationLoop(this, "rotationAngle",
                                                                   "rotationAngleStart", "rotationAngleFinal",
                                                                   1000);
        /* Translate finally: */
        retranslateUi();
    }

private slots:

    /** Handles state change. */
    void setState(int iState)
    {
        /* Update animation state: */
        switch (iState)
        {
            case UIIndicatorStateVideoCapture_Disabled:
                m_pAnimation->stop();
                m_dRotationAngle = 0;
                break;
            case UIIndicatorStateVideoCapture_Enabled:
                m_pAnimation->start();
                break;
            default:
                break;
        }
        /* Call to base-class: */
        QIStateStatusBarIndicator::setState(iState);
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        updateAppearance();
    }

    /** Paint-event handler. */
    void paintEvent(QPaintEvent*)
    {
        /* Create new painter: */
        QPainter painter(this);
        /* Configure painter for *enabled* state: */
        if (state() == UIIndicatorStateVideoCapture_Enabled)
        {
            /* Configure painter for smooth animation: */
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            /* Shift rotation origin according pixmap center: */
            painter.translate(height() / 2, height() / 2);
            /* Rotate painter: */
            painter.rotate(rotationAngle());
            /* Unshift rotation origin according pixmap center: */
            painter.translate(- height() / 2, - height() / 2);
        }
        /* Draw contents: */
        drawContents(&painter);
    }

    /** Update routine. */
    void updateAppearance()
    {
        /* Get machine: */
        const CMachine machine = m_session.GetMachine();

        /* Prepare tool-tip: */
        QString strToolTip = QApplication::translate("UIIndicatorsPool", "<nobr>Indicates video capturing activity:</nobr><br>%1");
        switch (state())
        {
            case UIIndicatorStateVideoCapture_Disabled:
            {
                strToolTip = strToolTip.arg(QApplication::translate("UIIndicatorsPool", "<nobr><b>Video capture disabled</b></nobr>"));
                break;
            }
            case UIIndicatorStateVideoCapture_Enabled:
            {
                strToolTip = strToolTip.arg(QApplication::translate("UIIndicatorsPool", "<nobr><b>Video capture file:</b> %1</nobr>"));
                strToolTip = strToolTip.arg(machine.GetVideoCaptureFile());
                break;
            }
            default:
                break;
        }

        /* Update tool-tip: */
        setToolTip(strToolTip);
        /* Update indicator state: */
        setState(machine.GetVideoCaptureEnabled());
    }

    /** Returns rotation start angle. */
    double rotationAngleStart() const { return 0; }
    /** Returns rotation finish angle. */
    double rotationAngleFinal() const { return 360; }
    /** Returns current rotation angle. */
    double rotationAngle() const { return m_dRotationAngle; }
    /** Defines current rotation angle. */
    void setRotationAngle(double dRotationAngle) { m_dRotationAngle = dRotationAngle; update(); }

    /** Holds the rotation animation instance. */
    UIAnimationLoop *m_pAnimation;
    /** Holds current rotation angle. */
    double m_dRotationAngle;
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Features indicator. */
class UIIndicatorFeatures : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, passes @a session to the UISessionStateStatusBarIndicator constructor. */
    UIIndicatorFeatures(CSession &session)
        : UISessionStateStatusBarIndicator(session)
    {
        /* Assign state-icons: */
        setStateIcon(0, UIIconPool::iconSet(":/vtx_amdv_disabled_16px.png"));
        setStateIcon(1, UIIconPool::iconSet(":/vtx_amdv_16px.png"));
        /* Translate finally: */
        retranslateUi();
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        updateAppearance();
    }

    /** Update routine. */
    void updateAppearance()
    {
        /* Get console: */
        const CConsole console = m_session.GetConsole();
        if (console.isNull())
            return;

        /* Get debugger: */
        const CMachineDebugger debugger = console.GetDebugger();
        if (debugger.isNull())
            return;

        /* VT-x/AMD-V feature: */
        bool bVirtEnabled = debugger.GetHWVirtExEnabled();
        QString virtualization = bVirtEnabled ?
            VBoxGlobal::tr("Enabled", "details report (VT-x/AMD-V)") :
            VBoxGlobal::tr("Disabled", "details report (VT-x/AMD-V)");

        /* Nested Paging feature: */
        bool bNestEnabled = debugger.GetHWVirtExNestedPagingEnabled();
        QString nestedPaging = bNestEnabled ?
            VBoxGlobal::tr("Enabled", "details report (Nested Paging)") :
            VBoxGlobal::tr("Disabled", "details report (Nested Paging)");

        /* Unrestricted Execution feature: */
        bool bUXEnabled = debugger.GetHWVirtExUXEnabled();
        QString unrestrictExec = bUXEnabled ?
            VBoxGlobal::tr("Enabled", "details report (Unrestricted Execution)") :
            VBoxGlobal::tr("Disabled", "details report (Unrestricted Execution)");

        /* CPU Execution Cap feature: */
        QString strCPUExecCap = QString::number(console.GetMachine().GetCPUExecutionCap());

        /* Prepare tool-tip: */
        QString tip(QApplication::translate("UIIndicatorsPool",
                                            "Additional feature status:"
                                            "<br><nobr><b>%1:</b>&nbsp;%2</nobr>"
                                            "<br><nobr><b>%3:</b>&nbsp;%4</nobr>"
                                            "<br><nobr><b>%5:</b>&nbsp;%6</nobr>"
                                            "<br><nobr><b>%7:</b>&nbsp;%8%</nobr>",
                                            "Virtualization Stuff LED")
                    .arg(VBoxGlobal::tr("VT-x/AMD-V", "details report"), virtualization)
                    .arg(VBoxGlobal::tr("Nested Paging"), nestedPaging)
                    .arg(VBoxGlobal::tr("Unrestricted Execution"), unrestrictExec)
                    .arg(VBoxGlobal::tr("Execution Cap", "details report"), strCPUExecCap));

        /* CPU count: */
        int cpuCount = console.GetMachine().GetCPUCount();
        if (cpuCount > 1)
            tip += QApplication::translate("UIIndicatorsPool", "<br><nobr><b>%1:</b>&nbsp;%2</nobr>", "Virtualization Stuff LED")
                      .arg(VBoxGlobal::tr("Processor(s)", "details report")).arg(cpuCount);

        /* Update tool-tip: */
        setToolTip(tip);
        /* Update indicator state: */
        setState(bVirtEnabled);
    }
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Mouse indicator. */
class UIIndicatorMouse : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, using @a pSession for state-update routine. */
    UIIndicatorMouse(UISession *pSession)
        : UISessionStateStatusBarIndicator(pSession->session())
    {
        /* Assign state-icons: */
        setStateIcon(0, UIIconPool::iconSet(":/mouse_disabled_16px.png"));
        setStateIcon(1, UIIconPool::iconSet(":/mouse_16px.png"));
        setStateIcon(2, UIIconPool::iconSet(":/mouse_seamless_16px.png"));
        setStateIcon(3, UIIconPool::iconSet(":/mouse_can_seamless_16px.png"));
        setStateIcon(4, UIIconPool::iconSet(":/mouse_can_seamless_uncaptured_16px.png"));
        /* Configure connection: */
        connect(pSession, SIGNAL(sigMouseStateChange(int)), this, SLOT(setState(int)));
        setState(pSession->mouseState());
        /* Translate finally: */
        retranslateUi();
    }

private slots:

    /** Handles state change. */
    void setState(int iState)
    {
        if ((iState & UIMouseStateType_MouseAbsoluteDisabled) &&
            (iState & UIMouseStateType_MouseAbsolute) &&
            !(iState & UIMouseStateType_MouseCaptured))
        {
            QIStateStatusBarIndicator::setState(4);
        }
        else
        {
            QIStateStatusBarIndicator::setState(iState & (UIMouseStateType_MouseAbsolute | UIMouseStateType_MouseCaptured));
        }
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        setToolTip(QApplication::translate("UIIndicatorsPool",
                   "Indicates whether the host mouse pointer is captured by the guest OS:<br>"
                   "<nobr><img src=:/mouse_disabled_16px.png/>&nbsp;&nbsp;pointer is not captured</nobr><br>"
                   "<nobr><img src=:/mouse_16px.png/>&nbsp;&nbsp;pointer is captured</nobr><br>"
                   "<nobr><img src=:/mouse_seamless_16px.png/>&nbsp;&nbsp;mouse integration (MI) is On</nobr><br>"
                   "<nobr><img src=:/mouse_can_seamless_16px.png/>&nbsp;&nbsp;MI is Off, pointer is captured</nobr><br>"
                   "<nobr><img src=:/mouse_can_seamless_uncaptured_16px.png/>&nbsp;&nbsp;MI is Off, pointer is not captured</nobr><br>"
                   "Note that the mouse integration feature requires Guest Additions to be installed in the guest OS."));
    }

    /** Update routine. */
    void updateAppearance() {}
};

/** UISessionStateStatusBarIndicator extension for Runtime UI: Keyboard indicator. */
class UIIndicatorKeyboard : public UISessionStateStatusBarIndicator
{
    Q_OBJECT;

public:

    /** Constructor, using @a pSession for state-update routine. */
    UIIndicatorKeyboard(UISession *pSession)
        : UISessionStateStatusBarIndicator(pSession->session())
    {
        /* Assign state-icons: */
        setStateIcon(0, UIIconPool::iconSet(":/hostkey_16px.png"));
        setStateIcon(1, UIIconPool::iconSet(":/hostkey_captured_16px.png"));
        setStateIcon(2, UIIconPool::iconSet(":/hostkey_pressed_16px.png"));
        setStateIcon(3, UIIconPool::iconSet(":/hostkey_captured_pressed_16px.png"));
        /* Configure connection: */
        connect(pSession, SIGNAL(sigKeyboardStateChange(int)), this, SLOT(setState(int)));
        setState(pSession->keyboardState());
        /* Translate finally: */
        retranslateUi();
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        setToolTip(QApplication::translate("UIIndicatorsPool",
                   "Indicates whether the keyboard is captured by the guest OS "
                   "(<img src=:/hostkey_captured_16px.png/>) or not (<img src=:/hostkey_16px.png/>)."));
    }

    /** Update routine. */
    void updateAppearance() {}
};

/** QITextStatusBarIndicator extension for Runtime UI: Keyboard-extension indicator. */
class UIIndicatorKeyboardExtension : public QIWithRetranslateUI<QITextStatusBarIndicator>
{
    Q_OBJECT;

public:

    /** Constructor. */
    UIIndicatorKeyboardExtension()
    {
        /* Make sure host-combination label will be updated: */
        connect(&vboxGlobal().settings(), SIGNAL(propertyChanged(const char *, const char *)),
                this, SLOT(sltUpdateAppearance()));
        /* Translate finally: */
        retranslateUi();
    }

public slots:

    /** Update routine. */
    void sltUpdateAppearance()
    {
        setText(UIHostCombo::toReadableString(vboxGlobal().settings().hostCombo()));
    }

private:

    /** Retranslation routine. */
    void retranslateUi()
    {
        sltUpdateAppearance();
        setToolTip(QApplication::translate("UIMachineWindowNormal",
                                           "Shows the currently assigned Host key.<br>"
                                           "This key, when pressed alone, toggles the keyboard and mouse "
                                           "capture state. It can also be used in combination with other keys "
                                           "to quickly perform actions from the main menu."));
    }
};


UIIndicatorsPool::UIIndicatorsPool(UISession *pSession, QWidget *pParent /* = 0 */)
    : QWidget(pParent)
    , m_pSession(pSession)
    , m_session(m_pSession->session())
    , m_fEnabled(false)
    , m_pTimerAutoUpdate(0)
{
    /* Prepare: */
    prepare();
}

UIIndicatorsPool::~UIIndicatorsPool()
{
    /* Cleanup: */
    cleanup();
}

void UIIndicatorsPool::updateAppearance(IndicatorType indicatorType)
{
    /* Skip missed indicators: */
    if (!m_pool.contains(indicatorType))
        return;

    /* Get indicator: */
    QIStatusBarIndicator *pIndicator = m_pool.value(indicatorType);

    /* Assert indicators with NO appearance: */
    UISessionStateStatusBarIndicator *pSessionStateIndicator =
        qobject_cast<UISessionStateStatusBarIndicator*>(pIndicator);
    AssertPtrReturnVoid(pSessionStateIndicator);

    /* Update indicator appearance: */
    pSessionStateIndicator->updateAppearance();
}

void UIIndicatorsPool::setAutoUpdateIndicatorStates(bool fEnabled)
{
    /* Make sure auto-update timer exists: */
    AssertPtrReturnVoid(m_pTimerAutoUpdate);

    /* Start/stop timer: */
    if (fEnabled)
        m_pTimerAutoUpdate->start(100);
    else
        m_pTimerAutoUpdate->stop();
}

void UIIndicatorsPool::sltHandleConfigurationChange()
{
    /* Update pool: */
    updatePool();
}

void UIIndicatorsPool::sltAutoUpdateIndicatorStates()
{
    /* Update states for following indicators: */
    if (m_pool.contains(IndicatorType_HardDisks))
        updateIndicatorStateForDevice(m_pool.value(IndicatorType_HardDisks),     KDeviceType_HardDisk);
    if (m_pool.contains(IndicatorType_OpticalDisks))
        updateIndicatorStateForDevice(m_pool.value(IndicatorType_OpticalDisks),  KDeviceType_DVD);
    if (m_pool.contains(IndicatorType_FloppyDisks))
        updateIndicatorStateForDevice(m_pool.value(IndicatorType_FloppyDisks),   KDeviceType_Floppy);
    if (m_pool.contains(IndicatorType_USB))
        updateIndicatorStateForDevice(m_pool.value(IndicatorType_USB),           KDeviceType_USB);
    if (m_pool.contains(IndicatorType_Network))
        updateIndicatorStateForDevice(m_pool.value(IndicatorType_Network),       KDeviceType_Network);
    if (m_pool.contains(IndicatorType_SharedFolders))
        updateIndicatorStateForDevice(m_pool.value(IndicatorType_SharedFolders), KDeviceType_SharedFolder);
}

void UIIndicatorsPool::sltContextMenuRequest(QIStatusBarIndicator *pIndicator, QContextMenuEvent *pEvent)
{
    /* If that is one of pool indicators: */
    foreach (IndicatorType indicatorType, m_pool.keys())
        if (m_pool[indicatorType] == pIndicator)
        {
            /* Notify listener: */
            emit sigContextMenuRequest(indicatorType, pEvent->globalPos());
            return;
        }
}

void UIIndicatorsPool::prepare()
{
    /* Prepare connections: */
    prepareConnections();
    /* Prepare contents: */
    prepareContents();
    /* Prepare auto-update timer: */
    prepareUpdateTimer();
}

void UIIndicatorsPool::prepareConnections()
{
    /* Listen for the status-bar configuration changes: */
    connect(gEDataManager, SIGNAL(sigStatusBarConfigurationChange()),
            this, SLOT(sltHandleConfigurationChange()));
}

void UIIndicatorsPool::prepareContents()
{
    /* Create main-layout: */
    m_pMainLayout = new QHBoxLayout(this);
    AssertPtrReturnVoid(m_pMainLayout);
    {
        /* Configure main-layout: */
        m_pMainLayout->setContentsMargins(0, 0, 0, 0);
        m_pMainLayout->setSpacing(5);
        /* Update pool: */
        updatePool();
    }
}

void UIIndicatorsPool::prepareUpdateTimer()
{
    /* Create auto-update timer: */
    m_pTimerAutoUpdate = new QTimer(this);
    AssertPtrReturnVoid(m_pTimerAutoUpdate);
    {
        /* Configure auto-update timer: */
        connect(m_pTimerAutoUpdate, SIGNAL(timeout()),
                this, SLOT(sltAutoUpdateIndicatorStates()));
        setAutoUpdateIndicatorStates(true);
    }
}

void UIIndicatorsPool::updatePool()
{
    /* Acquire status-bar availability: */
    m_fEnabled = gEDataManager->statusBarEnabled(vboxGlobal().managedVMUuid());
    /* If status-bar is not enabled: */
    if (!m_fEnabled)
    {
        /* Remove all indicators: */
        while (!m_pool.isEmpty())
        {
            const IndicatorType firstType = m_pool.keys().first();
            delete m_pool.value(firstType);
            m_pool.remove(firstType);
        }
        /* And return: */
        return;
    }

    /* Acquire status-bar restrictions: */
    m_restrictions = gEDataManager->restrictedStatusBarIndicators(vboxGlobal().managedVMUuid());
    /* Remove restricted indicators: */
    foreach (const IndicatorType &indicatorType, m_restrictions)
    {
        if (m_pool.contains(indicatorType))
        {
            delete m_pool.value(indicatorType);
            m_pool.remove(indicatorType);
        }
    }

    /* Acquire status-bar order: */
    m_order = gEDataManager->statusBarIndicatorOrder(vboxGlobal().managedVMUuid());
    /* Make sure the order is complete taking restrictions into account: */
    for (int iType = IndicatorType_Invalid; iType < IndicatorType_Max; ++iType)
    {
        /* Get iterated type: */
        IndicatorType type = (IndicatorType)iType;
        /* Skip invalid type: */
        if (type == IndicatorType_Invalid)
            continue;
        /* Take restriction/presence into account: */
        bool fRestricted = m_restrictions.contains(type);
        bool fPresent = m_order.contains(type);
        if (fRestricted && fPresent)
            m_order.removeAll(type);
        else if (!fRestricted && !fPresent)
            m_order << type;
    }

    /* Add/Update allowed indicators: */
    foreach (const IndicatorType &indicatorType, m_order)
    {
        /* Indicator exists: */
        if (m_pool.contains(indicatorType))
        {
            /* Get indicator: */
            QIStatusBarIndicator *pIndicator = m_pool.value(indicatorType);
            /* Make sure it have valid position: */
            const int iWantedIndex = indicatorPosition(indicatorType);
            const int iActualIndex = m_pMainLayout->indexOf(pIndicator);
            if (iActualIndex != iWantedIndex)
            {
                /* Re-inject indicator into main-layout at proper position: */
                m_pMainLayout->removeWidget(pIndicator);
                m_pMainLayout->insertWidget(iWantedIndex, pIndicator);
            }
        }
        /* Indicator missed: */
        else
        {
            /* Create indicator: */
            switch (indicatorType)
            {
                case IndicatorType_HardDisks:         m_pool[indicatorType] = new UIIndicatorHardDrive(m_session);     break;
                case IndicatorType_OpticalDisks:      m_pool[indicatorType] = new UIIndicatorOpticalDisks(m_session);  break;
                case IndicatorType_FloppyDisks:       m_pool[indicatorType] = new UIIndicatorFloppyDisks(m_session);   break;
                case IndicatorType_USB:               m_pool[indicatorType] = new UIIndicatorUSB(m_session);           break;
                case IndicatorType_Network:           m_pool[indicatorType] = new UIIndicatorNetwork(m_session);       break;
                case IndicatorType_SharedFolders:     m_pool[indicatorType] = new UIIndicatorSharedFolders(m_session); break;
                case IndicatorType_VideoCapture:      m_pool[indicatorType] = new UIIndicatorVideoCapture(m_session);  break;
                case IndicatorType_Features:          m_pool[indicatorType] = new UIIndicatorFeatures(m_session);      break;
                case IndicatorType_Mouse:             m_pool[indicatorType] = new UIIndicatorMouse(m_pSession);        break;
                case IndicatorType_Keyboard:          m_pool[indicatorType] = new UIIndicatorKeyboard(m_pSession);     break;
                case IndicatorType_KeyboardExtension: m_pool[indicatorType] = new UIIndicatorKeyboardExtension;        break;
                default: break;
            }
            /* Configure indicator: */
            connect(m_pool.value(indicatorType), SIGNAL(sigContextMenuRequest(QIStatusBarIndicator*, QContextMenuEvent*)),
                    this, SLOT(sltContextMenuRequest(QIStatusBarIndicator*, QContextMenuEvent*)));
            /* Insert indicator into main-layout at proper position: */
            m_pMainLayout->insertWidget(indicatorPosition(indicatorType), m_pool.value(indicatorType));
        }
    }
}

void UIIndicatorsPool::cleanupUpdateTimer()
{
    /* Destroy auto-update timer: */
    AssertPtrReturnVoid(m_pTimerAutoUpdate);
    {
        m_pTimerAutoUpdate->stop();
        delete m_pTimerAutoUpdate;
        m_pTimerAutoUpdate = 0;
    }
}

void UIIndicatorsPool::cleanupContents()
{
    /* Cleanup indicators: */
    while (!m_pool.isEmpty())
    {
        const IndicatorType firstType = m_pool.keys().first();
        delete m_pool.value(firstType);
        m_pool.remove(firstType);
    }
}

void UIIndicatorsPool::cleanup()
{
    /* Cleanup auto-update timer: */
    cleanupUpdateTimer();
    /* Cleanup indicators: */
    cleanupContents();
}

int UIIndicatorsPool::indicatorPosition(IndicatorType indicatorType) const
{
    int iPosition = 0;
    foreach (const IndicatorType &iteratedIndicatorType, m_order)
        if (iteratedIndicatorType == indicatorType)
            return iPosition;
        else
            ++iPosition;
    return iPosition;
}

void UIIndicatorsPool::updateIndicatorStateForDevice(QIStatusBarIndicator *pIndicator, KDeviceType deviceType)
{
    /* Assert indicators with NO state: */
    QIStateStatusBarIndicator *pStateIndicator = qobject_cast<QIStateStatusBarIndicator*>(pIndicator);
    AssertPtrReturnVoid(pStateIndicator);

    /* Skip indicators with NULL state: */
    if (pStateIndicator->state() == KDeviceActivity_Null)
        return;

    /* Paused VM have all indicator states set to IDLE: */
    if (m_pSession->isPaused())
    {
        /* If current state differs from IDLE => set the IDLE one:  */
        if (pStateIndicator->state() != KDeviceActivity_Idle)
            pStateIndicator->setState(KDeviceActivity_Idle);
    }
    else
    {
        /* Get actual indicator state: */
        const int iState = m_session.GetConsole().GetDeviceActivity(deviceType);
        /* If curren state differs from actual => set the actual one: */
        if (pStateIndicator->state() != iState)
            pStateIndicator->setState(iState);
    }
}

#include "UIIndicatorsPool.moc"

