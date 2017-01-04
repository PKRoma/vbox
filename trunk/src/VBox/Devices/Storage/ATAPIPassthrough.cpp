/* $Id$ */
/** @file
 * VBox storage devices: ATAPI emulation (common code for DevATA and DevAHCI).
 */

/*
 * Copyright (C) 2012-2016 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */
#define LOG_GROUP LOG_GROUP_DEV_IDE
#include <iprt/log.h>
#include <iprt/assert.h>
#include <iprt/mem.h>

#include <VBox/log.h>
#include <VBox/err.h>
#include <VBox/cdefs.h>
#include <VBox/scsi.h>
#include <VBox/scsiinline.h>

#include "ATAPIPassthrough.h"

/** The track was not detected yet. */
#define TRACK_FLAGS_UNDETECTED   RT_BIT_32(0)
/** The track is the lead in track of the medium. */
#define TRACK_FLAGS_LEAD_IN      RT_BIT_32(1)
/** The track is the lead out track of the medium. */
#define TRACK_FLAGS_LEAD_OUT     RT_BIT_32(2)

/** Don't clear already detected tracks on the medium. */
#define ATAPI_TRACK_LIST_REALLOCATE_FLAGS_DONT_CLEAR RT_BIT_32(0)

/**
 * Track main data form.
 */
typedef enum TRACKDATAFORM
{
    /** Invalid data form. */
    TRACKDATAFORM_INVALID = 0,
    /** 2352 bytes of data. */
    TRACKDATAFORM_CDDA,
    /** CDDA data is pause. */
    TRACKDATAFORM_CDDA_PAUSE,
    /** Mode 1 with 2048 bytes sector size. */
    TRACKDATAFORM_MODE1_2048,
    /** Mode 1 with 2352 bytes sector size. */
    TRACKDATAFORM_MODE1_2352,
    /** Mode 1 with 0 bytes sector size (generated by the drive). */
    TRACKDATAFORM_MODE1_0,
    /** XA Mode with 2336 bytes sector size. */
    TRACKDATAFORM_XA_2336,
    /** XA Mode with 2352 bytes sector size. */
    TRACKDATAFORM_XA_2352,
    /** XA Mode with 0 bytes sector size (generated by the drive). */
    TRACKDATAFORM_XA_0,
    /** Mode 2 with 2336 bytes sector size. */
    TRACKDATAFORM_MODE2_2336,
    /** Mode 2 with 2352 bytes sector size. */
    TRACKDATAFORM_MODE2_2352,
    /** Mode 2 with 0 bytes sector size (generated by the drive). */
    TRACKDATAFORM_MODE2_0
} TRACKDATAFORM;

/**
 * Subchannel data form.
 */
typedef enum SUBCHNDATAFORM
{
    /** Invalid subchannel data form. */
    SUBCHNDATAFORM_INVALID = 0,
    /** 0 bytes for the subchannel (generated by the drive). */
    SUBCHNDATAFORM_0,
    /** 96 bytes of data for the subchannel. */
    SUBCHNDATAFORM_96
} SUBCHNDATAFORM;

/**
 * Track entry.
 */
typedef struct TRACK
{
    /** Start LBA of the track. */
    int64_t        iLbaStart;
    /** Number of sectors in the track. */
    uint32_t       cSectors;
    /** Data form of main data. */
    TRACKDATAFORM  enmMainDataForm;
    /** Data form of sub channel. */
    SUBCHNDATAFORM enmSubChnDataForm;
    /** Flags for the track. */
    uint32_t       fFlags;
} TRACK, *PTRACK;

/**
 * Media track list.
 */
typedef struct TRACKLIST
{
    /** Number of detected tracks of the current medium. */
    unsigned    cTracksCurrent;
    /** Maximum number of tracks the list can contain. */
    unsigned    cTracksMax;
    /** Variable list of tracks. */
    PTRACK      paTracks;
} TRACKLIST, *PTRACKLIST;


