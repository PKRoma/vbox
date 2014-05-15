/* $Id$ */
/** @file
 *
 * VBox frontends: Qt4 GUI ("VirtualBox"):
 * UIMachineSettingsPortForwardingDlg class implementation
 */

/*
 * Copyright (C) 2010-2013 Oracle Corporation
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
#include <QVBoxLayout>
#include <QPushButton>

/* GUI includes: */
#include "UIMachineSettingsPortForwardingDlg.h"
#include "UIIconPool.h"
#include "QIDialogButtonBox.h"

UIMachineSettingsPortForwardingDlg::UIMachineSettingsPortForwardingDlg(QWidget *pParent,
                                                                       const UIPortForwardingDataList &rules)
    : QIWithRetranslateUI<QIDialog>(pParent)
    , m_pTable(0)
    , m_pButtonBox(0)
{
    /* Set dialog icon: */
    setWindowIcon(UIIconPool::iconSetFull(":/nw_32px.png", ":/nw_16px.png"));

    /* Create layout: */
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    {
        /* Create table: */
        m_pTable = new UIPortForwardingTable(rules, false);
        /* Create button-box: */
        m_pButtonBox = new QIDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
        {
            /* Configure button-box: */
            connect(m_pButtonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()));
            connect(m_pButtonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
        }
        /* Add widgets into layout: */
        pMainLayout->addWidget(m_pTable);
        pMainLayout->addWidget(m_pButtonBox);
    }

    /* Retranslate dialog: */
    retranslateUi();
}

const UIPortForwardingDataList& UIMachineSettingsPortForwardingDlg::rules() const
{
    return m_pTable->rules();
}

void UIMachineSettingsPortForwardingDlg::accept()
{
    /* Validate table: */
    bool fPassed = m_pTable->validate();
    if (!fPassed)
        return;
    /* Call to base-class: */
    QIWithRetranslateUI<QIDialog>::accept();
}

void UIMachineSettingsPortForwardingDlg::reject()
{
    /* Discard table: */
    bool fPassed = m_pTable->discard();
    if (!fPassed)
        return;
    /* Call to base-class: */
    QIWithRetranslateUI<QIDialog>::reject();
}

void UIMachineSettingsPortForwardingDlg::retranslateUi()
{
    /* Set window title: */
    setWindowTitle(tr("Port Forwarding Rules"));
}

