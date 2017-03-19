#include "musb_phy.h"
#include "usb_interface.h"
#include "irq_interface.h"
#include "interrupt.h"

#ifndef HAVE_MALLOC
static u8 ep0_buffer[64] ALIGNED(64);

static u8 ep1_tx_buffer[64] ALIGNED(64);
static u8 ep1_rx_buffer[64] ALIGNED(64);

static u8 ep2_tx_buffer[256] ALIGNED(64);
static u8 ep2_rx_buffer[256] ALIGNED(64);

static u8 ep3_tx_buffer[64] ALIGNED(64);
static u8 ep3_rx_buffer[64] ALIGNED(64);

#endif

//Specific by Jieli Implement
//binding endpoint
const static struct usb_phy musb_phy[0] = {
    .endpoint0 = {
        .number = 0,
        .type = USB_ENDPOINT_XFER_CONTROL,
        .size = MUSB_CONTROL_ENDPOINT_SIZE,
#ifndef HAVE_MALLOC
        .dma_buf = ep0_buffer,
#endif
    },

    .rx_endpoint[0] = {
        .number = MUSB_RX_BULK_ENDPOINT_NUMBER;
        .type = USB_ENDPOINT_XFER_BULK,
        .size = MUSB_RX_BULK_ENDPOINT_SIZE,
#ifndef HAVE_MALLOC
        .dma_buf = ep1_rx_buffer,
#endif
    },
    .rx_endpoint[1] = {
        .number = MUSB_RX_ISOCHRONOUS_ENDPOINT_NUMBER;
        .type = USB_ENDPOINT_XFER_ISOC,
        .size = MUSB_RX_ISOCHRONOUS_ENDPOINT_SIZE,
#ifndef HAVE_MALLOC
        .dma_buf = ep2_rx_buffer,
#endif
    },
    .rx_endpoint[2] = {
        .number = MUSB_RX_INTERRUPT_ENDPOINT_NUMBER;
        .type = USB_ENDPOINT_XFER_INT,
        .size = MUSB_RX_INTERRUPT_ENDPOINT_SIZE,
#ifndef HAVE_MALLOC
        .dma_buf = ep3_rx_buffer,
#endif
    },

    .tx_endpoint[0] = {
        .number = MUSB_TX_BULK_ENDPOINT_NUMBER;
        .type = USB_ENDPOINT_XFER_BULK,
        .size = MUSB_TX_BULK_ENDPOINT_SIZE,
#ifndef HAVE_MALLOC
        .dma_buf = ep1_tx_buffer,
#endif
    },
    .tx_endpoint[1] = {
        .number = MUSB_TX_ISOCHRONOUS_ENDPOINT_NUMBER;
        .type = USB_ENDPOINT_XFER_ISOC,
        .size = MUSB_TX_ISOCHRONOUS_ENDPOINT_SIZE,
#ifndef HAVE_MALLOC
        .dma_buf = ep2_tx_buffer,
#endif
    },
    .tx_endpoint[2] = {
        .number = MUSB_TX_INTERRUPT_ENDPOINT_NUMBER;
        .type = USB_ENDPOINT_XFER_INT,
        .size = MUSB_TX_INTERRUPT_ENDPOINT_SIZE,
#ifndef HAVE_MALLOC
        .dma_buf = ep3_tx_buffer,
#endif
    },
}

#define PHY_IS_IDLE(x)          (!x->state)

u8 __musb_read_reg(JL_USB_TypeDef *musb, u8 addr)
{
    u8 result;

    if (!(musb->CON0 & BIT(2))) { //USB模块复位判断
        return 0;
    }

    //ENTER_CRITICAL(int_sfr);

    musb->CON1 = (addr << 8) | BIT(14); //USB寄存器读+地址

    while ((musb->CON1 & BIT(15)) == 0);

    //result = USB_DAT;
    result = musb->CON1 & 0x00ff; //获取cpu与usb之间的通讯数据

    //LEAVE_CRITICAL(int_sfr);
    return result;
}

void __musb_write_reg(JL_USB_TypeDef *musb, u8 addr, u8 value)
{
    if (!(musb->CON0 & BIT(2))) { //USB模块复位判断
        return;
    }

    //ENTER_CRITICAL(int_sfr);

    musb->CON1 = value | (addr << 8);
    while ((musb->CON1 & BIT(15)) == 0);

    //LEAVE_CRITICAL(int_sfr);
}




static void __musb_phy_init(struct usb_phy *phy)
{
    /*-TODO-*/

    phy->sfrs->CON0 = 0;
}


static void __musb_set_io(struct usb_phy *phy)
{
    /*-TODO-*/

    phy->sfrs->IO_CON = 0;
}


/********************************************************************************/
/*
 *                   HW Abstract Layer
 */

static void musb_ep0_process(struct usb_phy *phy)
{
    struct usb_pipe *pipe = NULL;
    struct usb_rx *pkt;
    u16 pending, i;
    u8 csr0;

    //multi PHY
    musb = phy->sfrs;

    pending = __musb_read_reg_intrtxe(musb);
    pending = __musb_read_reg_intrtx(musb) & pending;

    if (pending & BIT(0)) {
        pipe = phy->endpoint0.pipe;
        csr0 = __musb_read_reg_csr0(musb);
        if (csr0 & CSR0P_SentStall) {
            pipe->transfer_state = USB_EP0_STALL;
        }
        if (csr0 & CSR0P_SetupEnd) {
            pipe->transfer_state = USB_EP0_SETUPEND;
        }
        if (csr0 & CSR0H_RxPktRdy) {
            pkt->size = __musb_read_reg_count0(musb);
            pkt->payload = phy->endpoint0.dma_buf;
        }

        if (phy->probe_handler && phy->probe_handler->ep0_handler) {
            phy->probe_handler->ep0_handler(phy->priv, pipe, pkt);
        }
    }
}