/**
 * Reallocate the given track list to be able to hold the given number of tracks.
 *
 * @returns VBox status code.
 * @param   pTrackList    The track list to reallocate.
 * @param   cTracks       Number of tracks the list must be able to hold.
 * @param   fFlags        Flags for the reallocation.
 */
static int atapiTrackListReallocate(PTRACKLIST pTrackList, unsigned cTracks, uint32_t fFlags)
{
    int rc = VINF_SUCCESS;

    if (!(fFlags & ATAPI_TRACK_LIST_REALLOCATE_FLAGS_DONT_CLEAR))
        ATAPIPassthroughTrackListClear(pTrackList);

    if (pTrackList->cTracksMax < cTracks)
    {
        PTRACK paTracksNew = (PTRACK)RTMemRealloc(pTrackList->paTracks, cTracks * sizeof(TRACK));
        if (paTracksNew)
        {
            pTrackList->paTracks = paTracksNew;

            /* Mark new tracks as undetected. */
            for (unsigned i = pTrackList->cTracksMax; i < cTracks; i++)
                pTrackList->paTracks[i].fFlags |= TRACK_FLAGS_UNDETECTED;

            pTrackList->cTracksMax = cTracks;
        }
        else
            rc = VERR_NO_MEMORY;
    }

    if (RT_SUCCESS(rc))
        pTrackList->cTracksCurrent = cTracks;

    return rc;
}

/**
 * Initilizes the given track from the given CUE sheet entry.
 *
 * @returns nothing.
 * @param   pTrack             The track to initialize.
 * @param   pbCueSheetEntry    CUE sheet entry to use.
 */
static void atapiTrackListEntryCreateFromCueSheetEntry(PTRACK pTrack, const uint8_t *pbCueSheetEntry)
{
    TRACKDATAFORM enmTrackDataForm = TRACKDATAFORM_INVALID;
    SUBCHNDATAFORM enmSubChnDataForm = SUBCHNDATAFORM_INVALID;

    /* Determine size of main data based on the data form field. */
    switch (pbCueSheetEntry[3] & 0x3f)
    {
        case 0x00: /* CD-DA with data. */
            enmTrackDataForm = TRACKDATAFORM_CDDA;
            break;
        case 0x01: /* CD-DA without data (used for pauses between tracks). */
            enmTrackDataForm = TRACKDATAFORM_CDDA_PAUSE;
            break;
        case 0x10: /* CD-ROM mode 1 */
        case 0x12:
            enmTrackDataForm = TRACKDATAFORM_MODE1_2048;
            break;
        case 0x11:
        case 0x13:
            enmTrackDataForm = TRACKDATAFORM_MODE1_2352;
            break;
        case 0x14:
            enmTrackDataForm = TRACKDATAFORM_MODE1_0;
            break;
        case 0x20: /* CD-ROM XA, CD-I */
        case 0x22:
            enmTrackDataForm = TRACKDATAFORM_XA_2336;
            break;
        case 0x21:
        case 0x23:
            enmTrackDataForm = TRACKDATAFORM_XA_2352;
            break;
        case 0x24:
            enmTrackDataForm = TRACKDATAFORM_XA_0;
            break;
        case 0x31: /* CD-ROM Mode 2 */
        case 0x33:
            enmTrackDataForm = TRACKDATAFORM_MODE2_2352;
            break;
        case 0x30:
        case 0x32:
            enmTrackDataForm = TRACKDATAFORM_MODE2_2336;
            break;
        case 0x34:
            enmTrackDataForm = TRACKDATAFORM_MODE2_0;
            break;
        default: /* Reserved, invalid mode. Log and leave default sector size. */
            LogRel(("ATA: Invalid data form mode %d for current CUE sheet\n",
                    pbCueSheetEntry[3] & 0x3f));
    }

    /* Determine size of sub channel data based on data form field. */
    switch ((pbCueSheetEntry[3] & 0xc0) >> 6)
    {
        case 0x00: /* Sub channel all zeroes, autogenerated by the drive. */
            enmSubChnDataForm = SUBCHNDATAFORM_0;
            break;
        case 0x01:
        case 0x03:
            enmSubChnDataForm = SUBCHNDATAFORM_96;
            break;
        default:
            LogRel(("ATA: Invalid sub-channel data form mode %u for current CUE sheet\n",
                    pbCueSheetEntry[3] & 0xc0));
    }

    pTrack->enmMainDataForm = enmTrackDataForm;
    pTrack->enmSubChnDataForm = enmSubChnDataForm;
    pTrack->iLbaStart = scsiMSF2LBA(&pbCueSheetEntry[5]);
    if (pbCueSheetEntry[1] != 0xaa)
    {
        /* Calculate number of sectors from the next entry. */
        int64_t iLbaNext = scsiMSF2LBA(&pbCueSheetEntry[5+8]);
        pTrack->cSectors = iLbaNext - pTrack->iLbaStart;
    }
    else
    {
        pTrack->fFlags |= TRACK_FLAGS_LEAD_OUT;
        pTrack->cSectors = 0;
    }
    pTrack->fFlags &= ~TRACK_FLAGS_UNDETECTED;
}

