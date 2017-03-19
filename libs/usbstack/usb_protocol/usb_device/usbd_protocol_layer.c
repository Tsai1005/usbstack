#include "usb/ch9.h"
#include "usb_interface.h"
#include "thread.h"
#include <linked_list.h>
#include "list.h"


struct usbd_hdl {
    list_head *link_head;
    struct thread protocol_thread;
};

static struct usbd_hdl hdl;

#define __this  (&hdl)

static struct usb_protocol_link *usb_link_for_index(u8 index)
{
    struct usb_protocol_link *link;

    list_for_each_entry(link, &__this->link_head, entry) {
        if (link->index == index) {
            return link;
        }
    }
    return NULL;
}

static struct usb_pipe *usb_pipe_for_number_by_link(struct usb_protocol_link *link, u8 number)
{
    struct usb_pipe *pipe;

    list_for_each_entry(pipe, &link->pipe_head, entry) {
        if (pipe->number == number) {
            return pipe;
        }
    }
    return NULL;
}



static void rx_probe_packet_handler(void *priv, struct usb_pipe *pipe, const struct usb_rx *rx)
{
    debug_info("usbd rx resume");
    struct usb_protocol_link *link = (struct usb_protocol_link *)priv;
    struct usb_rx *pkt;

    pkt = lbuf_alloc(pipe->lbuf, sizeof(*pkt) + rx->size)
    if (!pkt) {
        return -1;
    }

    pkt->size = rx->size;
    memcpy(pkt->payload, rx->payload, rx->size);
    lbuf_push(pkt);

    thread_resume(&__this->protocol_thread);
}



static void tx_probe_packet_handler(struct usb_pipe *pipe, const struct usb_tx *tx)
{
    struct usb_tx *pkt;

    if (pipe->data_len)
    {
        pipe->data_len -= pkt->maxpktsize;
        if (pipe->data_len == 0) {
            pipe->send_ok = 1;
            thread_resume(&__this->protocol_thread);
        }
    }

    //send remain packet
    pkt = lbuf_pop(pipe->lbuf);
    if (pkt) {
        __usbd_drv->send_packet(pipe, pkt->payload, pkt->size);
    }
}

static void ep0_send_respond

static void ep0_probe_packet_handler(void *priv, struct usb_pipe *pipe, const struct usb_rx *rx)
{
    switch(pipe->transfer_state)
    {
    case USB_EP0_STALL:
    case USB_EP0_SETUPEND:
        pipe->state = EP0_SETUP_ST;
        return;
    }
    switch (pipe->state) {
    case EP0_SETUP_ST:
    case EP0_DATA_OUT_ST:
        rx_probe_packet_handler(priv, pipe, rx);
        break;
    case EP0_DATA_IN_ST:
        tx_probe_packet_handler(priv, pipe, NULL);
        break;
    }
}

//register to bottom layer
static const struct usb_phy_handler probe_handler = {
    .ep0_probe_handler = ep0_probe_packet_handler,
    .rx_probe_handler = rx_probe_packet_handler,
    .tx_probe_handler = tx_probe_packet_handler,
};


static void __get_status(struct usb_pipe *pipe, const struct usb_ctrlrequest *setup)
{
    if (setup->) {
        u16 respond;
    }
    respond = SELF_POWERED;

    __usbd_drv->send_packet(pipe, __this->app_usb_desc_device, *__this->app_usb_desc_device);
}


static void __get_descriptor(const usb_pipe *pipe, const struct usb_ctrlrequest *setup)
{
    usb_std_descriptor_type_t const desc_type = p_request->wvalue >> 8;
    u8 const desc_index = p_request->wvalue & 0xff;

    switch (desc_type) {
    case USB_DESC_TYPE_DEVICE:
        __usbd_drv->send_packet(pipe, __this->app_usb_desc_device, *__this->app_usb_desc_device);
        break;

    case USB_DESC_TYPE_CONFIGURATION:
        break;

    case USB_DESC_TYPE_STRING: {
        uint8_t *p_string = __this->app_usb_desc_string;
        for (uint8_t index = 0; index < desc_index; index++) {
            p_string += (*p_string);
        }
        __usbd_drv->send_packet(pipe, p_string, *p_string);
    }
    break;

    default:
        break;
    }
}

