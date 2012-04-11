/* $Id$ */
/** @file
 *
 * VBox frontends: Qt4 GUI ("VirtualBox"):
 * UIWizardNewVMPageBasic3 class implementation
 */

/*
 * Copyright (C) 2006-2012 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/* Global includes: */
#include <QIntValidator>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QGroupBox>
#include <QLabel>

/* Local includes: */
#include "UIWizardNewVMPageBasic3.h"
#include "UIWizardNewVM.h"
#include "COMDefs.h"
#include "VBoxGlobal.h"
#include "QIRichTextLabel.h"
#include "VBoxGuestRAMSlider.h"
#include "QILineEdit.h"

UIWizardNewVMPageBasic3::UIWizardNewVMPageBasic3()
{
    /* Create widget: */
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
        m_pLabel1 = new QIRichTextLabel(this);
        m_pLabel2 = new QIRichTextLabel(this);
        m_pMemoryCnt = new QGroupBox(this);
            m_pMemoryCnt->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            QGridLayout *pMemoryLayout = new QGridLayout(m_pMemoryCnt);
                m_pRamSlider = new VBoxGuestRAMSlider(m_pMemoryCnt);
                    m_pRamSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                    m_pRamSlider->setOrientation(Qt::Horizontal);
                    m_pRamSlider->setTickPosition(QSlider::TicksBelow);
                m_pRamEditor = new QILineEdit(m_pMemoryCnt);
                    m_pRamEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                    m_pRamEditor->setFixedWidthByText("88888");
                    m_pRamEditor->setAlignment(Qt::AlignRight);
                    m_pRamEditor->setValidator(new QIntValidator(m_pRamSlider->minRAM(), m_pRamSlider->maxRAM(), this));
                m_pRamUnits = new QLabel(m_pMemoryCnt);
                    m_pRamUnits->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
                m_pRamMin = new QLabel(m_pMemoryCnt);
                    m_pRamMin->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
                QSpacerItem *m_pRamSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding);
                m_pRamMax = new QLabel(m_pMemoryCnt);
                    m_pRamMax->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            pMemoryLayout->addWidget(m_pRamSlider, 0, 0, 1, 3);
            pMemoryLayout->addWidget(m_pRamEditor, 0, 3);
            pMemoryLayout->addWidget(m_pRamUnits, 0, 4);
            pMemoryLayout->addWidget(m_pRamMin, 1, 0);
            pMemoryLayout->addItem(m_pRamSpacer, 1, 1);
            pMemoryLayout->addWidget(m_pRamMax, 1, 2);
    pMainLayout->addWidget(m_pLabel1);
    pMainLayout->addWidget(m_pLabel2);
    pMainLayout->addWidget(m_pMemoryCnt);
    pMainLayout->addStretch();

    /* Setup connections: */
    connect(m_pRamSlider, SIGNAL(valueChanged(int)), this, SLOT(ramSliderValueChanged(int)));
    connect(m_pRamEditor, SIGNAL(textChanged(const QString&)), this, SLOT(ramEditorTextChanged(const QString&)));

    /* Initialize connections: */
    ramSliderValueChanged(m_pRamSlider->value());

    /* Register field: */
    registerField("ram*", m_pRamSlider, "value", SIGNAL(valueChanged(int)));
}

void UIWizardNewVMPageBasic3::ramSliderValueChanged(int iValue)
{
    /* Update 'ram' field editor connected to slider: */
    m_pRamEditor->setText(QString::number(iValue));
}

void UIWizardNewVMPageBasic3::ramEditorTextChanged(const QString &strText)
{
    /* Update 'ram' field slider connected to editor: */
    m_pRamSlider->setValue(strText.toInt());
}

void UIWizardNewVMPageBasic3::retranslateUi()
{
    /* Translate page: */
    setTitle(UIWizardNewVM::tr("Memory"));

    /* Translate widgets: */
    m_pLabel1->setText(UIWizardNewVM::tr("<p>Select the amount of base memory (RAM) in megabytes to be allocated to the virtual machine.</p>"));
    QString strRecommendedRAM = field("type").value<CGuestOSType>().isNull() ?
                                QString() : QString::number(field("type").value<CGuestOSType>().GetRecommendedRAM());
    m_pLabel2->setText(UIWizardNewVM::tr("The recommended base memory size is <b>%1</b> MB.").arg(strRecommendedRAM));
    m_pMemoryCnt->setTitle(UIWizardNewVM::tr("Base &Memory Size"));
    m_pRamUnits->setText(VBoxGlobal::tr("MB", "size suffix MBytes=1024 KBytes"));
    m_pRamMin->setText(QString("%1 %2").arg(m_pRamSlider->minRAM()).arg(VBoxGlobal::tr("MB", "size suffix MBytes=1024 KBytes")));
    m_pRamMax->setText(QString("%1 %2").arg(m_pRamSlider->maxRAM()).arg(VBoxGlobal::tr("MB", "size suffix MBytes=1024 KBytes")));
}

void UIWizardNewVMPageBasic3::initializePage()
{
    /* Translate page: */
    retranslateUi();

    /* Get recommended 'ram' field value: */
    CGuestOSType type = field("type").value<CGuestOSType>();
    ramSliderValueChanged(type.GetRecommendedRAM());

    /* 'Ram' field should have focus initially: */
    m_pRamSlider->setFocus();
}

bool UIWizardNewVMPageBasic3::isComplete() const
{
    /* Check what 'ram' field value feats the bounds: */
    return m_pRamSlider->value() >= qMax(1, (int)m_pRamSlider->minRAM()) &&
           m_pRamSlider->value() <= (int)m_pRamSlider->maxRAM();
}

