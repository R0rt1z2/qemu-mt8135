#include "qemu/osdep.h"
#include "qemu/log.h"
#include "hw/misc/mtk_hacc.h"

/* HACC_ACON2 bits */
#define HACC_AES_START  0x00000001
#define HACC_AES_CLR    0x00000002
#define HACC_AES_RDY    0x00008000

/* Offsets */
#define R_CON       0x000
#define R_ACON      0x004
#define R_ACON2     0x008
#define R_ACONK     0x00C
#define R_ASRC0     0x010
#define R_ASRC1     0x014
#define R_ASRC2     0x018
#define R_ASRC3     0x01C
#define R_AKEY0     0x020
#define R_AKEY7     0x03C
#define R_ACFG0     0x040
#define R_ACFG3     0x04C
#define R_AOUT0     0x050
#define R_AOUT3     0x05C
#define R_SW_OTP0   0x060
#define R_SW_OTP7   0x07C
#define R_SECINIT0  0x080
#define R_SECINIT1  0x084
#define R_SECINIT2  0x088
#define R_MKJ       0x0A0
#define R_UNK       0x0BC

#define HACC_MMIO_SIZE 0x100

static void mtk_hacc_do_aes(MtkHaccState *s)
{
    s->hacc_aout[0] = s->hacc_asrc[0];
    s->hacc_aout[1] = s->hacc_asrc[1];
    s->hacc_aout[2] = s->hacc_asrc[2];
    s->hacc_aout[3] = s->hacc_asrc[3];

    // FIXME
    s->hacc_acon2 = (s->hacc_acon2 & ~HACC_AES_START) | HACC_AES_RDY;
}

static uint64_t mtk_hacc_read(void *opaque, hwaddr offset, unsigned size)
{
    MtkHaccState *s = MTK_HACC(opaque);

    switch (offset) {
    case R_CON:
        return s->hacc_con;
    case R_ACON:
        return s->hacc_acon;
    case R_ACON2:
        return s->hacc_acon2;
    case R_ACONK:
        return s->hacc_aconk;
    case R_ASRC0 ... R_ASRC3:
        return s->hacc_asrc[(offset - R_ASRC0) / 4];
    case R_AKEY0 ... R_AKEY7:
        return s->hacc_akey[(offset - R_AKEY0) / 4];
    case R_ACFG0 ... R_ACFG3:
        return s->hacc_acfg[(offset - R_ACFG0) / 4];
    case R_AOUT0 ... R_AOUT3:
        return s->hacc_aout[(offset - R_AOUT0) / 4];
    case R_SW_OTP0 ... R_SW_OTP7:
        return s->hacc_sw_otp[(offset - R_SW_OTP0) / 4];
    case R_SECINIT0:
        return s->hacc_secinit[0];
    case R_SECINIT1:
        return s->hacc_secinit[1];
    case R_SECINIT2:
        return s->hacc_secinit[2];
    case R_MKJ:
        return s->hacc_mkj;
    case R_UNK:
        return s->hacc_unk;
    default:
        qemu_log_mask(LOG_UNIMP,
                      "mtk-hacc: unhandled read at 0x%03" HWADDR_PRIx "\n",
                      offset);
        return 0;
    }
}

static void mtk_hacc_write(void *opaque, hwaddr offset,
                            uint64_t value, unsigned size)
{
    MtkHaccState *s = MTK_HACC(opaque);

    switch (offset) {
    case R_CON:
        s->hacc_con = value;
        break;
    case R_ACON:
        s->hacc_acon = value;
        break;
    case R_ACON2:
        if (value & HACC_AES_CLR) {
            memset(s->hacc_asrc, 0, sizeof(s->hacc_asrc));
            memset(s->hacc_acfg, 0, sizeof(s->hacc_acfg));
            memset(s->hacc_aout, 0, sizeof(s->hacc_aout));
            s->hacc_acon2 = value & ~HACC_AES_CLR;
            /*
             * The BROM sometimes writes 0x40000002 and polls for
             * bit 31 to be set. Set it immediately :)
             */
            if (value & 0x40000000) {
                s->hacc_acon2 |= 0x80000000;
            }
            break;
        }
        if (value & HACC_AES_START) {
            s->hacc_acon2 = value;
            mtk_hacc_do_aes(s);
            break;
        }
        s->hacc_acon2 = value;
        if (value & 0x40000000) {
            s->hacc_acon2 |= 0x80000000;
        }
        break;
    case R_ACONK:
        s->hacc_aconk = value;
        break;
    case R_ASRC0 ... R_ASRC3:
        s->hacc_asrc[(offset - R_ASRC0) / 4] = value;
        break;
    case R_AKEY0 ... R_AKEY7:
        s->hacc_akey[(offset - R_AKEY0) / 4] = value;
        break;
    case R_ACFG0 ... R_ACFG3:
        s->hacc_acfg[(offset - R_ACFG0) / 4] = value;
        break;
    case R_AOUT0 ... R_AOUT3:
        s->hacc_aout[(offset - R_AOUT0) / 4] = value;
        break;
    case R_SW_OTP0 ... R_SW_OTP7:
        s->hacc_sw_otp[(offset - R_SW_OTP0) / 4] = value;
        break;
    case R_SECINIT0:
        s->hacc_secinit[0] = value;
        break;
    case R_SECINIT1:
        s->hacc_secinit[1] = value;
        break;
    case R_SECINIT2:
        s->hacc_secinit[2] = value;
        break;
    case R_MKJ:
        s->hacc_mkj = value;
        break;
    case R_UNK:
        s->hacc_unk = value;
        break;
    default:
        qemu_log_mask(LOG_UNIMP,
                      "mtk-hacc: unhandled write at 0x%03" HWADDR_PRIx
                      " = 0x%" PRIx64 "\n", offset, value);
        break;
    }
}

static const MemoryRegionOps mtk_hacc_ops = {
    .read = mtk_hacc_read,
    .write = mtk_hacc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

static void mtk_hacc_init(Object *obj)
{
    MtkHaccState *s = MTK_HACC(obj);

    memory_region_init_io(&s->mmio, obj, &mtk_hacc_ops, s,
                          TYPE_MTK_HACC, HACC_MMIO_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static const TypeInfo mtk_hacc_info = {
    .name          = TYPE_MTK_HACC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(MtkHaccState),
    .instance_init = mtk_hacc_init,
};

static void mtk_hacc_register_types(void)
{
    type_register_static(&mtk_hacc_info);
}

type_init(mtk_hacc_register_types)