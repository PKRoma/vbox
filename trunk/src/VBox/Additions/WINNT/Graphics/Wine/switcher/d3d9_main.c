/* $Id$ */

/** @file
 * VBox D3D8 dll switcher
 */

/*
 * Copyright (C) 2009 Sun Microsystems, Inc.
 *
 * Sun Microsystems, Inc. confidential
 * All rights reserved
 */

#include "d3d9.h"
#include "switcher.h"

typedef void (WINAPI *DebugSetMuteProc)(void);
typedef IDirect3D9* (WINAPI *Direct3DCreate9Proc)(UINT SDKVersion);
typedef HRESULT (WINAPI *Direct3DCreate9ExProc)(UINT SDKVersion, IDirect3D9Ex **direct3d9ex);
typedef void* (WINAPI *Direct3DShaderValidatorCreate9Proc)(void);
typedef int (WINAPI *D3DPERF_BeginEventProc)(D3DCOLOR color, LPCWSTR name);
typedef int (WINAPI *D3DPERF_EndEventProc)(void);
typedef DWORD (WINAPI *D3DPERF_GetStatusProc)(void);
typedef void (WINAPI *D3DPERF_SetOptionsProc)(DWORD options);
typedef BOOL (WINAPI *D3DPERF_QueryRepeatFrameProc)(void);
typedef void (WINAPI *D3DPERF_SetMarkerProc)(D3DCOLOR color, LPCWSTR name);
typedef void (WINAPI *D3DPERF_SetRegionProc)(D3DCOLOR color, LPCWSTR name);

typedef struct _D3D9ExTag
{
    int                     initialized;
    const char              *vboxName;
    const char              *msName;
    DebugSetMuteProc        pDebugSetMute;
    Direct3DCreate9Proc     pDirect3DCreate9;
    Direct3DCreate9ExProc   pDirect3DCreate9Ex;
    Direct3DShaderValidatorCreate9Proc pDirect3DShaderValidatorCreate9;
    D3DPERF_BeginEventProc  pD3DPERF_BeginEvent;
    D3DPERF_EndEventProc    pD3DPERF_EndEvent;
    D3DPERF_GetStatusProc   pD3DPERF_GetStatus;
    D3DPERF_SetOptionsProc  pD3DPERF_SetOptions;
    D3DPERF_QueryRepeatFrameProc pD3DPERF_QueryRepeatFrame;
    D3DPERF_SetMarkerProc   pD3DPERF_SetMarker;
    D3DPERF_SetRegionProc   pD3DPERF_SetRegion;
} D3D9Export;

static D3D9Export g_swd3d9 = {0, "VBoxD3D9.dll", "MSD3D9.dll",};

void FillD3DExports(HANDLE hDLL)
{
    SW_FILLPROC(g_swd3d9, hDLL, DebugSetMute);
    SW_FILLPROC(g_swd3d9, hDLL, Direct3DCreate9);
    SW_FILLPROC(g_swd3d9, hDLL, Direct3DCreate9Ex);
    SW_FILLPROC(g_swd3d9, hDLL, Direct3DShaderValidatorCreate9);
    SW_FILLPROC(g_swd3d9, hDLL, D3DPERF_BeginEvent);
    SW_FILLPROC(g_swd3d9, hDLL, D3DPERF_EndEvent);
    SW_FILLPROC(g_swd3d9, hDLL, D3DPERF_GetStatus);
    SW_FILLPROC(g_swd3d9, hDLL, D3DPERF_SetOptions);
    SW_FILLPROC(g_swd3d9, hDLL, D3DPERF_QueryRepeatFrame);
    SW_FILLPROC(g_swd3d9, hDLL, D3DPERF_SetMarker);
    SW_FILLPROC(g_swd3d9, hDLL, D3DPERF_SetRegion);
}

void WINAPI DebugSetMute(void) 
{
    SW_CHECKCALL(g_swd3d9, DebugSetMute);
    g_swd3d9.pDebugSetMute();    
}

IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion)
{
    SW_CHECKRET(g_swd3d9, Direct3DCreate9, NULL);
    return g_swd3d9.pDirect3DCreate9(SDKVersion);
}

HRESULT WINAPI Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex **direct3d9ex)
{
    SW_CHECKRET(g_swd3d9, Direct3DCreate9Ex, E_FAIL);
    return g_swd3d9.pDirect3DCreate9Ex(SDKVersion, direct3d9ex);
}

void* WINAPI Direct3DShaderValidatorCreate9(void)
{
    SW_CHECKRET(g_swd3d9, Direct3DShaderValidatorCreate9, NULL);
    return g_swd3d9.pDirect3DShaderValidatorCreate9();
}

int WINAPI D3DPERF_BeginEvent(D3DCOLOR color, LPCWSTR name)
{
    SW_CHECKRET(g_swd3d9, D3DPERF_BeginEvent, -1);
    return g_swd3d9.pD3DPERF_BeginEvent(color, name);
}

int WINAPI D3DPERF_EndEvent(void)
{
    SW_CHECKRET(g_swd3d9, D3DPERF_EndEvent, -1);
    return g_swd3d9.pD3DPERF_EndEvent();
}

DWORD WINAPI D3DPERF_GetStatus(void)
{
    SW_CHECKRET(g_swd3d9, D3DPERF_EndEvent, 0);
    return g_swd3d9.pD3DPERF_GetStatus();
}

void WINAPI D3DPERF_SetOptions(DWORD options)
{
    SW_CHECKCALL(g_swd3d9, D3DPERF_SetOptions);
    g_swd3d9.pD3DPERF_SetOptions(options);
}

BOOL WINAPI D3DPERF_QueryRepeatFrame(void)
{
    SW_CHECKRET(g_swd3d9, D3DPERF_QueryRepeatFrame, FALSE);
    return g_swd3d9.pD3DPERF_QueryRepeatFrame();
}

void WINAPI D3DPERF_SetMarker(D3DCOLOR color, LPCWSTR name)
{
    SW_CHECKCALL(g_swd3d9, D3DPERF_SetMarker);
    g_swd3d9.pD3DPERF_SetMarker(color, name);
}

void WINAPI D3DPERF_SetRegion(D3DCOLOR color, LPCWSTR name)
{
    SW_CHECKCALL(g_swd3d9, D3DPERF_SetRegion);
    g_swd3d9.pD3DPERF_SetRegion(color, name);
}
