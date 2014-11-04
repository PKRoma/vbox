/*
 * Declarations for MSACM
 *
 * Copyright (C) the Wine project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

/*
 * Oracle LGPL Disclaimer: For the avoidance of doubt, except that if any license choice
 * other than GPL or LGPL is available it will apply instead, Oracle elects to use only
 * the Lesser General Public License version 2.1 (LGPLv2) at this time for any software where
 * a choice of LGPL license versions is made available with the language indicating
 * that LGPLv2 or any later version may be used, or where a choice of which version
 * of the LGPL is applied is otherwise unspecified.
 */

#ifndef __WINE_MSACM_H
#define __WINE_MSACM_H

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

#include <pshpack1.h>

#define ACMAPI              WINAPI

/***********************************************************************
 * Defines/Enums
 */
#define ACMERR_BASE        512
#define ACMERR_NOTPOSSIBLE (ACMERR_BASE + 0)
#define ACMERR_BUSY        (ACMERR_BASE + 1)
#define ACMERR_UNPREPARED  (ACMERR_BASE + 2)
#define ACMERR_CANCELED    (ACMERR_BASE + 3)

#define MM_ACM_OPEN  MM_STREAM_OPEN
#define MM_ACM_CLOSE MM_STREAM_CLOSE
#define MM_ACM_DONE  MM_STREAM_DONE

#define ACM_DRIVERADDF_NAME       __MSABI_LONG(0x00000001)
#define ACM_DRIVERADDF_FUNCTION   __MSABI_LONG(0x00000003)
#define ACM_DRIVERADDF_NOTIFYHWND __MSABI_LONG(0x00000004)
#define ACM_DRIVERADDF_TYPEMASK   __MSABI_LONG(0x00000007)
#define ACM_DRIVERADDF_LOCAL      __MSABI_LONG(0x00000000)
#define ACM_DRIVERADDF_GLOBAL     __MSABI_LONG(0x00000008)

#define ACMDRIVERDETAILS_SHORTNAME_CHARS  32
#define ACMDRIVERDETAILS_LONGNAME_CHARS  128
#define ACMDRIVERDETAILS_COPYRIGHT_CHARS  80
#define ACMDRIVERDETAILS_LICENSING_CHARS 128
#define ACMDRIVERDETAILS_FEATURES_CHARS  512

#define ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC mmioFOURCC('a', 'u', 'd', 'c')
#define ACMDRIVERDETAILS_FCCCOMP_UNDEFINED  mmioFOURCC('\0', '\0', '\0', '\0')

#define ACMDRIVERDETAILS_SUPPORTF_CODEC     __MSABI_LONG(0x00000001)
#define ACMDRIVERDETAILS_SUPPORTF_CONVERTER __MSABI_LONG(0x00000002)
#define ACMDRIVERDETAILS_SUPPORTF_FILTER    __MSABI_LONG(0x00000004)
#define ACMDRIVERDETAILS_SUPPORTF_HARDWARE  __MSABI_LONG(0x00000008)
#define ACMDRIVERDETAILS_SUPPORTF_ASYNC     __MSABI_LONG(0x00000010)
#define ACMDRIVERDETAILS_SUPPORTF_LOCAL     __MSABI_LONG(0x40000000)
#define ACMDRIVERDETAILS_SUPPORTF_DISABLED  __MSABI_LONG(0x80000000)

#define ACM_DRIVERENUMF_NOLOCAL  __MSABI_LONG(0x40000000)
#define ACM_DRIVERENUMF_DISABLED __MSABI_LONG(0x80000000)

#define ACM_DRIVERPRIORITYF_ENABLE    __MSABI_LONG(0x00000001)
#define ACM_DRIVERPRIORITYF_DISABLE   __MSABI_LONG(0x00000002)
#define ACM_DRIVERPRIORITYF_ABLEMASK  __MSABI_LONG(0x00000003)
#define ACM_DRIVERPRIORITYF_BEGIN     __MSABI_LONG(0x00010000)
#define ACM_DRIVERPRIORITYF_END       __MSABI_LONG(0x00020000)
#define ACM_DRIVERPRIORITYF_DEFERMASK __MSABI_LONG(0x00030000)

#define MM_ACM_FILTERCHOOSE 0x8000

#define FILTERCHOOSE_MESSAGE          0
#define FILTERCHOOSE_FILTERTAG_VERIFY (FILTERCHOOSE_MESSAGE+0)
#define FILTERCHOOSE_FILTER_VERIFY    (FILTERCHOOSE_MESSAGE+1)
#define FILTERCHOOSE_CUSTOM_VERIFY    (FILTERCHOOSE_MESSAGE+2)