static void ep0_setup_process(struct usb_pipe *pipe, const struct usb_rx *pkt)
{
    struct usb_ctrlrequest *setup = (struct usb_ctrlrequest *)pkt->payload;

    //notify upper layer process
    if ((setup->bRequestType & USB_TYPE_MASK) == USB_STANDARD_REQUEST) {
        pipe->handler(USB_STANDARD_REQUEST_PACKET, pipe->endpoint, setup, sizeof(struct usb_ctrlrequest));
        return;
    }

    switch (setup->bRequest) {
    case USB_REQ_GET_STATUS:        //device/interface/endpoint
        __get_status(setup);
        break;
    case USB_REQ_CLEAR_FEATURE:     //device/interface/endpoint
        __clear_featrure(setup);
        break;
    case USB_REQ_SET_FEATURE:       //device/interface/endpoint
        break;
    case USB_REQ_SET_ADDRESS:       //device
        break;
    case USB_REQ_GET_DESCRIPTOR:    //device
        __get_descriptor(setup);
        break;
    case USB_REQ_SET_DESCRIPTOR:    //device
        break;
    case USB_REQ_GET_CONFIGURATION: //device
        break;
    case USB_REQ_SET_CONFIGURATION: //device
        break;
    case USB_REQ_GET_INTERFACE:     //interface
        break;
    case USB_REQ_SET_INTERFACE:     //interface
        break;
    case USB_REQ_SYNCH_FRAME:       //endpoint
        break;
    default:
        printf("rx_std_request_process : unknow request\n");
        break;
    }
}

static void ep0_post_packet_handler(struct usb_pipe *pipe)
{
    struct usb_rx *pkt;

    while (!lbuf_empty(pipe->lbuf)) {
        pkt = lbuf_pop(pipe->lbuf);
        if (pkt) {
            switch (pipe->state) {
            case EP0_SETUP_ST:
                ep0_setup_process(pipe, pkt);
                break;
            case EP0_DATA_IN_ST:
                ep0_tx_process(pipe, pkt);
                break;
            case EP0_DATA_OUT_ST:
                ep0_rx_process(pipe, pkt);
            }
        }
    }
}



static void rx_post_packet_handler(struct usb_pipe *pipe)
{
    struct usb_rx *pkt;

    while (!lbuf_empty(pipe->lbuf)) {
        pkt = lbuf_pop(pipe->lbuf);
        if (pkt) {
            //default control pipe
            pipe->handler(USB_DATA_PACKET, pipe->endpoint, pkt, pipe->pkt_size);
        }
    }
}

static void usbstack_event_

static void tx_post_packet_handler(struct usb_pipe *pipe)
{
    if (pipe->send_ok)
    {

        pipe->handler(USB_EVENT_PACKET, pipe->end)
    }
}


static void usb_protocol_thread(struct thread *th)
{
    static usb_protocol_link *link;
    struct usb_pipe *pipe;

    list_for_each_entry(link, &__this->link_head, entry)
    list_for_each_entry(pipe & link->pipe, entry) {
        if (pipe->number == 0) {
            ep0_post_packet_handler(pipe);
        } else {
            if (pipe->direction == USB_DIR_OUT) {
                rx_post_packet_handler(pipe);
            } else {
                tx_post_packet_handler(pipe);
            }
        }
    }

    //
    thread_suspend(th, 0);
}

static void event_handler(uint8_t type, uint8_t *packet, uint16_t size)
{
    pipe->handler(USB_EVENT_PACKET, pipe->endpoint, event, event->size);
}

