#ifndef MY_ABC_HERE
#define MY_ABC_HERE
#endif
#ifndef _SYNO_G1_CODEC_PATENT_H
#define _SYNO_G1_CODEC_PATENT_H

#ifdef MY_ABC_HERE

#define SZ_SYNO_CODEC_SKIP_ACTIVATION "SYNO_CODEC_SKIP_ACTIVATION"

//TODO must change by define by package
#if defined(MY_ABC_HERE)
#define SZ_PKG_NAME "videostation"
#elif defined(SYNO_AUDIOSTATION)
#define SZ_PKG_NAME "audiostation"
#elif defined(SYNO_MEDIASERVER)
#define SZ_PKG_NAME "mediaserver"
#else
#define SZ_PKG_NAME "dsm"
#endif

int ActivateCodec(const char *szCodecName);
#endif //MY_ABC_HERE

#endif //_SYNO_G1_CODEC_PATENT_H
