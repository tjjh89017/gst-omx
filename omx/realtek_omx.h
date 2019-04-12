#ifndef __REALTEK_OMX_H__
#define __REALTEK_OMX_H__
/* Add declarations for struct BUFFER needed by OMX_SE_Memcpy function */

/* Obtain from linux_media/device/realtek/proprietary/libs/libRTKAllocator/include/system/SystemMemory.h */
#ifdef __cplusplus
class SystemMemory : public AllocatorDef {
public:
    void * const PHYADDR_ERROR = AllocatorDef::PHYADDR_ERROR;
    void * const VIRADDR_ERROR = AllocatorDef::VIRADDR_ERROR;

    SystemMemory();
    virtual ~SystemMemory();

    /* Spec API of SystemMemory */
    virtual void            setAllocator(AllocatorDef * pAllocator, bool deleteOnRelease = true);
    virtual void            setDeleteAllocatorOnRelease(bool deleteOnRelease) {
        mAllocator_DeleteOnRelease = deleteOnRelease;
    };
    virtual void            genIonAllocator(bool deleteOnRelease = true);
    virtual void            genMallocAllocator(bool deleteOnRelease = true);
    virtual void            releaseAllocator();
    virtual AllocatorDef *  getAllocator() {return mAllocator;};
    virtual void            setCheckSizeAnyTime(bool en);

    /* Base API of AllocatorDef */
    virtual int             Alloc(unsigned long nByte);
    virtual int             ReAlloc(unsigned long nByte);
    virtual void            Free();
    virtual unsigned long   GetSize();
    virtual void *          GetVirAddr();
    virtual void *          GetPhyAddr();
    virtual void            Sync();
    virtual void            Flush();
    virtual void            Invalidate();

    /* Spec API of SystemMemory */
    virtual void            CheckSize       (unsigned long byte); // realloc
    virtual void            MemcpyFrom      (const void * src, unsigned long nByte);
    virtual void            MemcpyTo        (void * dest, unsigned long nByte);
    virtual void            MemcpySkipFrom  (unsigned long dst_skip_offset, const void * src, unsigned long nByte);
    virtual void            MemcpySeekTo    (unsigned long src_seek_offset, void * dest, unsigned long nByte);

    /* Spec API of SystemMemory */
    virtual void            PUT_BYTE        (unsigned char data);
    virtual void            PUT_BUFFER      (void * buf, unsigned long len);
    virtual void            PUT_LE32        (unsigned int data);
    virtual void            PUT_BE32        (unsigned int data);
    virtual void            PUT_LE16        (unsigned int data);
    virtual void            PUT_BE16        (unsigned int data);
    virtual void            PUT_RESET       () {mPutOffset = 0;};
    virtual int             PUT_SIZE        () {return (int) mPutOffset;};
    virtual void            PUT_OFFSET      (unsigned long offset) {mPutOffset = offset;};

private:
    AllocatorDef *  mAllocator;
    bool            mAllocator_DeleteOnRelease;
    unsigned long   mPutOffset;
    bool            mCheckSizeAnyTime;
};
#else
#define SYSTEMMEMORY_PHYADDR_ERROR DEF_PHYADDR_ERROR;
#define SYSTEMMEMORY_VIRADDR_ERROR DEF_VIRADDR_ERROR;

typedef struct SystemMemory SystemMemory;
#endif

/* Obtain from linux_media/hardware/realtek/realtek_omx/osal_rtk/OSAL_RTK.h */
typedef uint32_t OSAL_U32;
typedef int ion_user_handle_t;

typedef OSAL_U32 OSAL_BUS_WIDTH;

typedef struct _private_rtk_v1_data
{
    unsigned int        yuv_mode;
    unsigned long long  pts[2];
    int                 delay_mode;
    int                 delay_depth;
    unsigned int        IsForceDIBobMode;
    int                 init_frame;
    int                 deintflag; // 0:AUTODEINT 1:FORCEDEINT 2:FORCEPROGRESSIVE
    int                 ssid;
    unsigned int        lumaOffTblAddr;
    unsigned int        chromaOffTblAddr;
    unsigned int        lumaOffTblAddrR;
    unsigned int        chromaOffTblAddrR;
    unsigned int        bufBitDepth;
    unsigned int        bufFormat;
    unsigned int        transferCharacteristics;
    unsigned int        display_primaries_x0;
    unsigned int        display_primaries_y0;
    unsigned int        display_primaries_x1;
    unsigned int        display_primaries_y1;
    unsigned int        display_primaries_x2;
    unsigned int        display_primaries_y2;
    unsigned int        white_point_x;
    unsigned int        white_point_y;
    unsigned int        max_display_mastering_luminance;
    unsigned int        min_display_mastering_luminance;
    unsigned int        reserve[64-28];
} private_rtk_v1_data;

/* Obtain from linux_media/hardware/realtek/realtek_omx/osal_rtk/port.h */
typedef struct BUFFER
{
    OMX_BUFFERHEADERTYPE* header;
    OMX_BUFFERHEADERTYPE  headerdata;
    OMX_U32               flags;
    OMX_U32               allocsize;
    OSAL_BUS_WIDTH        bus_address;
    OMX_U8*               bus_data;
    ion_user_handle_t ionhdl;
    OMX_U32               ionFd;
    OMX_U32               shareFd;
    private_rtk_v1_data priv_data;
    void *pANativeWindowBuffer;
    SystemMemory          *mBufferMemory;
} BUFFER;

#endif /* __REALTEK_OMX_H__ */
