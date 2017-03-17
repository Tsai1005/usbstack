#include "usb/ch9.h"
#include "usb_interface.h"
#include "thread.h"
#include <linked_list.h>
#include "list.h"

/* struct usb_controller_handler { */
/* void (*rx_handler)(struct usb_pipe *, struct usb_rx *); */

/* void (*tx_handler)(struct usb_pipe*, struct usb_tx *); */
/* }; */

struct usb_controller_hdl {
    linked_list_t event_handlers;           //notify to upper layer

    /* struct usb_controller_hander *handler; */
    list_head endpoint_head;//endpoint list
};

static struct usb_controller_hdl hdl;

#define __this  (&hdl)




static void packet_handler(struct usb_pipe *pipe, struct usb_rx *pkt)
{
    if (__this->handler && __this->handler->rx_handler) {
        __this->handler->rx_handler(pipe, pkt);
    }
}



void usb_controller_init(struct usb_protocol_link *link, struct usb_phy_handler *handler)
{
    void *priv;

    link->priv = usb_drv->init(link->index, link->role);

    //binding protocol layer to driver layer
    usb_drv->register_packet_hander(link->priv, link, handler);

    return priv;
}

void usb_pipe_open(struct usb_protocol_link *link, const struct usb_pipe *pipe)
{
    usb_drv->open(link->priv, pipe);

    //binding protocol layer to driver layer
}

void usb_pipe_send_packet(struct usb_pipe *pipe, u8 *packet, u16 size)
{
    usb_drv->send_packet(pipe, packet, size);
}

void usb_pipe_request_packet(struct usb_pipe *pipe)
{
    usb_drv->request_packet()
}

void usb_pipe_upload_packet(void)
{
    usb_drv->upload_packet()
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

