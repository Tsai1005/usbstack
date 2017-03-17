#ifndef _MUSB_H_
#define _MUSB_H_

#include "typedef.h"
#include "usb_interface.h"

//.............  Full Speed USB ...................
#define MUSB_FADDR                   0x00
#define MUSB_POWER                   0x01
#define MUSB_INTRTX1                 0x02
#define MUSB_INTRTX2                 0x03
#define MUSB_INTRRX1                 0x04
#define MUSB_INTRRX2                 0x05
#define MUSB_INTRUSB                 0x06
#define MUSB_INTRTX1E                0x07
#define MUSB_INTRTX2E                0x08
#define MUSB_INTRRX1E                0x09
#define MUSB_INTRRX2E                0x0a
#define MUSB_INTRUSBE                0x0b
#define MUSB_FRAME1                  0x0c
#define MUSB_FRAME2                  0x0d
#define MUSB_INDEX                   0x0e
#define MUSB_DEVCTL                  0x0f
#define MUSB_TXMAXP                  0x10
#define MUSB_CSR0                    0x11
#define MUSB_TXCSR1                  0x11
#define MUSB_TXCSR2                  0x12
#define MUSB_RXMAXP                  0x13
#define MUSB_RXCSR1                  0x14
#define MUSB_RXCSR2                  0x15
#define MUSB_COUNT0                  0x16
#define MUSB_RXCOUNT1                0x16
#define MUSB_RXCOUNT2                0x17
#define MUSB_TXTYPE                  0x18
#define MUSB_TXINTERVAL              0x19
#define MUSB_RXTYPE                  0x1a
#define MUSB_RXINTERVAL              0x1b


/*****MUSB SFR BitMap******/
/*INTRUSB mode*/
#define INTRUSB_SUSPEND             BIT(0)
#define INTRUSB_RESUME              BIT(1)
#define INTRUSB_RESET_BABBLE        BIT(2)
#define INTRUSB_SOF                 BIT(3)
#define INTRUSB_CON                 BIT(4)
#define INTRUSB_DISCON              BIT(5)
#define INTRUSB_SESS_REQ            BIT(6)
#define INTRUSB_VBUS_ERROR          BIT(7)

/*CSR0 peripheral mode*/
#define CSR0P_RxPktRdy              0x01
#define CSR0P_TxPktRdy              0x02
#define CSR0P_SendtStall            0x04
#define CSR0P_DataEnd               0x08
#define CSR0P_SetupEnd              0x10
#define CSR0P_SentStall             0x20
#define CSR0P_ClrRxPktRdy           0x40
#define CSR0P_ClrSetupEnd           0x80


/*TXCSR1 peripheral mode*/
#define TXCSR1P_TxPktRdy            0x01
#define TXCSR1P_FIFONotEmpty        0x02
#define TXCSR1P_UnderRun            0x04
#define TXCSR1P_FlushFIFO           0x08
#define TXCSR1P_SendStall           0x10
#define TXCSR1P_SentStall           0x20
#define TXCSR1P_ClrDataTog          0x40


/*RXCSR1 peripheral mode*/
#define RXCSR1P_RxPktRdy            0x01
#define RXCSR1P_FIFOFull            0x02
#define RXCSR1P_OverRun             0x04
#define RXCSR1P_DataError           0x08
#define RXCSR1P_FlushFIFO           0x10
#define RXCSR1P_SendStall           0x20
#define RXCSR1P_SentStall           0x40
#define RXCSR1P_ClrDataTog          0x80

/*CSR0 host mode*/
#define CSR0H_RxPktRdy              0x01
#define CSR0H_TxPktRdy              0x02
#define CSR0H_RxStall               0x04
#define CSR0H_SetupPkt              0x08
#define CSR0H_Error                 0x10
#define CSR0H_ReqPkt                0x20
#define CSR0H_StatusPkt             0x40

/*TXCSR1 host mode*/
#define TXCSR1H_TxPktRdy            0x01
#define TXCSR1H_FIFONotEmpty        0x02
#define TXCSR1H_Error               0x04
#define TXCSR1H_FlushFIFO           0x08
#define TXCSR1H_RxStall             0x20
#define TXCSR1H_ClrDataTog          0x40


