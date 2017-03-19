#include "musb_phy.h"
#include "usb_interface.h"

/********************************************************************************/
/*
 *                   HW Baisc Sfr Setting
 */
//---CSR0 Peripheral Mode
static void __musbd_csr0_tx(JL_USB_TypeDef *musb)
{
    __musb_write_reg_csr0(musb, CSR0P_TxPktRdy);
}

static void __musbd_csr0_tx_end(JL_USB_TypeDef *musb)
{
    __musb_write_reg_csr0(musb, CSR0P_DataEnd | CSR0P_TxPktRdy);
}

static void __musbd_csr0_rx_end(JL_USB_TypeDef *musb)
{
    __musb_write_reg_csr0(musb, CSR0P_ClrRxPktRdy | CSR0P_DataEnd);
}

static void __musbd_csr0_sent_stall(JL_USB_TypeDef *musb)
{
    __musb_write_reg_csr0(musb, CSR0P_ClrRxPktRdy | CSR0P_SentStall);
}



static void __alloc_endpoint_memory(struct usb_endpoint *endpoint, u16 size)
{
    u32 *dma_addr;

    switch (endpoint->number) {
    case 0:
        dma_addr = musb->EP0_ADR;
        break;
    case 1:
        dma_addr = (endpoint->direction) ? musb->EP1_TADR : musb->EP1_RADR;
        break;
    case 2:
        dma_addr = (endpoint->direction) ? musb->EP2_TADR : musb->EP2_RADR;
        break;
    case 3:
        dma_addr = (endpoint->direction) ? musb->EP3_TADR : musb->EP3_RADR;
        break;
    }
#ifdef HAVE_MALLOC
    endpoint->dma_buf = __malloc(endpoint->size);
#endif
    if (!endpoint->dma_buf) {
        return -1
    }

    dma_addr = endpoint->dma_buf;
}



static void __musb_rx_endpoint_init(JL_USB_TypeDef *musb, struct usb_endpoint *endpoint)
{
    __musb_write_reg_intrrx1e(BIT(endpoint->number));

    __musb_write_reg_index(endpoint->number);

    __musb_write_reg_rxmaxp(endpoint->size / 8);

    __musb_write_reg_rxcsr1(musb, RXCSR1P_FlushFIFO | RXCSR1P_ClrDataTog);

    if (endpoint->type == USB_ENDPOINT_XFER_ISOC) {
        __musb_write_reg_rxcsr2(musb, RXCSR2P_ISO);
    } else {
        __musb_write_reg_rxcsr2(musb, 0x0);
    }
}

static void __musb_tx_endpoint_init(JL_USB_TypeDef *musb, struct usb_endpoint *endpoint)
{
    __musb_write_reg_intrtx1e(INTRTX1E, BIT(endpoint->number));

    __musb_write_reg_index(endpoint->number);

    __musb_write_reg_txmaxp(endpoint->size / 8);

    __musb_write_reg_txcsr1(musb, TXCSR1P_ClrDataTog | TXCSR1P_FlushFIFO);

    if (endpoint->type == USB_ENDPOINT_XFER_ISOC) {
        __musb_write_reg_txcsr2(musb, TXCSR2P_ISO);
    } else {
        __musb_write_reg_txcsr2(musb, 0x0);
    }
}

static void musb_hw_endpoint_open(struct usb_phy *phy, struct usb_pipe *pipe)
{
    struct usb_endpoint *endpoint;

    musb_hw_endpoint_init(phy, pipe):

    endpoint = pipe->endpoint;

    __alloc_endpoint_memory(endpoint);

    if (endpoint->number) {
    if (endpoint->direction) {
        __musb_tx_endpoint_init(musb, endpoint);
    } else {
        __musb_rx_endpoint_init(musb, endpoint);
    }
}


static void __musb_ep0_tx_flush(JL_USB_TypeDef * musb, struct usb_endpoint * endpoint, u16 size) {
    u8 csr0 = __musb_read_reg_csr0(musb);

    if (csr0 & CSR0P_TxPktRdy) {
        debug_info("CSR0 under run\n");
        return;
    }
    musb->EP0_CNT = size;
    if (size < endpoint->size) {
        __musbd_csr0_tx_end(musb);
    } else {
        __musbd_csr0_tx(musb);
    }
}

static void __musb_ep_tx_flush(JL_USB_TypeDef * musb, struct usb_endpoint * endpoint, u16 size) {
    u32 *base = musb->EP0_CNT;

    *(base + i) = size;

    __musb_write_reg_index(endpoint->number);

    __musb_write_reg_txcsr(TXCSR1P_TxPktRdy);
}

static void musb_send_packet(struct usb_pipe * pipe, u8 * packet, u16 size) {
    struct usb_endpoint *endpoint;

    endpoint = pipe->endpoint;

    memcpy(endpoint->dma_buf, packet, size);

    JL_USB_TypeDef *musb;

    musb = pipe->phy->sfrs;

    if (endpoint->number == 0) {
        __musb_ep0_tx_flush(musb, endpoint, size);
    } else {
        __musb_ep_tx_flush(musb, endpoint, size);
    }
}


static void __musb_phy_device_init(JL_USB_TypeDef * musb) {
    musb->CON0 &= ~BIT(9); //关闭比较器

    musb->CON0 &= 0xffff00;
    musb->CON1 = 0x00;

    musb->CON0 |= BIT(0);				//[4]:PHY_ON 使能USB_PHY

    musb->IO_CON &= ~(BIT(4) | BIT(5)); //DP & DM pull down disable
    musb->IO_CON |= (BIT(6) | BIT(7));  //DP & DM pull up enable

    musb->CON0 |= (BIT(4) | BIT(3) | BIT(5) | BIT(2));	//[4]:CID  [3]:TM1 [5]:VBUS	 [2]:JL_USB->NRST
}
static void ep0_packet_handler(void *,) {

}

static void *musbd_hw_init(u8 index, void *priv, const struct usb_phy_handler * handler) {
    struct usb_phy *phy
        JL_USB_TypeDef *musb;

    phy = musb_hw_init(index, USB_ROLE_DEVICE);

    __musb_phy_device_init(musb);

    musb_register_packet_handler(phy, priv, handler);

    return phy;
}

static const struct usb_driver musbd_drv = {
    .init                   = musb_hw_init,
    .open                   = musb_hw_endpoint_open;
    .send_packet            = musb_send_packet,
    .request_packet         = musb_request_packet,
    .ioctrl                 = musb_ioctrl,
};
REGISTER_USB_DEVICE_OPERATION(musb_drv);
