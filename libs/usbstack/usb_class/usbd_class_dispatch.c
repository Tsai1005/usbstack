#include <linked_list.h>


struct usb_class_hdl {
    linked_list_t pipe_handlers
};

static struct usb_class_hdl hdl;

#define __this  (&hdl)

static void rx_dispatch_pipe_process(struct usb_pipe *pipe, const struct usb_rx *pkt)
{
    linked_list_iterator_t it;
    linked_list_iterator_init(&it, &__this->pipe_handlers);
    while (linked_list_iterator_has_next(&it)) {
        usbstack_packet_callback_registration_t *entry = (usbstack_packet_callback_registration_t *) linked_list_iterator_next(&it);
        entry->callback(HCI_EVENT_PACKET, event, size);
    }
}

static void rx_class_pipe_handler(u8 pipe_index, u8 *packet)
{

}

void usbd_class_register_pipe_hander(usbstack_packet_callback_registration_t *callback_handler)
{
    linked_list_add_tail(&__this->pipe_handlers, (linked_item_t *) callback_handler);
}

static void __config_descriptor_parser(u8 *descriptor)
{
    //binding class & endpoint
}



/**
 * @brief usbd_config_descriptor_register : USB Class register the config descriptor
 *
 * @param descriptor
 */
void usbd_config_descriptor_register(u8 *descriptor)
{
    __config_descriptor_parser(descriptor);
}