/*RXCSR1 host mode*/
#define RXCSR1H_RxPktRdy            0x01
#define RXCSR1H_FIFOFull            0x02
#define RXCSR1H_Error               0x04
#define RXCSR1H_DataError           0x08
#define RXCSR1H_FlushFIFO           0x10
#define RXCSR1H_ReqPkt              0x20
#define RXCSR1H_RxStall             0x40
#define RXCSR1H_ClrDataTog          0x80

#define TXCSR2_FrcDataTog           0x08
#define TXCSR2_DMAEnab              0x10
#define TXCSR2_MODE                 0x20
#define TXCSR2P_ISO                 0x40
#define TXCSR2_AutoSet              0x80

#define RXCSR2_AutoClear            0x80
#define RXCSR2P_ISO                 0x40
#define RXCSR2_DMAEnable            0x20


//.............  Full Speed USB Configuration...................
#define MUSB_MAX_CONTROLLER_NUMBER      1
#define MUSB_MAX_TX_ENDPOINT_NUMBER     3
#define MUSB_MAX_RX_ENDPOINT_NUMBER     3

#define MUSB_CONTROL_ENDPOINT_NUMBER        0
#define MUSB_CONTROL_ENDPOINT_SIZE          64

//pipe endpoint binding
#define MUSB_RX_BULK_ENDPOINT_NUMBER        1
#define MUSB_RX_BULK_ENDPOINT_SIZE          64

#define MUSB_TX_BULK_ENDPOINT_NUMBER        1
#define MUSB_TX_BULK_ENDPOINT_SIZE          64

#define MUSB_RX_ISOCHRONOUS_ENDPOINT_NUMBER     2
#define MUSB_RX_ISOCHRONOUS_ENDPOINT_SIZE       256

#define MUSB_TX_ISOCHRONOUS_ENDPOINT_NUMBER     2
#define MUSB_TX_ISOCHRONOUS_ENDPOINT_SIZE       256

#define MUSB_RX_INTERRUPT_ENDPOINT_NUMBER       3
#define MUSB_RX_INTERRUPT_ENDPOINT_SIZE         64

#define MUSB_TX_INTERRUPT_ENDPOINT_NUMBER       3
#define MUSB_TX_INTERRUPT_ENDPOINT_SIZE         64

enum {
    IDLE_ST = 0,
    STANDBY_ST,
    BUSY_ST,
};

struct usb_endpoint {
    u8 direction;
    u8 number;
    u8 type;

    u8 *dma_buf;
    u16 maxpktsize;

    struct lbuff_head *lbuf;

    void *pipe; //binding upper
    void *phy;  //quick access the phy
};

struct usb_phy {
    u8 role;
    u8 state;

    struct usb_phy_handler *probe_handler;    //control/bulk/isoc/int
    void *priv; //binding upper

    JL_USB_TypeDef *sfrs;

    struct usb_endpoint endpoint0;
    struct usb_endpoint rx_endpoint[MUSB_MAX_RX_ENDPOINT_NUMBER];
    struct usb_endpoint tx_endpoint[MUSB_MAX_TX_ENDPOINT_NUMBER];
};


/********************************************************************************/
/*
 *                   MUSB Bridge Sfr Setting
 */
extern u8 __musb_read_reg(JL_USB_TypeDef *musb, u8 addr);
extern void __musb_write_reg(JL_USB_TypeDef *musb, u8 addr, u8 value);

//---Common Registers

_INLINE_ static void __musb_write_reg_faddr(JL_USB_TypeDef *musb, u8 value)
{
    __musb_write_reg(musb, MUSB_FADDR, value);
}

_INLINE_ static u8 __musb_read_reg_power(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_POWER);
}
_INLINE_ static void __musb_write_reg_power(JL_USB_TypeDef *musb, u8 value)
{
    __musb_write_reg(musb, MUSB_POWER, value);
}

_INLINE_ static u16 __musb_read_reg_intrtx(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_INTRTX1) | __musb_read_reg(musb, MUSB_INTRTX2) << 8;
}


_INLINE_ static u16 __musb_read_reg_intrrx(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_INTRRX1) | __musb_read_reg(musb, MUSB_INTRRX2) << 8;
}