/**
 * Update the track list from a SEND CUE SHEET request.
 *
 * @returns VBox status code.
 * @param   pTrackList    Track list to update.
 * @param   pbCDB         CDB of the SEND CUE SHEET request.
 * @param   pvBuf         The CUE sheet.
 */
static int atapiTrackListUpdateFromSendCueSheet(PTRACKLIST pTrackList, const uint8_t *pbCDB, const void *pvBuf)
{
    int rc = VINF_SUCCESS;
    unsigned cbCueSheet = scsiBE2H_U24(pbCDB + 6);
    unsigned cTracks = cbCueSheet / 8;

    AssertReturn(cbCueSheet % 8 == 0 && cTracks, VERR_INVALID_PARAMETER);

    rc = atapiTrackListReallocate(pTrackList, cTracks, 0);
    if (RT_SUCCESS(rc))
    {
        const uint8_t *pbCueSheet = (uint8_t *)pvBuf;
        PTRACK pTrack = pTrackList->paTracks;

        for (unsigned i = 0; i < cTracks; i++)
        {
            atapiTrackListEntryCreateFromCueSheetEntry(pTrack, pbCueSheet);
            if (i == 0)
                pTrack->fFlags |= TRACK_FLAGS_LEAD_IN;
            pTrack++;
            pbCueSheet += 8;
        }
    }

    return rc;
}

static int atapiTrackListUpdateFromSendDvdStructure(PTRACKLIST pTrackList, const uint8_t *pbCDB, const void *pvBuf)
{
    RT_NOREF(pTrackList, pbCDB, pvBuf);
    return VERR_NOT_IMPLEMENTED;
}

/**
 * Update track list from formatted TOC data.
 *
 * @returns VBox status code.
 * @param   pTrackList    The track list to update.
 * @param   iTrack        The first track the TOC has data for.
 * @param   fMSF          Flag whether block addresses are in MSF or LBA format.
 * @param   pbBuf         Buffer holding the formatted TOC.
 * @param   cbBuffer      Size of the buffer.
 */
