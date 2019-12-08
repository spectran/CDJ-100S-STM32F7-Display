/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0/RPSL 1.0 
 *  
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved. 
 *      
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Public Source License 
 * Version 1.0 (the "RPSL") available at 
 * http://www.helixcommunity.org/content/rpsl unless you have licensed 
 * the file under the RealNetworks Community Source License Version 1.0 
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl, 
 * in which case the RCSL will apply. You may also obtain the license terms 
 * directly from RealNetworks.  You may not use this file except in 
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks 
 * applicable to this file, the RCSL.  Please see the applicable RPSL or 
 * RCSL for the rights, obligations and limitations governing use of the 
 * contents of the file.  
 *  
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 *  
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 *    http://www.helixcommunity.org/content/tck 
 * 
 * Contributor(s): 
 *  
 * ***** END LICENSE BLOCK ***** */ 

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * mpadecobj.cpp - C++ wrapper for compatibility with Helix mp3 renderer
 *                 
 * Notes: this sits on top of mp3dec.c, and if this C++ API is not required
 *          then the C-only API in mp3dec.c can also be called directly
 *        right now only layer 3 is enabled, since fixed-pt layer 1, 2 decoding is not
 *          currently supported
 *        see datatype/mp3/renderer for more info
 **************************************************************************************/

#include "mpadecobjfixpt.h"
#ifdef HELIX_CONFIG_MOBLIN
#include "mpadecl1.h"
#include "mpadecl2.h"
#include "../fltpt/statname.h"
#include "hlxclib/string.h"
#endif

CMpaDecObj::CMpaDecObj()
 :  m_pDec(0),
    m_pDecL1(0),
    m_pDecL2(0),
    m_pDecL3(0),
    m_bUseFrameSize(0),
    m_bTrustPackets(0)
{
}

CMpaDecObj::~CMpaDecObj()
{
    if (m_pDecL1)
    {
#if defined (HELIX_FEATURE_AUDIO_MPA_LAYER1)
		/* not implemented - see comments in mpadecobj.h */
#ifdef HELIX_CONFIG_MOBLIN
        CMpaDecoderL1* temp = (CMpaDecoderL1*)m_pDecL1;
        HX_DELETE(temp);
#else
        delete m_pDecL1;
#endif//HELIX_CONFIG_MOBLIN
#endif
        m_pDecL1 = 0;
    }

    if (m_pDecL2)
    {
#if defined (HELIX_FEATURE_AUDIO_MPA_LAYER2)
		/* not implemented - see comments in mpadecobj.h */
#ifdef HELIX_CONFIG_MOBLIN
        CMpaDecoderL2* temp = (CMpaDecoderL2*)m_pDecL2;
        HX_DELETE(temp);
#else		
        delete m_pDecL2;
#endif//HELIX_CONFIG_MOBLIN
#endif
        m_pDecL2 = 0;
    }

    if (m_pDecL3)
    {
#if defined (HELIX_FEATURE_AUDIO_MPA_LAYER3)
        MP3FreeDecoder(m_pDecL3);
#endif
        m_pDecL3 = 0;
    }

	m_pDec = 0;
}

/* returns 1 on success, 0 on failure */
int CMpaDecObj::Init_n(unsigned char *pSync, 
					   unsigned long ulSize, 
					   unsigned char bUseSize)
{
	/* Right now only layer 3 is implemented in fixed-point. 
	 * To add layers 1,2 you'll need to parse the MPEG header, 
	 *   switch() on the layer number, and init the right codec.
	 */
#ifdef HELIX_CONFIG_MOBLIN
    MPEG_HEAD h;
    memset(&h, 0, sizeof(h));
    int nFrameSize = head_info(pSync, ulSize, &h, m_bTrustPackets);
    if (!nFrameSize)
    {
        return nFrameSize;
    }
    decoder_state = h.option;
    switch (h.option)
    {

    case 1:
#if defined (HELIX_FEATURE_AUDIO_MPA_LAYER3)
	m_pDecL3 = MP3InitDecoder();
	if (MP3GetNextFrameInfo((HMP3Decoder)m_pDecL3, &m_lastMP3FrameInfo, pSync)) {
		/* returns non-zero if error */
        MP3FreeDecoder(m_pDecL3);
        m_pDecL3 = 0;
	} else {
		m_pDec = m_pDecL3;
	}
#endif
    
    if (!m_pDec)
        return 0;

	m_bUseFrameSize = bUseSize;
	
	return 1;
	break;

    case 2:
#if defined (HELIX_FEATURE_AUDIO_MPA_LAYER2)
        m_pDecL2 = new CMpaDecoderL2();
#endif //HELIX_FEATURE_AUDIO_MPA_LAYER2

        m_pDec = m_pDecL2;
        break;

    case 3:
#if defined (HELIX_FEATURE_AUDIO_MPA_LAYER2)
        m_pDecL1 = new CMpaDecoderL1();
#endif //HELIX_FEATURE_AUDIO_MPA_LAYER2

        m_pDec = m_pDecL1;
        break;
    }
	if (!m_pDec)
        {
            return 0;
        }
        m_bUseFrameSize = bUseSize;
        return ((CMpaDecoder*)m_pDec)->audio_decode_init(&h, nFrameSize, 0, 0, 0, 24000, (int)m_bUseFrameSize);
#else
#if defined (HELIX_FEATURE_AUDIO_MPA_LAYER3)
    m_pDecL3 = MP3InitDecoder();
    if (MP3GetNextFrameInfo((HMP3Decoder)m_pDecL3, &m_lastMP3FrameInfo, pSync)) 
    {
        /* returns non-zero if error */
        MP3FreeDecoder(m_pDecL3);
        m_pDecL3 = 0;
    } 
    else 
    {
        m_pDec = m_pDecL3;
    }
#endif
    
    if (!m_pDec)
        return 0;

	m_bUseFrameSize = bUseSize;
	return 1;
#endif//HELIX_CONFIG_MOBLIN END
    
}