_INLINE_ static u8 __musb_read_reg_intrusb(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_INTRUSB);
}

_INLINE_ static void __musb_write_reg_intrtxe(JL_USB_TypeDef *musb, u16 value)
{
    __musb_write_reg(musb, MUSB_INTRTX1E, value & 0xff);
    __musb_write_reg(musb, MUSB_INTRTX2E, value >> 8);
}

_INLINE_ static void __musb_write_reg_intrrxe(JL_USB_TypeDef *musb, u16 value)
{
    __musb_write_reg(musb, MUSB_INTRRX1E, value & 0xff);
    __musb_write_reg(musb, MUSB_INTRRX2E, value >> 8);
}


_INLINE_ static void __musb_write_reg_intrusbe(JL_USB_TypeDef *musb, u8 value)
{
    __musb_write_reg(musb, MUSB_INTRUSBE, value)
}

_INLINE_ static u8 __musb_read_reg_index(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_INDEX);
}
_INLINE_ static void __musb_write_reg_index(JL_USB_TypeDef *musb, u8 value)
{
    __musb_write_reg(musb, MUSB_INDEX, value);
}

_INLINE_ static u8 __musb_read_reg_devctl(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_DEVCTL);
}

//---Indexed Register
_INLINE_ static u8 __musb_read_reg_csr0(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_CSR0);
}

_INLINE_ static void __musb_write_reg_csr0(JL_USB_TypeDef *musb, u8 value)
{
    __musb_wrtie_reg(musb, MUSB_CSR0, value);
}

_INLINE_ static u8 __musb_read_reg_count0(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_COUNT0);
}

_INLINE_ static void __musb_write_reg_txmaxp(JL_USB_TypeDef *musb, u8 value)
{
    __musb_wrtie_reg(musb, MUSB_TXMAXP, value);
}

_INLINE_ static u16 __musb_read_reg_txcsr(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_TXCSR1) | __musb_read_reg(musb, MUSB_TXCSR2) << 8;
}

_INLINE_ static void __musb_write_reg_txcsr(JL_USB_TypeDef *musb, u16 value)
{
    __musb_write_reg(musb, MUSB_TXCSR1, value & 0xff);
    __musb_write_reg(musb, MUSB_TXCSR2, value >> 8);
}


_INLINE_ static void __musb_write_reg_rxmaxp(JL_USB_TypeDef *musb, u8 value)
{
    __musb_wrtie_reg(musb, MUSB_RXMAXP, value);
}

_INLINE_ static u16 __musb_read_reg_rxcsr(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_RXCSR1) | __musb_read_reg(musb, MUSB_RXCSR2) << 8;
}

_INLINE_ static void __musb_write_reg_rxcsr(JL_USB_TypeDef *musb, u16 value)
{
    __musb_write_reg(musb, MUSB_RXCSR1, value & 0xff);
    __musb_write_reg(musb, MUSB_RXCSR2, value >> 8);
}


_INLINE_ static u16 __musb_read_reg_rxcount(JL_USB_TypeDef *musb)
{
    return __musb_read_reg(musb, MUSB_RXCOUNT1) | __musb_read_reg(musb, MUSB_RXCOUNT2) << 3;
}


_INLINE_ static void __musb_wrtie_reg_txtype(JL_USB_TypeDef *musb, u8 value)
{
    __musb_write_reg(musb, MUSB_TXTYPE, value);
}

_INLINE_ static void __musb_wrtie_reg_txinterval(JL_USB_TypeDef *musb, u8 value)
{
    __musb_write_reg(musb, MUSB_TXINTERVAL, value);
}

_INLINE_ static void __musb_wrtie_reg_rxtype(JL_USB_TypeDef *musb, u8 value)
{
    __musb_write_reg(musb, MUSB_RXTYPE, value);
}

_INLINE_ static void __musb_wrtie_reg_rxinterval(JL_USB_TypeDef *musb, u8 value)
{
    __musb_write_reg(musb, MUSB_RXINTERVAL, value);
}

extern void musb_hw_endpoint_init(struct usb_phy *phy, struct usb_pipe *pipe);

#endif
