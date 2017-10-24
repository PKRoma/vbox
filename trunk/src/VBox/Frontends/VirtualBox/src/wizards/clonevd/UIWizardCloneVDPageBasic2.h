/* $Id$ */
/** @file
 * VBox Qt GUI - UIWizardCloneVDPageBasic2 class declaration.
 */

/*
 * Copyright (C) 2006-2017 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef ___UIWizardCloneVDPageBasic2_h___
#define ___UIWizardCloneVDPageBasic2_h___

/* GUI includes: */
#include "UIWizardPage.h"

/* COM includes: */
#include "COMEnums.h"
#include "CMediumFormat.h"

/* Forward declarations: */
class QVBoxLayout;
class QButtonGroup;
class QRadioButton;
class QIRichTextLabel;


/** 2nd page of the Clone Virtual Disk Image wizard (base part): */
class UIWizardCloneVDPage2 : public UIWizardPageBase
{
protected:

    /** Constructs page basis. */
    UIWizardCloneVDPage2();

    /** Adds format button.
      * @param  pParent         Brings the parent to add button to.
      * @param  pFormatsLayout  Brings the layout to insert button to.
      * @param  medFormat       Brings the medium format object to acquire format from.
      * @param  fPreferred      Brings whether curretn format is preferred or not. */
    void addFormatButton(QWidget *pParent, QVBoxLayout *pFormatsLayout, CMediumFormat medFormat, bool fPreferred = false);

    /** Returns 'mediumFormat' field value. */
    CMediumFormat mediumFormat() const;
    /** Defines 'mediumFormat' field value. */
    void setMediumFormat(const CMediumFormat &mediumFormat);

    /** Holds the format button-group instance. */
    QButtonGroup         *m_pFormatButtonGroup;
    /** Holds the format description list. */
    QList<CMediumFormat>  m_formats;
    /** Holds the format name list. */
    QStringList           m_formatNames;
};


/** 2nd page of the Clone Virtual Disk Image wizard (basic extension): */
class UIWizardCloneVDPageBasic2 : public UIWizardPage, public UIWizardCloneVDPage2
{
    Q_OBJECT;
    Q_PROPERTY(CMediumFormat mediumFormat READ mediumFormat WRITE setMediumFormat);

public:

    /** Constructs basic page. */
    UIWizardCloneVDPageBasic2();

private:

    /** Handles translation event. */
    void retranslateUi();

    /** Prepares the page. */
    void initializePage();

    /** Returns whether the page is complete. */
    bool isComplete() const;

    /** Returns the ID of the next page to traverse to. */
    int nextId() const;

    /** Holds the description label instance. */
    QIRichTextLabel *m_pLabel;
};

#endif /* !___UIWizardCloneVDPageBasic2_h___ */