#define ACMFILTERCHOOSE_STYLEF_SHOWHELP             __MSABI_LONG(0x00000004)
#define ACMFILTERCHOOSE_STYLEF_ENABLEHOOK           __MSABI_LONG(0x00000008)
#define ACMFILTERCHOOSE_STYLEF_ENABLETEMPLATE       __MSABI_LONG(0x00000010)
#define ACMFILTERCHOOSE_STYLEF_ENABLETEMPLATEHANDLE __MSABI_LONG(0x00000020)
#define ACMFILTERCHOOSE_STYLEF_INITTOFILTERSTRUCT   __MSABI_LONG(0x00000040)
#define ACMFILTERCHOOSE_STYLEF_CONTEXTHELP          __MSABI_LONG(0x00000080)

#define ACMFILTERDETAILS_FILTER_CHARS 128

#define ACM_FILTERDETAILSF_INDEX     __MSABI_LONG(0x00000000)
#define ACM_FILTERDETAILSF_FILTER    __MSABI_LONG(0x00000001)
#define ACM_FILTERDETAILSF_QUERYMASK __MSABI_LONG(0x0000000F)

#define ACMFILTERTAGDETAILS_FILTERTAG_CHARS 48

#define ACM_FILTERTAGDETAILSF_INDEX       __MSABI_LONG(0x00000000)
#define ACM_FILTERTAGDETAILSF_FILTERTAG   __MSABI_LONG(0x00000001)
#define ACM_FILTERTAGDETAILSF_LARGESTSIZE __MSABI_LONG(0x00000002)
#define ACM_FILTERTAGDETAILSF_QUERYMASK   __MSABI_LONG(0x0000000F)

#define ACM_FILTERENUMF_DWFILTERTAG __MSABI_LONG(0x00010000)

#define ACMHELPMSGSTRINGA       "acmchoose_help"
#if defined(__GNUC__)
# define ACMHELPMSGSTRINGW (const WCHAR []){ 'a','c','m', \
  'c','h','o','o','s','e','_','h','e','l','p',0 }
#elif defined(_MSC_VER)
# define ACMHELPMSGSTRINGW      L"acmchoose_help"
#else
static const WCHAR ACMHELPMSGSTRINGW[] = { 'a','c','m',
  'c','h','o','o','s','e','_','h','e','l','p',0 };
#endif
#define ACMHELPMSGSTRING WINELIB_NAME_AW(ACMHELPMSGSTRING)

#define ACMHELPMSGCONTEXTMENUA  "acmchoose_contextmenu"
#if defined(__GNUC__)
# define ACMHELPMSGCONTEXTMENUW (const WCHAR []){ 'a','c','m', \
  'c','h','o','o','s','e','_','c','o','n','t','e','x','t','m','e','n','u',0 }
#elif defined(_MSC_VER)
# define ACMHELPMSGCONTEXTMENUW L"acmchoose_contextmenu"
#else
static const WCHAR ACMHELPMSGCONTEXTMENUW[] = { 'a','c','m',
  'c','h','o','o','s','e','_','c','o','n','t','e','x','t','m','e','n','u',0 };
#endif
#define ACMHELPMSGCONTEXTMENU WINELIB_NAME_AW(ACMHELPMSGCONTEXTMENU)

#define ACMHELPMSGCONTEXTHELPA  "acmchoose_contexthelp"
#if defined(__GNUC__)
# define ACMHELPMSGCONTEXTHELPW (const WCHAR []){ 'a','c','m', \
  'c','h','o','o','s','e','_','c','o','n','t','e','x','t','h','e','l','p',0 }
#elif defined(_MSC_VER)
# define ACMHELPMSGCONTEXTHELPW L"acmchoose_contexthelp"
#else
static const WCHAR ACMHELPMSGCONTEXTHELPW[] = { 'a','c','m',
  'c','h','o','o','s','e','_','c','o','n','t','e','x','t','h','e','l','p',0 };
#endif
#define ACMHELPMSGCONTEXTHELP WINELIB_NAME_AW(ACMHELPMSGCONTEXTHELP)

#define MM_ACM_FORMATCHOOSE 0x8000

#define FORMATCHOOSE_MESSAGE          0
#define FORMATCHOOSE_FORMATTAG_VERIFY (FORMATCHOOSE_MESSAGE+0)
#define FORMATCHOOSE_FORMAT_VERIFY    (FORMATCHOOSE_MESSAGE+1)
#define FORMATCHOOSE_CUSTOM_VERIFY    (FORMATCHOOSE_MESSAGE+2)

#define ACMFORMATCHOOSE_STYLEF_SHOWHELP             __MSABI_LONG(0x00000004)
#define ACMFORMATCHOOSE_STYLEF_ENABLEHOOK           __MSABI_LONG(0x00000008)
#define ACMFORMATCHOOSE_STYLEF_ENABLETEMPLATE       __MSABI_LONG(0x00000010)
#define ACMFORMATCHOOSE_STYLEF_ENABLETEMPLATEHANDLE __MSABI_LONG(0x00000020)
#define ACMFORMATCHOOSE_STYLEF_INITTOWFXSTRUCT      __MSABI_LONG(0x00000040)
#define ACMFORMATCHOOSE_STYLEF_CONTEXTHELP          __MSABI_LONG(0x00000080)

