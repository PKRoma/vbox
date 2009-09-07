/* $Id$ */
/** @file
 * VBoxFBQGL Opengl-based FrameBuffer implementation
 */

/*
 * Copyright (C) 2009 Sun Microsystems, Inc.
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 */
#if defined (VBOX_GUI_USE_QGL)

#define LOG_GROUP LOG_GROUP_GUI

#include "VBoxFrameBuffer.h"

#include "VBoxConsoleView.h"
//#include "VBoxProblemReporter.h"
//#include "VBoxGlobal.h"

/* Qt includes */
#include <QGLWidget>

//#include <iprt/asm.h>
//
#ifdef VBOX_WITH_VIDEOHWACCEL
#include <VBox/VBoxVideo.h>
//#include <VBox/types.h>
//#include <VBox/ssm.h>
#endif
//#include <iprt/semaphore.h>
//
//#include <QFile>
//#include <QTextStream>


/** @class VBoxQGLFrameBuffer
 *
 *  The VBoxQImageFrameBuffer class is a class that implements the IFrameBuffer
 *  interface and uses QImage as the direct storage for VM display data. QImage
 *  is then converted to QPixmap and blitted to the console view widget.
 */

VBoxQGLFrameBuffer::VBoxQGLFrameBuffer (VBoxConsoleView *aView) :
    VBoxFrameBuffer (aView),
    mCmdPipe(aView)
{
//    mWidget = new GLWidget(aView->viewport());
#ifndef VBOXQGL_PROF_BASE
    resizeEvent (new VBoxResizeEvent (FramebufferPixelFormat_Opaque,
                                      NULL, 0, 0, 640, 480));
#else
    resizeEvent (new VBoxResizeEvent (FramebufferPixelFormat_Opaque,
                                      NULL, 0, 0, VBOXQGL_PROF_WIDTH, VBOXQGL_PROF_HEIGHT));
#endif
}

/** @note This method is called on EMT from under this object's lock */
STDMETHODIMP VBoxQGLFrameBuffer::NotifyUpdate (ULONG aX, ULONG aY,
                                                  ULONG aW, ULONG aH)
{
//    /* We're not on the GUI thread and update() isn't thread safe in
//     * Qt 4.3.x on the Win, Qt 3.3.x on the Mac (4.2.x is),
//     * on Linux (didn't check Qt 4.x there) and probably on other
//     * non-DOS platforms, so post the event instead. */
#ifdef VBOXQGL_PROF_BASE
    QApplication::postEvent (mView,
                             new VBoxRepaintEvent (aX, aY, aW, aH));
#else
    QRect r(aX, aY, aW, aH);
    mCmdPipe.postCmd(VBOXVHWA_PIPECMD_PAINT, &r);
#endif
    return S_OK;
}

#ifdef VBOXQGL_PROF_BASE
STDMETHODIMP VBoxQGLFrameBuffer::RequestResize (ULONG aScreenId, ULONG aPixelFormat,
                              BYTE *aVRAM, ULONG aBitsPerPixel, ULONG aBytesPerLine,
                              ULONG aWidth, ULONG aHeight,
                              BOOL *aFinished)
{
	aWidth = VBOXQGL_PROF_WIDTH;
	aHeight = VBOXQGL_PROF_HEIGHT;
    VBoxFrameBuffer::RequestResize (aScreenId, aPixelFormat,
            aVRAM, aBitsPerPixel, aBytesPerLine,
            aWidth, aHeight,
            aFinished);

//    if(aVRAM)
    {
        for(;;)
        {
            ULONG aX = 0;
            ULONG aY = 0;
            ULONG aW = aWidth;
            ULONG aH = aHeight;
            NotifyUpdate (aX, aY, aW, aH);
            RTThreadSleep(40);
        }
    }
    return S_OK;
}
#endif

VBoxGLWidget* VBoxQGLFrameBuffer::vboxWidget()
{
    return (VBoxGLWidget*)mView->viewport();
}

void VBoxQGLFrameBuffer::paintEvent (QPaintEvent *pe)
{
    VBoxGLWidget * pw = vboxWidget();
    pw->makeCurrent();

    QRect vp(mView->contentsX(), mView->contentsY(), pw->width(), pw->height());
    if(vp != pw->vboxViewport())
    {
        pw->vboxDoUpdateViewport(vp);
    }

    pw->performDisplay();

    pw->swapBuffers();
}

void VBoxQGLFrameBuffer::resizeEvent (VBoxResizeEvent *re)
{
    mWdt = re->width();
    mHgt = re->height();

    vboxWidget()->vboxResizeEvent(re);
}

/* processing the VHWA command, called from the GUI thread */
void VBoxQGLFrameBuffer::doProcessVHWACommand(QEvent * pEvent)
{
    vboxWidget()->vboxProcessVHWACommands(&mCmdPipe);
}


#ifdef VBOX_WITH_VIDEOHWACCEL

STDMETHODIMP VBoxQGLFrameBuffer::ProcessVHWACommand(BYTE *pCommand)
{
    VBOXVHWACMD * pCmd = (VBOXVHWACMD*)pCommand;
//    Assert(0);
    /* indicate that we process and complete the command asynchronously */
    pCmd->Flags |= VBOXVHWACMD_FLAG_HG_ASYNCH;
    /* post the command to the GUI thread for processing */
//    QApplication::postEvent (mView,
//                             new VBoxVHWACommandProcessEvent (pCmd));
    mCmdPipe.postCmd(VBOXVHWA_PIPECMD_VHWA, pCmd);
    return S_OK;
}


VBoxQImageOverlayFrameBuffer::VBoxQImageOverlayFrameBuffer (VBoxConsoleView *aView)
    : VBoxQImageFrameBuffer(aView),
      mOverlay(aView, this)
{}


STDMETHODIMP VBoxQImageOverlayFrameBuffer::ProcessVHWACommand(BYTE *pCommand)
{
    return mOverlay.onVHWACommand((VBOXVHWACMD*)pCommand);
}

void VBoxQImageOverlayFrameBuffer::doProcessVHWACommand(QEvent * pEvent)
{
    mOverlay.onVHWACommandEvent(pEvent);
}

STDMETHODIMP VBoxQImageOverlayFrameBuffer::NotifyUpdate (ULONG aX, ULONG aY,
                             ULONG aW, ULONG aH)
{
    if(mOverlay.onNotifyUpdate(aX, aY, aW, aH))
        return S_OK;
    return VBoxQImageFrameBuffer::NotifyUpdate(aX, aY, aW, aH);
}

void VBoxQImageOverlayFrameBuffer::paintEvent (QPaintEvent *pe)
{
    QRect rect;
    VBOXFBOVERLAY_RESUT res = mOverlay.onPaintEvent(pe, &rect);
    switch(res)
    {
        case VBOXFBOVERLAY_MODIFIED:
        {
            QPaintEvent modified(rect);
            VBoxQImageFrameBuffer::paintEvent(&modified);
        } break;
        case VBOXFBOVERLAY_UNTOUCHED:
            VBoxQImageFrameBuffer::paintEvent(pe);
            break;
    }
}

void VBoxQImageOverlayFrameBuffer::resizeEvent (VBoxResizeEvent *re)
{
    mOverlay.onResizeEvent(re);
    VBoxQImageFrameBuffer::resizeEvent(re);
    mOverlay.onResizeEventPostprocess(re);
}

#endif

#endif