/* passthrough for backwards compatibility, disregarding error code (see comments below) */
void CMpaDecObj::DecodeFrame_v(unsigned char *pSource,
              	               unsigned long *pulSize,
                   	           unsigned char *pPCM,
                       	       unsigned long *pulPCMSize)
{
#ifdef HELIX_CONFIG_MOBLIN
    switch(decoder_state)
    {
    case 1:
	DecodeFrame_v(pSource, pulSize, pPCM, pulPCMSize, 0);
	break;
    case 2:
    case 3:
	{
            IN_OUT io = ((CMpaDecoder*)m_pDec)->audio_decode(pSource, pPCM, m_bUseFrameSize ? *pulSize : 0);
            *pulSize = io.in_bytes;
            *pulPCMSize = io.out_bytes;	
	}
	break;
    }
#else
    DecodeFrame_v(pSource, pulSize, pPCM, pulPCMSize, 0);
#endif//HELIX_CONFIG_MOBLIN END
}

void CMpaDecObj::DecodeFrame_v(unsigned char *pSource,
              	               unsigned long *pulSize,
                   	           unsigned char *pPCM,
                       	       unsigned long *pulPCMSize,
							   int *errCode)
{
	int err=0, bytesLeft;
	unsigned char *buf;

	buf = pSource;
	bytesLeft = (int)(*pulSize);
	if (m_pDec)
		err = MP3Decode((HMP3Decoder)m_pDec, &buf, &bytesLeft, (short *)pPCM, (int)m_bUseFrameSize);

	/* see mpadecobj.h - DecodeFrame_v() overloaded so it will continue to work if
	 *   errCode parameter is not included. But if you do include an errCode ptr you'll get
	 *   the error result (recommended - see main.c for examples of handling error codes)
	 */
	if (errCode)
		*errCode = err;

	if (err) {
		*pulSize = (*pulSize - (unsigned long)bytesLeft);
		*pulPCMSize = 0;
	} else {
		MP3GetLastFrameInfo((HMP3Decoder)m_pDec, &m_lastMP3FrameInfo);
		*pulSize = (*pulSize - (unsigned long)bytesLeft);
		*pulPCMSize = (m_lastMP3FrameInfo.bitsPerSample >> 3) * m_lastMP3FrameInfo.outputSamps;
	}
}

void CMpaDecObj::GetPCMInfo_v(unsigned long &ulSampRate,
                              int &nChannels,
                              int &nBitsPerSample)
{
#ifdef HELIX_CONFIG_MOBLIN
    switch(decoder_state)
    {
    case 1:	
	ulSampRate = m_lastMP3FrameInfo.samprate;
	nChannels = m_lastMP3FrameInfo.nChans;
	nBitsPerSample = m_lastMP3FrameInfo.bitsPerSample;
	break;
    case 2:
    case 3:
    ((CMpaDecoder*)m_pDec)->GetPCMInfo_v(ulSampRate, nChannels, nBitsPerSample);
	break;
    }
#else
	ulSampRate = m_lastMP3FrameInfo.samprate;
	nChannels = m_lastMP3FrameInfo.nChans;
	nBitsPerSample = m_lastMP3FrameInfo.bitsPerSample;
#endif//HELIX_CONFIG_MOBLIN END
}

/* return number of samples per frame, per channel, actually (see usage in payload/pktparse.cpp) */
int CMpaDecObj::GetSamplesPerFrame_n()
{
#ifdef HELIX_CONFIG_MOBLIN
    switch(decoder_state)
    {
    case 1:
	return (m_lastMP3FrameInfo.outputSamps >> (m_lastMP3FrameInfo.nChans - 1));	/* only works for 1 or 2 channels */
	break;
    case 2:
    case 3:
    return ((CMpaDecoder*)m_pDec)->GetSamplesPerFrame_n();
	break;
    }
#else
    return (m_lastMP3FrameInfo.outputSamps >> (m_lastMP3FrameInfo.nChans - 1));	/* only works for 1 or 2 channels */
#endif//HELIX_CONFIG_MOBLIN END
}