#define ACMFORMATDETAILS_FORMAT_CHARS   128

#define ACM_FORMATDETAILSF_INDEX     __MSABI_LONG(0x00000000)
#define ACM_FORMATDETAILSF_FORMAT    __MSABI_LONG(0x00000001)
#define ACM_FORMATDETAILSF_QUERYMASK __MSABI_LONG(0x0000000F)

#define ACM_FORMATENUMF_WFORMATTAG     __MSABI_LONG(0x00010000)
#define ACM_FORMATENUMF_NCHANNELS      __MSABI_LONG(0x00020000)
#define ACM_FORMATENUMF_NSAMPLESPERSEC __MSABI_LONG(0x00040000)
#define ACM_FORMATENUMF_WBITSPERSAMPLE __MSABI_LONG(0x00080000)
#define ACM_FORMATENUMF_CONVERT        __MSABI_LONG(0x00100000)
#define ACM_FORMATENUMF_SUGGEST        __MSABI_LONG(0x00200000)
#define ACM_FORMATENUMF_HARDWARE       __MSABI_LONG(0x00400000)
#define ACM_FORMATENUMF_INPUT          __MSABI_LONG(0x00800000)
#define ACM_FORMATENUMF_OUTPUT         __MSABI_LONG(0x01000000)

#define ACM_FORMATSUGGESTF_WFORMATTAG     __MSABI_LONG(0x00010000)
#define ACM_FORMATSUGGESTF_NCHANNELS      __MSABI_LONG(0x00020000)
#define ACM_FORMATSUGGESTF_NSAMPLESPERSEC __MSABI_LONG(0x00040000)
#define ACM_FORMATSUGGESTF_WBITSPERSAMPLE __MSABI_LONG(0x00080000)
#define ACM_FORMATSUGGESTF_TYPEMASK       __MSABI_LONG(0x00FF0000)

#define ACMFORMATTAGDETAILS_FORMATTAG_CHARS 48

#define ACM_FORMATTAGDETAILSF_INDEX       __MSABI_LONG(0x00000000)
#define ACM_FORMATTAGDETAILSF_FORMATTAG   __MSABI_LONG(0x00000001)
#define ACM_FORMATTAGDETAILSF_LARGESTSIZE __MSABI_LONG(0x00000002)
#define ACM_FORMATTAGDETAILSF_QUERYMASK   __MSABI_LONG(0x0000000F)

#define ACM_METRIC_COUNT_DRIVERS            1
#define ACM_METRIC_COUNT_CODECS             2
#define ACM_METRIC_COUNT_CONVERTERS         3
#define ACM_METRIC_COUNT_FILTERS            4
#define ACM_METRIC_COUNT_DISABLED           5
#define ACM_METRIC_COUNT_HARDWARE           6
#define ACM_METRIC_COUNT_LOCAL_DRIVERS     20
#define ACM_METRIC_COUNT_LOCAL_CODECS      21
#define ACM_METRIC_COUNT_LOCAL_CONVERTERS  22
#define ACM_METRIC_COUNT_LOCAL_FILTERS     23
#define ACM_METRIC_COUNT_LOCAL_DISABLED    24
#define ACM_METRIC_HARDWARE_WAVE_INPUT     30
#define ACM_METRIC_HARDWARE_WAVE_OUTPUT    31
#define ACM_METRIC_MAX_SIZE_FORMAT         50
#define ACM_METRIC_MAX_SIZE_FILTER         51
#define ACM_METRIC_DRIVER_SUPPORT         100
#define ACM_METRIC_DRIVER_PRIORITY        101

#define ACM_STREAMCONVERTF_BLOCKALIGN 0x00000004
#define ACM_STREAMCONVERTF_START      0x00000010
#define ACM_STREAMCONVERTF_END        0x00000020

#define ACMSTREAMHEADER_STATUSF_DONE     __MSABI_LONG(0x00010000)
#define ACMSTREAMHEADER_STATUSF_PREPARED __MSABI_LONG(0x00020000)
#define ACMSTREAMHEADER_STATUSF_INQUEUE  __MSABI_LONG(0x00100000)

#define ACM_STREAMOPENF_QUERY       0x00000001
#define ACM_STREAMOPENF_ASYNC       0x00000002
#define ACM_STREAMOPENF_NONREALTIME 0x00000004

#define ACM_STREAMSIZEF_SOURCE      __MSABI_LONG(0x00000000)
#define ACM_STREAMSIZEF_DESTINATION __MSABI_LONG(0x00000001)
#define ACM_STREAMSIZEF_QUERYMASK   __MSABI_LONG(0x0000000F)

#define ACMDM_USER                  (DRV_USER + 0x0000)
#define ACMDM_RESERVED_LOW          (DRV_USER + 0x2000)
#define ACMDM_RESERVED_HIGH         (DRV_USER + 0x2FFF)

