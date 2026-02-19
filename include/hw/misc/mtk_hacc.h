#ifndef MTK_HACC_H
#define MTK_HACC_H

#include "hw/core/sysbus.h"
#include "qom/object.h"

#define TYPE_MTK_HACC "mtk-hacc"
OBJECT_DECLARE_SIMPLE_TYPE(MtkHaccState, MTK_HACC)

struct MtkHaccState {
    SysBusDevice parent_obj;
    MemoryRegion mmio;

    uint32_t hacc_con;       /* 0x000 */
    uint32_t hacc_acon;      /* 0x004 */
    uint32_t hacc_acon2;     /* 0x008 */
    uint32_t hacc_aconk;     /* 0x00C */
    uint32_t hacc_asrc[4];   /* 0x010-0x01C */
    uint32_t hacc_akey[8];   /* 0x020-0x03C */
    uint32_t hacc_acfg[4];   /* 0x040-0x04C */
    uint32_t hacc_aout[4];   /* 0x050-0x05C */
    uint32_t hacc_sw_otp[8]; /* 0x060-0x07C */
    uint32_t hacc_secinit[3];/* 0x080-0x088 */
    uint32_t hacc_mkj;       /* 0x0A0 */
    uint32_t hacc_unk;       /* 0x0BC */
};

#endif