static int atapiTrackListUpdateFromFormattedToc(PTRACKLIST pTrackList, uint8_t iTrack,
                                                bool fMSF, const uint8_t *pbBuf, uint32_t cbBuffer)
{
    RT_NOREF(iTrack, cbBuffer); /** @todo unused parameters */
    int rc = VINF_SUCCESS;
    unsigned cbToc = scsiBE2H_U16(pbBuf);
    uint8_t iTrackFirst = pbBuf[2];
    unsigned cTracks;

    cbToc -= 2;
    pbBuf += 4;
    AssertReturn(cbToc % 8 == 0, VERR_INVALID_PARAMETER);

    cTracks = cbToc / 8 + iTrackFirst;

    rc = atapiTrackListReallocate(pTrackList, iTrackFirst + cTracks, ATAPI_TRACK_LIST_REALLOCATE_FLAGS_DONT_CLEAR);
    if (RT_SUCCESS(rc))
    {
        PTRACK pTrack = &pTrackList->paTracks[iTrackFirst];

        for (unsigned i = iTrackFirst; i < cTracks; i++)
        {
            if (pbBuf[1] & 0x4)
                pTrack->enmMainDataForm = TRACKDATAFORM_MODE1_2048;
            else
                pTrack->enmMainDataForm = TRACKDATAFORM_CDDA;

            pTrack->enmSubChnDataForm = SUBCHNDATAFORM_0;
            if (fMSF)
                pTrack->iLbaStart = scsiMSF2LBA(&pbBuf[4]);
            else
                pTrack->iLbaStart = scsiBE2H_U32(&pbBuf[4]);

            if (pbBuf[2] != 0xaa)
            {
                /* Calculate number of sectors from the next entry. */
                int64_t iLbaNext;

                if (fMSF)
                    iLbaNext = scsiMSF2LBA(&pbBuf[4+8]);
                else
                    iLbaNext = scsiBE2H_U32(&pbBuf[4+8]);

                pTrack->cSectors = iLbaNext - pTrack->iLbaStart;
            }
            else
                pTrack->cSectors = 0;

            pTrack->fFlags &= ~TRACK_FLAGS_UNDETECTED;
            pbBuf += 8;
            pTrack++;
        }
    }

    return rc;
}

static int atapiTrackListUpdateFromReadTocPmaAtip(PTRACKLIST pTrackList, const uint8_t *pbCDB, const void *pvBuf)
{
    int rc = VINF_SUCCESS;
    uint16_t cbBuffer = scsiBE2H_U16(&pbCDB[7]);
    bool fMSF = (pbCDB[1] & 0x2) != 0;
    uint8_t uFmt = pbCDB[2] & 0xf;
    uint8_t iTrack = pbCDB[6];

    switch (uFmt)
    {
        case 0x00:
            rc = atapiTrackListUpdateFromFormattedToc(pTrackList, iTrack, fMSF, (uint8_t *)pvBuf, cbBuffer);
            break;
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
            rc = VERR_NOT_IMPLEMENTED;
            break;
        case 0x05:
            rc = VINF_SUCCESS; /* Does not give information about the tracklist. */
            break;
        default:
            rc = VERR_INVALID_PARAMETER;
    }

    return rc;
}

static int atapiTrackListUpdateFromReadTrackInformation(PTRACKLIST pTrackList, const uint8_t *pbCDB, const void *pvBuf)
{
    RT_NOREF(pTrackList, pbCDB, pvBuf);
    return VERR_NOT_IMPLEMENTED;
}

static int atapiTrackListUpdateFromReadDvdStructure(PTRACKLIST pTrackList, const uint8_t *pbCDB, const void *pvBuf)
{
    RT_NOREF(pTrackList, pbCDB, pvBuf);
    return VERR_NOT_IMPLEMENTED;
}

static int atapiTrackListUpdateFromReadDiscInformation(PTRACKLIST pTrackList, const uint8_t *pbCDB, const void *pvBuf)
{
    RT_NOREF(pTrackList, pbCDB, pvBuf);
    return VERR_NOT_IMPLEMENTED;
}

#ifdef LOG_ENABLED

/**
 * Converts the given track data form to a string.
 *
 * @returns Track data form as a string.
 * @param   enmTrackDataForm    The track main data form.
 */
static const char *atapiTrackListMainDataFormToString(TRACKDATAFORM enmTrackDataForm)
{
    switch (enmTrackDataForm)
    {
        case TRACKDATAFORM_CDDA:
            return "CD-DA";
        case TRACKDATAFORM_CDDA_PAUSE:
            return "CD-DA Pause";
        case TRACKDATAFORM_MODE1_2048:
            return "Mode 1 (2048 bytes)";
        case TRACKDATAFORM_MODE1_2352:
            return "Mode 1 (2352 bytes)";
        case TRACKDATAFORM_MODE1_0:
            return "Mode 1 (0 bytes)";
        case TRACKDATAFORM_XA_2336:
            return "XA (2336 bytes)";
        case TRACKDATAFORM_XA_2352:
            return "XA (2352 bytes)";
        case TRACKDATAFORM_XA_0:
            return "XA (0 bytes)";
        case TRACKDATAFORM_MODE2_2336:
            return "Mode 2 (2336 bytes)";
        case TRACKDATAFORM_MODE2_2352:
            return "Mode 2 (2352 bytes)";
        case TRACKDATAFORM_MODE2_0:
            return "Mode 2 (0 bytes)";
        case TRACKDATAFORM_INVALID:
        default:
            return "Invalid";
    }
}