static void usbd_control_pipe_open(struct usb_protocol_link *link)
{
    struct usb_pipe *control_pipe;

    control_pipe = create_usb_pipe(link->phy);
    if (!pipe) {
        debug_log("create usb pipe fail!\n");
        return -2;
    }
    list_add_tail(&pipe->entry, &link->pipe_head);

    control_pipe.type = USB_XFER_CONTROL;
    control_pipe.state = USB_DEVICE_W4_SETUP;

    __usbd_drv->open(link->phy, control_pipe);
}

u8 usbd_protocol_init(u8 index, packet_handler)
{
    struct usb_protocol_link *link;

    link = create_usbd_link(index);
    if (!link) {
        debug_log("create usb link fail!\n");
        return -1;
    }
    list_add_tail(&link->entry, &__this->link_head);

    link->phy = __usbd_drv->init(index, link, probe_handler);

    usbd_control_pipe_init(link);

    thread_create(&__this->protocol_thread, usb_protocol_thread);

    INIT_LIST_HEAD(&__this->descriptor_head);

    usb_contoller_add_event_hander(event_handler);

    return 0;
}

void usbd_pipe_open(u8 index, const struct usb_pipe *pipe, usbstack_packet_handler_t packet_handler)
{
    struct usb_protocol_link *link;

    link = usb_link_for_index(index);

    struct usb_pipe *pipe_temp;

    pipe_temp = create_usb_pipe(link->phy);
    if (!pipe_temp) {
        debug_log("create usb pipe fail!\n");
        return -1;
    }
    list_add_tail(&pipe_temp->entry, &link->pipe_head);

    memcpy(pipe_temp, pipe, sizeof(struct usb_pipe));

    __usbd_drv->open(link->phy, pipe_temp);

    pipe_temp->handler = packet_handler;
}

static void usbd_pipe_send_packet_start()
{
    
}

void usbd_pipe_send_packet(struct usb_pipe *pipe, u8 *packet, u16 size)
{
    struct usb_tx *tx;
    u16 len, remain;

    pipe->data_len = size;
    pipe->wait_ok = 0;

    while (size) {
        len = (size > pipe->maxpktsize) ? pipe->maxpktsize : size;

        pkt = lbuf_alloc(pipe->lbuf, sizeof(*pkt) + pipe->maxpktsize)
        if (!pkt) {
            return -1;
        }

        pkt->size = len;
        memcpy(pkt->payload, packet, len);
        lbuf_push(pkt);
        size -= len;
        packet += len;
    }

    //start send
    pkt = lbuf_pop(pipe->lbuf);
    if (pkt) {
        __usbd_drv->send_packet(pipe, pkt->payload, pkt->size);
    }
}


void usbd_pipe_send_packet_wait(struct usb_pipe *pipe, u8 *packet, u16 size)
{
    usbd_pipe_send_packet(pipe, packet, size);

    while (!pipe->wait_ok) {
        thread_suspend(__this->protocol_thread, 0);
    }
}


void usbd_register_desc_device(tusb_descriptor_device_t *string)
{
    __this->app_usb_desc_device = string
}

void usbd_register_desc_configuration(tusb_descriptor_configuration_t *string)
{
    __this->app_usb_desc_configuration = string;
}

void usbd_register_desc_strings(tusb_descriptor_string_t *string)
{
    __this->app_usb_desc_string = string;
}

static void ucd_emit_event(uint8_t *event, uint16_t size, int dump)
{
    // dispatch to all event handlers
    linked_list_iterator_t it;
    linked_list_iterator_init(&it, &__this->event_handlers);
    while (linked_list_iterator_has_next(&it)) {
        usbstack_packet_callback_registration_t *entry = (usbstack_packet_callback_registration_t *) linked_list_iterator_next(&it);
        entry->callback(USB_EVENT_PACKET, event, size);
    }
}


/**
 * @brief ucd_add_event_hander : USB Controller Driver Event handlers register
 *
 * @param callback_handler : Event Receiver
 */
void usb_contoller_add_event_hander(usbstack_packet_callback_registration_t *callback_handler)
{
    linked_list_add_tail(&__this->event_handlers, (linked_item_t *) callback_handler);
}