static void musb_rx_process(struct usb_phy *phy)
{
    struct usb_pipe *pipe = NULL;
    struct usb_rx *pkt;
    u16 pending, i;

    //multi PHY
    musb = phy->sfrs;

    pending = __musb_read_reg(musb, MUSB_INTRRX1E);
    pending = __musb_read_reg(musb, MUSB_INTRRX1) & pending;

    for (i = 1; i < MUSB_MAX_RX_ENDPOINT_NUMBER; i++) {

        if (pending & BIT(i)) {
            pipe = phy->rx_endpoint[i - 1].pipe;
            pkt->size = __musb_read_reg_count(musb);
            pkt->payload = phy->endpoint[i - 1].dma_buf;
            if (phy->probe_handler && phy->probe_handler->rx_handler) {
                phy->probe_handler->rx_handler(pipe, pkt);
            }
        }
    }
}


static void musb_tx_process(struct usb_phy *phy)
{
    struct usb_pipe *pipe = NULL;
    struct usb_rx *pkt;
    u16 pending, i;

    //multi PHY
    musb = phy->sfrs;

    for (i = 1; i < MUSB_MAX_TX_ENDPOINT_NUMBER; i++) {

        if (pending & BIT(i)) {
            pipe = phy->tx_endpoint[i - 1].pipe;
            pkt->size = __musb_read_reg_txcount(musb);
            pkt->payload = phy->endpoint[i - 1].dma_buf;
            if (phy->probe_handler && phy->probe_handler->tx_handler) {
                phy->probe_handler->tx_handler(pipe, pkt);
            }
        }
    }
}


static void musb0_irq_handler(void)
{
    struct usb_phy *phy = musb_phy[0];
    JL_USB_TypeDef *musb = musb_phy[0].sfrs;

    if (phy.state == IDLE_ST) {
        return;
    }

    u8 enable, pending;

    //read pending
    enable  = __musb_read_reg(musb, INTRUSBE);
    pending = __musb_read_reg(musb, INTRUSB) & enable;

    if (pending & INTRUSB_RESUME) {

    }
    if (pending & INTRUSB_RESET_BABBLE) {

    }

    musb_ep0_process(phy);

    musb_rx_process(phy);

    musb_tx_process(phy);
}


static struct usb_phy *usb_phy_alloc(u8 index)
{
    struct usb_phy *phy = NULL;

    ASSERT(index < USBSTACK_MAX_CONTROLLER_NUMBER, __func__);

    phy = &musb_phy[index];

    return phy;
}


void *musb_hw_init(u8 index, u8 role)
{
    struct usb_phy *phy = NULL;

    switch (index) {
    case 0:
        phy = usb_phy_alloc(index);
        phy->state = STANDBY_ST;
        phy->role = role;
        phy->sfrs = JL_USB;
        IRQ_REQUEST(IRQ_USB_CTRL_IDX, musb0_irq_handler, 3);
        /* IRQ_REQUEST(IRQ_USB_SOF_IDX, musb0_irq_handler, 3); */
        break;

    default:
        ASSERT(0, "musb init error %s\n"__func__);
        break;
    }

    return phy;
}


void musb_hw_endpoint_init(struct usb_phy *phy, struct usb_pipe *pipe)
{
    struct usb_endpoint *endpoint;

    //pipe to endpoint binding dispatch endpoint number & dma addr
    switch (pipe->type) {
    case USB_XFER_CONTROL:
        endpoint = phy->endpoint0;
        break;
    case USB_XFER_ISOCHRONOUS:
        endpoint = (pipe->direction) ? \
                   phy->rx_endpoint[MUSB_RX_ISOCHRONOUS_ENDPOINT_NUMBER] :
                   phy->tx_endpoint[MUSB_TX_ISOCHRONOUS_ENDPOINT_NUMBER];
        break;
    case USB_XFER_BULK:
        endpoint = (pipe->direction) ? \
                   phy->rx_endpoint[MUSB_RX_BULK_ENDPOINT_NUMBER] :
                   phy->tx_endpoint[MUSB_TX_BULK_ENDPOINT_NUMBER];
        break;
    case USB_XFER_INTERRUPT:
        endpoint = (pipe->direction) ? \
                   phy->rx_endpoint[MUSB_RX_INTERRUPT_ENDPOINT_NUMBER] :
                   phy->tx_endpoint[MUSB_TX_INTERRUPT_ENDPOINT_NUMBER];
        break;
    default:
        ASSERT(0, __func__);
        break;
    }

    //binding struct for quick access
    pipe->size = endpoint->size;
    pipe->number = endpoint->number;
    pipe->endpoint = endpoint;
    pipe->phy = phy;
}

static void *musb_ioctrl(struct usb_phy *phy, int ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);

    switch (ctrl) {
    case USB_SET_IO:
        __musb_set_io(phy, va_arg(argptr, int));
        break;

    case USB_SET_ADDRESS:
        __musb_set_addr(phy,);
        break;

    case USB_SET_STOP_AUTO_REQUEST://for ISO transfer
        break;
    }

    va_end(argptr);
}

void musb_register_packet_handler(struct usb_phy *phy, void *priv, const struct usb_phy_handler *handler)
{
    phy->priv = priv;

    phy->probe_handler = handler;
}