/**
 * Converts the given subchannel data form to a string.
 *
 * @returns Subchannel data form as a string.
 * @param   enmSubChnDataForm    The subchannel main data form.
 */
static const char *atapiTrackListSubChnDataFormToString(SUBCHNDATAFORM enmSubChnDataForm)
{
    switch (enmSubChnDataForm)
    {
        case SUBCHNDATAFORM_0:
            return "0";
        case SUBCHNDATAFORM_96:
            return "96";
        case SUBCHNDATAFORM_INVALID:
        default:
            return "Invalid";
    }
}

/**
 * Dump the complete track list to the release log.
 *
 * @returns nothing.
 * @param   pTrackList   The track list to dump.
 */
static void atapiTrackListDump(PTRACKLIST pTrackList)
{
    LogRel(("Track List: cTracks=%u\n", pTrackList->cTracksCurrent));
    for (unsigned i = 0; i < pTrackList->cTracksCurrent; i++)
    {
        PTRACK pTrack = &pTrackList->paTracks[i];

        LogRel(("    Track %u: LBAStart=%lld cSectors=%u enmMainDataForm=%s enmSubChnDataForm=%s fFlags=[%s%s%s]\n",
                i, pTrack->iLbaStart, pTrack->cSectors, atapiTrackListMainDataFormToString(pTrack->enmMainDataForm),
                atapiTrackListSubChnDataFormToString(pTrack->enmSubChnDataForm),
                pTrack->fFlags & TRACK_FLAGS_UNDETECTED ? "UNDETECTED " : "",
                pTrack->fFlags & TRACK_FLAGS_LEAD_IN ? "Lead-In " : "",
                pTrack->fFlags & TRACK_FLAGS_LEAD_OUT ? "Lead-Out" : ""));
    }
}

#endif /* LOG_ENABLED */

DECLHIDDEN(int) ATAPIPassthroughTrackListCreateEmpty(PTRACKLIST *ppTrackList)
{
    int rc = VERR_NO_MEMORY;
    PTRACKLIST pTrackList = (PTRACKLIST)RTMemAllocZ(sizeof(TRACKLIST));

    if (pTrackList)
    {
        rc = VINF_SUCCESS;
        *ppTrackList = pTrackList;
    }

    return rc;
}

DECLHIDDEN(void) ATAPIPassthroughTrackListDestroy(PTRACKLIST pTrackList)
{
    if (pTrackList->paTracks)
        RTMemFree(pTrackList->paTracks);
    RTMemFree(pTrackList);
}

DECLHIDDEN(void) ATAPIPassthroughTrackListClear(PTRACKLIST pTrackList)
{
    AssertPtrReturnVoid(pTrackList);

    pTrackList->cTracksCurrent = 0;

    /* Mark all tracks as undetected. */
    for (unsigned i = 0; i < pTrackList->cTracksMax; i++)
        pTrackList->paTracks[i].fFlags |= TRACK_FLAGS_UNDETECTED;
}