#define ACMDM_BASE                  ACMDM_RESERVED_LOW

#define ACMDM_DRIVER_ABOUT          (ACMDM_BASE + 11)

/* handles */

DECLARE_HANDLE(HACMDRIVERID);
DECLARE_HANDLE(HACMDRIVER);
DECLARE_HANDLE(HACMSTREAM);
DECLARE_HANDLE(HACMOBJ);
typedef HACMDRIVERID *PHACMDRIVERID, *LPHACMDRIVERID;
typedef HACMDRIVER   *PHACMDRIVER, *LPHACMDRIVER;
typedef HACMSTREAM   *PHACMSTREAM, *LPHACMSTREAM;
typedef HACMOBJ      *PHACMOBJ, *LPHACMOBJ;

/***********************************************************************
 * Callbacks
 */

typedef BOOL (CALLBACK *ACMDRIVERENUMCB)(
  HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport
);

typedef UINT (CALLBACK *ACMFILTERCHOOSEHOOKPROCA)(
  HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

typedef UINT (CALLBACK *ACMFILTERCHOOSEHOOKPROCW)(
  HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);
#define	ACMFILTERCHOOSEHOOKPROC WINELIB_NAME_AW(ACMFILTERCHOOSEHOOKPROC)

typedef UINT (CALLBACK *ACMFORMATCHOOSEHOOKPROCA)(
  HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

typedef UINT (CALLBACK *ACMFORMATCHOOSEHOOKPROCW)(
  HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);
#define	ACMFORMATCHOOSEHOOKPROC WINELIB_NAME_AW(ACMFORMATCHOOSEHOOKPROC)

/***********************************************************************
 * Structures
 */

typedef struct _ACMDRIVERDETAILSA
{
  DWORD    cbStruct;

  FOURCC   fccType;
  FOURCC   fccComp;

  WORD     wMid;
  WORD     wPid;

  DWORD    vdwACM;
  DWORD    vdwDriver;

  DWORD    fdwSupport;
  DWORD    cFormatTags;
  DWORD    cFilterTags;

  HICON  hicon;

  CHAR     szShortName[ACMDRIVERDETAILS_SHORTNAME_CHARS];
  CHAR     szLongName[ACMDRIVERDETAILS_LONGNAME_CHARS];
  CHAR     szCopyright[ACMDRIVERDETAILS_COPYRIGHT_CHARS];
  CHAR     szLicensing[ACMDRIVERDETAILS_LICENSING_CHARS];
  CHAR     szFeatures[ACMDRIVERDETAILS_FEATURES_CHARS];
} ACMDRIVERDETAILSA, *PACMDRIVERDETAILSA, *LPACMDRIVERDETAILSA;

typedef struct _ACMDRIVERDETAILSW
{
  DWORD    cbStruct;

  FOURCC   fccType;
  FOURCC   fccComp;

  WORD     wMid;
  WORD     wPid;

  DWORD    vdwACM;
  DWORD    vdwDriver;

  DWORD    fdwSupport;
  DWORD    cFormatTags;
  DWORD    cFilterTags;

  HICON  hicon;

  WCHAR    szShortName[ACMDRIVERDETAILS_SHORTNAME_CHARS];
  WCHAR    szLongName[ACMDRIVERDETAILS_LONGNAME_CHARS];
  WCHAR    szCopyright[ACMDRIVERDETAILS_COPYRIGHT_CHARS];
  WCHAR    szLicensing[ACMDRIVERDETAILS_LICENSING_CHARS];
  WCHAR    szFeatures[ACMDRIVERDETAILS_FEATURES_CHARS];
} ACMDRIVERDETAILSW, *PACMDRIVERDETAILSW, *LPACMDRIVERDETAILSW;

DECL_WINELIB_TYPE_AW(ACMDRIVERDETAILS)
DECL_WINELIB_TYPE_AW(PACMDRIVERDETAILS)
DECL_WINELIB_TYPE_AW(LPACMDRIVERDETAILS)

typedef struct _ACMFILTERCHOOSEA
{
  DWORD         cbStruct;
  DWORD         fdwStyle;

  HWND        hwndOwner;

  PWAVEFILTER   pwfltr;
  DWORD         cbwfltr;

  LPCSTR        pszTitle;

  CHAR          szFilterTag[ACMFILTERTAGDETAILS_FILTERTAG_CHARS];
  CHAR          szFilter[ACMFILTERDETAILS_FILTER_CHARS];
  LPSTR         pszName;
  DWORD         cchName;

  DWORD         fdwEnum;
  PWAVEFILTER   pwfltrEnum;

  HINSTANCE   hInstance;
  LPCSTR        pszTemplateName;
  LPARAM        lCustData;
  ACMFILTERCHOOSEHOOKPROCA pfnHook;
} ACMFILTERCHOOSEA, *PACMFILTERCHOOSEA, *LPACMFILTERCHOOSEA;

typedef struct _ACMFILTERCHOOSEW
{
  DWORD         cbStruct;
  DWORD         fdwStyle;

  HWND        hwndOwner;

  PWAVEFILTER   pwfltr;
  DWORD         cbwfltr;

  LPCWSTR       pszTitle;

  WCHAR         szFilterTag[ACMFILTERTAGDETAILS_FILTERTAG_CHARS];
  WCHAR         szFilter[ACMFILTERDETAILS_FILTER_CHARS];
  LPWSTR        pszName;
  DWORD         cchName;

  DWORD         fdwEnum;
  PWAVEFILTER   pwfltrEnum;

  HINSTANCE   hInstance;
  LPCWSTR       pszTemplateName;
  LPARAM        lCustData;
  ACMFILTERCHOOSEHOOKPROCW pfnHook;
} ACMFILTERCHOOSEW, *PACMFILTERCHOOSEW, *LPACMFILTERCHOOSEW;

DECL_WINELIB_TYPE_AW(ACMFILTERCHOOSE)
DECL_WINELIB_TYPE_AW(PACMFILTERCHOOSE)
DECL_WINELIB_TYPE_AW(LPACMFILTERCHOOSE)

typedef struct _ACMFILTERDETAILSA
{
  DWORD           cbStruct;
  DWORD           dwFilterIndex;
  DWORD           dwFilterTag;
  DWORD           fdwSupport;
  PWAVEFILTER     pwfltr;
  DWORD           cbwfltr;
  CHAR            szFilter[ACMFILTERDETAILS_FILTER_CHARS];
} ACMFILTERDETAILSA, *PACMFILTERDETAILSA, *LPACMFILTERDETAILSA;

typedef struct _ACMFILTERDETAILSW
{
  DWORD          cbStruct;
  DWORD          dwFilterIndex;
  DWORD          dwFilterTag;
  DWORD          fdwSupport;
  PWAVEFILTER    pwfltr;
  DWORD          cbwfltr;
  WCHAR          szFilter[ACMFILTERDETAILS_FILTER_CHARS];
} ACMFILTERDETAILSW, *PACMFILTERDETAILSW, *LPACMFILTERDETAILSW;

DECL_WINELIB_TYPE_AW(ACMFILTERDETAILS)
DECL_WINELIB_TYPE_AW(PACMFILTERDETAILS)
DECL_WINELIB_TYPE_AW(LPACMFILTERDETAILS)

typedef struct _ACMFILTERTAGDETAILSA
{
  DWORD cbStruct;
  DWORD dwFilterTagIndex;
  DWORD dwFilterTag;
  DWORD cbFilterSize;
  DWORD fdwSupport;
  DWORD cStandardFilters;
  CHAR  szFilterTag[ACMFILTERTAGDETAILS_FILTERTAG_CHARS];
} ACMFILTERTAGDETAILSA, *PACMFILTERTAGDETAILSA, *LPACMFILTERTAGDETAILSA;

typedef struct _ACMFILTERTAGDETAILSW
{
  DWORD cbStruct;
  DWORD dwFilterTagIndex;
  DWORD dwFilterTag;
  DWORD cbFilterSize;
  DWORD fdwSupport;
  DWORD cStandardFilters;
  WCHAR szFilterTag[ACMFILTERTAGDETAILS_FILTERTAG_CHARS];
} ACMFILTERTAGDETAILSW, *PACMFILTERTAGDETAILSW, *LPACMFILTERTAGDETAILSW;

DECL_WINELIB_TYPE_AW(ACMFILTERTAGDETAILS)
DECL_WINELIB_TYPE_AW(PACMFILTERTAGDETAILS)
DECL_WINELIB_TYPE_AW(LPACMFILTERTAGDETAILS)

typedef struct _ACMFORMATCHOOSEA
{
  DWORD           cbStruct;
  DWORD           fdwStyle;

  HWND          hwndOwner;

  PWAVEFORMATEX   pwfx;
  DWORD           cbwfx;
  LPCSTR          pszTitle;

  CHAR            szFormatTag[ACMFORMATTAGDETAILS_FORMATTAG_CHARS];
  CHAR            szFormat[ACMFORMATDETAILS_FORMAT_CHARS];

  LPSTR           pszName;
  DWORD           cchName;

  DWORD           fdwEnum;
  PWAVEFORMATEX   pwfxEnum;

  HINSTANCE     hInstance;
  LPCSTR          pszTemplateName;
  LPARAM          lCustData;
  ACMFORMATCHOOSEHOOKPROCA pfnHook;
} ACMFORMATCHOOSEA, *PACMFORMATCHOOSEA, *LPACMFORMATCHOOSEA;

typedef struct _ACMFORMATCHOOSEW
{
  DWORD           cbStruct;
  DWORD           fdwStyle;

  HWND          hwndOwner;

  PWAVEFORMATEX   pwfx;
  DWORD           cbwfx;
  LPCWSTR         pszTitle;

  WCHAR           szFormatTag[ACMFORMATTAGDETAILS_FORMATTAG_CHARS];
  WCHAR           szFormat[ACMFORMATDETAILS_FORMAT_CHARS];

  LPWSTR          pszName;
  DWORD           cchName;

  DWORD           fdwEnum;
  LPWAVEFORMATEX  pwfxEnum;

  HINSTANCE     hInstance;
  LPCWSTR         pszTemplateName;
  LPARAM          lCustData;
  ACMFORMATCHOOSEHOOKPROCW pfnHook;
} ACMFORMATCHOOSEW, *PACMFORMATCHOOSEW, *LPACMFORMATCHOOSEW;

DECL_WINELIB_TYPE_AW(ACMFORMATCHOOSE)
DECL_WINELIB_TYPE_AW(PACMFORMATCHOOSE)
DECL_WINELIB_TYPE_AW(LPACMFORMATCHOOSE)

typedef struct _ACMFORMATDETAILSA
{
  DWORD           cbStruct;
  DWORD           dwFormatIndex;
  DWORD           dwFormatTag;
  DWORD           fdwSupport;
  PWAVEFORMATEX   pwfx;
  DWORD           cbwfx;
  CHAR            szFormat[ACMFORMATDETAILS_FORMAT_CHARS];
} ACMFORMATDETAILSA, *PACMFORMATDETAILSA, *LPACMFORMATDETAILSA;

typedef struct _ACMFORMATDETAILSW
{
    DWORD           cbStruct;
    DWORD           dwFormatIndex;
    DWORD           dwFormatTag;
    DWORD           fdwSupport;
    PWAVEFORMATEX   pwfx;
    DWORD           cbwfx;
    WCHAR           szFormat[ACMFORMATDETAILS_FORMAT_CHARS];
} ACMFORMATDETAILSW, *PACMFORMATDETAILSW, *LPACMFORMATDETAILSW;

DECL_WINELIB_TYPE_AW(ACMFORMATDETAILS)
DECL_WINELIB_TYPE_AW(PACMFORMATDETAILS)
DECL_WINELIB_TYPE_AW(LPACMFORMATDETAILS)

typedef struct _ACMFORMATTAGDETAILSA
{
  DWORD cbStruct;
  DWORD dwFormatTagIndex;
  DWORD dwFormatTag;
  DWORD cbFormatSize;
  DWORD fdwSupport;
  DWORD cStandardFormats;
  CHAR  szFormatTag[ACMFORMATTAGDETAILS_FORMATTAG_CHARS];
} ACMFORMATTAGDETAILSA, *PACMFORMATTAGDETAILSA, *LPACMFORMATTAGDETAILSA;

typedef struct _ACMFORMATTAGDETAILSW
{
  DWORD cbStruct;
  DWORD dwFormatTagIndex;
  DWORD dwFormatTag;
  DWORD cbFormatSize;
  DWORD fdwSupport;
  DWORD cStandardFormats;
  WCHAR szFormatTag[ACMFORMATTAGDETAILS_FORMATTAG_CHARS];
} ACMFORMATTAGDETAILSW, *PACMFORMATTAGDETAILSW, *LPACMFORMATTAGDETAILSW;

DECL_WINELIB_TYPE_AW(ACMFORMATTAGDETAILS)
DECL_WINELIB_TYPE_AW(PACMFORMATTAGDETAILS)
DECL_WINELIB_TYPE_AW(LPACMFORMATTAGDETAILS)

#ifdef _WIN64
#  define _ACMSTREAMHEADERRESERVE 15
#else
#  define _ACMSTREAMHEADERRESERVE 10
#endif

typedef struct _ACMSTREAMHEADER
{
  DWORD     cbStruct;
  DWORD     fdwStatus;
  DWORD_PTR dwUser;
  LPBYTE    pbSrc;
  DWORD     cbSrcLength;
  DWORD     cbSrcLengthUsed;
  DWORD_PTR dwSrcUser;
  LPBYTE    pbDst;
  DWORD     cbDstLength;
  DWORD     cbDstLengthUsed;
  DWORD_PTR dwDstUser;
  DWORD     dwReservedDriver[_ACMSTREAMHEADERRESERVE];
} ACMSTREAMHEADER, *PACMSTREAMHEADER, *LPACMSTREAMHEADER;

#undef _ACMSTREAMHEADERRESERVE

/***********************************************************************
 * Callbacks 2
 */

typedef BOOL (CALLBACK *ACMFILTERENUMCBA)(
  HACMDRIVERID hadid, PACMFILTERDETAILSA pafd,
  DWORD_PTR dwInstance, DWORD fdwSupport
);

typedef BOOL (CALLBACK *ACMFILTERENUMCBW)(
  HACMDRIVERID hadid, PACMFILTERDETAILSW pafd,
  DWORD_PTR dwInstance, DWORD fdwSupport
);

#define ACMFILTERENUMCB WINELIB_NAME_AW(ACMFILTERENUMCB)

typedef BOOL (CALLBACK *ACMFILTERTAGENUMCBA)(
  HACMDRIVERID hadid, PACMFILTERTAGDETAILSA paftd,
  DWORD_PTR dwInstance, DWORD fdwSupport
);

typedef BOOL (CALLBACK *ACMFILTERTAGENUMCBW)(
  HACMDRIVERID hadid, PACMFILTERTAGDETAILSW paftd,
  DWORD_PTR dwInstance, DWORD fdwSupport
);

#define ACMFILTERTAGENUMCB WINELIB_NAME_AW(ACMFILTERTAGENUMCB)

typedef BOOL (CALLBACK *ACMFORMATENUMCBA)(
  HACMDRIVERID hadid, PACMFORMATDETAILSA pafd,
  DWORD_PTR dwInstance, DWORD fdwSupport
);

typedef BOOL (CALLBACK *ACMFORMATENUMCBW)(
  HACMDRIVERID hadid, PACMFORMATDETAILSW pafd,
  DWORD_PTR dwInstance, DWORD fdwSupport
);

#define ACMFORMATENUMCB WINELIB_NAME_AW(ACMFORMATENUMCB)

typedef BOOL (CALLBACK *ACMFORMATTAGENUMCBA)(
  HACMDRIVERID hadid, PACMFORMATTAGDETAILSA paftd,
  DWORD_PTR dwInstance, DWORD fdwSupport
);

typedef BOOL (CALLBACK *ACMFORMATTAGENUMCBW)(
  HACMDRIVERID hadid, PACMFORMATTAGDETAILSW paftd,
  DWORD_PTR dwInstance, DWORD fdwSupport
);

#define ACMFORMATTAGENUMCB WINELIB_NAME_AW(ACMFORMATTAGENUMCB)

/***********************************************************************
 * Functions - Win32
 */

MMRESULT WINAPI acmDriverAddA(
  PHACMDRIVERID phadid, HINSTANCE hinstModule,
  LPARAM lParam, DWORD dwPriority, DWORD fdwAdd
);
MMRESULT WINAPI acmDriverAddW(
  PHACMDRIVERID phadid, HINSTANCE hinstModule,
  LPARAM lParam, DWORD dwPriority, DWORD fdwAdd
);
#define acmDriverAdd WINELIB_NAME_AW(acmDriverAdd)

MMRESULT WINAPI acmDriverClose(
  HACMDRIVER had, DWORD fdwClose
);
MMRESULT WINAPI acmDriverDetailsA(
  HACMDRIVERID hadid, PACMDRIVERDETAILSA padd, DWORD fdwDetails
);
MMRESULT WINAPI acmDriverDetailsW(
  HACMDRIVERID hadid, PACMDRIVERDETAILSW padd, DWORD fdwDetails
);
#define acmDriverDetails WINELIB_NAME_AW(acmDriverDetails)

MMRESULT WINAPI acmDriverEnum(
  ACMDRIVERENUMCB fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum
);
MMRESULT WINAPI acmDriverID(
  HACMOBJ hao, PHACMDRIVERID phadid, DWORD fdwDriverID
);
LRESULT WINAPI acmDriverMessage(
  HACMDRIVER had, UINT uMsg, LPARAM lParam1, LPARAM lParam2
);
MMRESULT WINAPI acmDriverOpen(
  PHACMDRIVER phad, HACMDRIVERID hadid, DWORD fdwOpen
);
MMRESULT WINAPI acmDriverPriority(
  HACMDRIVERID hadid, DWORD dwPriority, DWORD fdwPriority
);
MMRESULT WINAPI acmDriverRemove(
  HACMDRIVERID hadid, DWORD fdwRemove
);
MMRESULT WINAPI acmFilterChooseA(
  PACMFILTERCHOOSEA pafltrc
);
MMRESULT WINAPI acmFilterChooseW(
  PACMFILTERCHOOSEW pafltrc
);
#define acmFilterChoose WINELIB_NAME_AW(acmFilterChoose)

MMRESULT WINAPI acmFilterDetailsA(
  HACMDRIVER had, PACMFILTERDETAILSA pafd, DWORD fdwDetails
);
MMRESULT WINAPI acmFilterDetailsW(
  HACMDRIVER had, PACMFILTERDETAILSW pafd, DWORD fdwDetails
);
#define acmFilterDetails WINELIB_NAME_AW(acmFilterDetails)

MMRESULT WINAPI acmFilterEnumA(
  HACMDRIVER had, PACMFILTERDETAILSA pafd,
  ACMFILTERENUMCBA fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum
);
MMRESULT WINAPI acmFilterEnumW(
  HACMDRIVER had, PACMFILTERDETAILSW pafd,
  ACMFILTERENUMCBW fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum
);
#define acmFilterEnum WINELIB_NAME_AW(acmFilterEnum)

MMRESULT WINAPI acmFilterTagDetailsA(
  HACMDRIVER had, PACMFILTERTAGDETAILSA paftd, DWORD fdwDetails
);
MMRESULT WINAPI acmFilterTagDetailsW(
  HACMDRIVER had, PACMFILTERTAGDETAILSW paftd, DWORD fdwDetails
);
#define acmFilterTagDetails WINELIB_NAME_AW(acmFilterTagDetails)

MMRESULT WINAPI acmFilterTagEnumA(
  HACMDRIVER had, PACMFILTERTAGDETAILSA paftd,
  ACMFILTERTAGENUMCBA fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum
);
MMRESULT WINAPI acmFilterTagEnumW(
  HACMDRIVER had, PACMFILTERTAGDETAILSW paftd,
  ACMFILTERTAGENUMCBW fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum
);
#define acmFilterTagEnum WINELIB_NAME_AW(acmFilterTagEnum)

MMRESULT WINAPI acmFormatChooseA(
  PACMFORMATCHOOSEA pafmtc
);
MMRESULT WINAPI acmFormatChooseW(
  PACMFORMATCHOOSEW pafmtc
);
#define acmFormatChoose WINELIB_NAME_AW(acmFormatChoose)

MMRESULT WINAPI acmFormatDetailsA(
  HACMDRIVER had, PACMFORMATDETAILSA pafd, DWORD fdwDetails
);
MMRESULT WINAPI acmFormatDetailsW(
  HACMDRIVER had, PACMFORMATDETAILSW pafd, DWORD fdwDetails
);
#define acmFormatDetails WINELIB_NAME_AW(acmFormatDetails)

MMRESULT WINAPI acmFormatEnumA(
  HACMDRIVER had, PACMFORMATDETAILSA pafd,
  ACMFORMATENUMCBA fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum
);
MMRESULT WINAPI acmFormatEnumW(
  HACMDRIVER had, PACMFORMATDETAILSW pafd,
  ACMFORMATENUMCBW fnCallback, DWORD_PTR dwInstance,  DWORD fdwEnum
);
#define acmFormatEnum WINELIB_NAME_AW(acmFormatEnum)

MMRESULT WINAPI acmFormatSuggest(
  HACMDRIVER had, PWAVEFORMATEX pwfxSrc, PWAVEFORMATEX pwfxDst,
  DWORD cbwfxDst, DWORD fdwSuggest
);
MMRESULT WINAPI acmFormatTagDetailsA(
  HACMDRIVER had, PACMFORMATTAGDETAILSA paftd, DWORD fdwDetails
);
MMRESULT WINAPI acmFormatTagDetailsW(
  HACMDRIVER had, PACMFORMATTAGDETAILSW paftd, DWORD fdwDetails
);
#define acmFormatTagDetails WINELIB_NAME_AW(acmFormatTagDetails)

MMRESULT WINAPI acmFormatTagEnumA(
  HACMDRIVER had, PACMFORMATTAGDETAILSA paftd,
  ACMFORMATTAGENUMCBA fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum
);
MMRESULT WINAPI acmFormatTagEnumW(
  HACMDRIVER had, PACMFORMATTAGDETAILSW paftd,
  ACMFORMATTAGENUMCBW fnCallback, DWORD_PTR dwInstance, DWORD fdwEnum
);
#define acmFormatTagEnum WINELIB_NAME_AW(acmFormatTagEnum)

DWORD WINAPI acmGetVersion(void
);
MMRESULT WINAPI acmMetrics(
  HACMOBJ hao, UINT  uMetric, LPVOID  pMetric
);
MMRESULT WINAPI acmStreamClose(
  HACMSTREAM has, DWORD fdwClose
);
MMRESULT WINAPI acmStreamConvert(
  HACMSTREAM has, PACMSTREAMHEADER pash, DWORD fdwConvert
);
MMRESULT WINAPI acmStreamMessage(
  HACMSTREAM has, UINT uMsg, LPARAM lParam1, LPARAM lParam2
);
MMRESULT WINAPI acmStreamOpen(
  PHACMSTREAM phas, HACMDRIVER had, PWAVEFORMATEX pwfxSrc,
  PWAVEFORMATEX pwfxDst, PWAVEFILTER pwfltr, DWORD_PTR dwCallback,
  DWORD_PTR dwInstance, DWORD fdwOpen
);
MMRESULT WINAPI acmStreamPrepareHeader(
  HACMSTREAM has, PACMSTREAMHEADER pash, DWORD fdwPrepare
);
MMRESULT WINAPI acmStreamReset(
  HACMSTREAM has, DWORD fdwReset
);
MMRESULT WINAPI acmStreamSize(
  HACMSTREAM has, DWORD cbInput,
  LPDWORD pdwOutputBytes, DWORD fdwSize
);
MMRESULT WINAPI acmStreamUnprepareHeader(
  HACMSTREAM has, PACMSTREAMHEADER pash, DWORD fdwUnprepare
);

#include <poppack.h>

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif  /* __WINE_MSACM_H */