DECLHIDDEN(int) ATAPIPassthroughTrackListUpdate(PTRACKLIST pTrackList, const uint8_t *pbCDB, const void *pvBuf)
{
    int rc = VINF_SUCCESS;

    switch (pbCDB[0])
    {
        case SCSI_SEND_CUE_SHEET:
            rc = atapiTrackListUpdateFromSendCueSheet(pTrackList, pbCDB, pvBuf);
            break;
        case SCSI_SEND_DVD_STRUCTURE:
            rc = atapiTrackListUpdateFromSendDvdStructure(pTrackList, pbCDB, pvBuf);
            break;
        case SCSI_READ_TOC_PMA_ATIP:
            rc = atapiTrackListUpdateFromReadTocPmaAtip(pTrackList, pbCDB, pvBuf);
            break;
        case SCSI_READ_TRACK_INFORMATION:
            rc = atapiTrackListUpdateFromReadTrackInformation(pTrackList, pbCDB, pvBuf);
            break;
        case SCSI_READ_DVD_STRUCTURE:
            rc = atapiTrackListUpdateFromReadDvdStructure(pTrackList, pbCDB, pvBuf);
            break;
        case SCSI_READ_DISC_INFORMATION:
            rc = atapiTrackListUpdateFromReadDiscInformation(pTrackList, pbCDB, pvBuf);
            break;
        default:
            LogRel(("ATAPI: Invalid opcode %#x while determining media layout\n", pbCDB[0]));
            rc = VERR_INVALID_PARAMETER;
    }

#ifdef LOG_ENABLED
    atapiTrackListDump(pTrackList);
#endif

    return rc;
}

DECLHIDDEN(uint32_t) ATAPIPassthroughTrackListGetSectorSizeFromLba(PTRACKLIST pTrackList, uint32_t iAtapiLba)
{
    PTRACK pTrack = NULL;
    uint32_t cbAtapiSector = 2048;

    if (pTrackList->cTracksCurrent)
    {
        if (   iAtapiLba > UINT32_C(0xffff4fa1)
            && (int32_t)iAtapiLba < -150)
        {
            /* Lead-In area, this is always the first entry in the cue sheet. */
            pTrack = pTrackList->paTracks;
            Assert(pTrack->fFlags & TRACK_FLAGS_LEAD_IN);
            LogFlowFunc(("Selected Lead-In area\n"));
        }
        else
        {
            int64_t iAtapiLba64 = (int32_t)iAtapiLba;
            pTrack = &pTrackList->paTracks[1];

            /* Go through the track list and find the correct entry. */
            for (unsigned i = 1; i < pTrackList->cTracksCurrent - 1; i++)
            {
                if (pTrack->fFlags & TRACK_FLAGS_UNDETECTED)
                    continue;

                if (   pTrack->iLbaStart <= iAtapiLba64
                    && iAtapiLba64 < pTrack->iLbaStart + pTrack->cSectors)
                    break;

                pTrack++;
            }
        }

        if (pTrack)
        {
            switch (pTrack->enmMainDataForm)
            {
                case TRACKDATAFORM_CDDA:
                case TRACKDATAFORM_MODE1_2352:
                case TRACKDATAFORM_XA_2352:
                case TRACKDATAFORM_MODE2_2352:
                    cbAtapiSector = 2352;
                    break;
                case TRACKDATAFORM_MODE1_2048:
                    cbAtapiSector = 2048;
                    break;
                case TRACKDATAFORM_CDDA_PAUSE:
                case TRACKDATAFORM_MODE1_0:
                case TRACKDATAFORM_XA_0:
                case TRACKDATAFORM_MODE2_0:
                    cbAtapiSector = 0;
                    break;
                case TRACKDATAFORM_XA_2336:
                case TRACKDATAFORM_MODE2_2336:
                    cbAtapiSector = 2336;
                    break;
                case TRACKDATAFORM_INVALID:
                default:
                    AssertMsgFailed(("Invalid track data form %d\n", pTrack->enmMainDataForm));
            }

            switch (pTrack->enmSubChnDataForm)
            {
                case SUBCHNDATAFORM_0:
                    break;
                case SUBCHNDATAFORM_96:
                    cbAtapiSector += 96;
                    break;
                case SUBCHNDATAFORM_INVALID:
                default:
                    AssertMsgFailed(("Invalid subchannel data form %d\n", pTrack->enmSubChnDataForm));
            }
        }
    }

    return cbAtapiSector;
}

