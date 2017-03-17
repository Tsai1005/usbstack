#ifndef _USB_INTERFACE_H_
#define _USB_INTERFACE_H_

#include "cpu.h"

struct usb_protocol_link {
    list_head entry;//link list
    list_head pipe_head;//link list

    struct list_head descriptor_head;       //config descriptor list head
    linked_list_t event_handlers;           //notify to upper layer

    //for app descriptor
    tusb_descriptor_device_t        *app_usb_desc_device;
    tusb_descriptor_configuration_t *app_usb_desc_configuration;
    tusb_descriptor_string_t        *app_usb_desc_string;

    u8 role;
    u8 state;
    u8 index;

    void *phy;  //binding with musb hw
};

//for protocol use
struct usb_pipe {
    list_head entry;

    //by upper layer
    volatile u8 send_ok;
    u8 direction;
    u8 state;               //IDLE/BUSY/HALT
    u8 type;   //CONTROL/BULK/ISO/INTERRUPT
    u16 endpoint_size;
    usbstack_packet_handler_t handler;   //notify to upper layer

    //by lower layer
    //privates
    void *endpoint; //binding with musb hw
    void *phy;  //binding with musb hw
    u8 endpoint_number;
};

static usb_rx{
    u16 size;
    u8 payload[0];
};

static usb_tx{
    u16 size;
    u8 payload[0];
};


// #define USB_ENDPOINT_XFER_CONTROL	0
// #define USB_ENDPOINT_XFER_ISOC		1
// #define USB_ENDPOINT_XFER_BULK		2
// #define USB_ENDPOINT_XFER_INT		3
struct usb_phy_handler {
    void (*ep0_handler)(void *, struct usb_rx *);
    void (*rx_handler)(void *, struct usb_rx *);
    void (*tx_handler)(void *, struct usb_tx *);
};


//USB Driver Interface
struct usb_driver {
    void (*init)(void *, u8);          //controller initial, role decided

    void (*close)(void *);

    void (*open)(void *, void *);   //pipe open, data type decided

    void (*control_transfer)(void *,);
    void (*bulk_transfer)(void *,);
    void (*isoc_transfer)(void *,);
    void (*int_transfer)(void *,);

    void (*ioctrl)(void *, int, ...);
}



//Full Speed USB Driver(OHCI) instance
#define REGISTER_USB_OPERATION(drv) \
	const struct usb_driver *__usb_drv \
            SEC_USED(.usb_drv) = &drv

extern const struct usb_driver *__usb_drv;

#define REGISTER_USB_DEVICE_OPERATION(drv) \
	const struct usb_driver *__usbd_drv \
            SEC_USED(.usb_drv) = &drv

extern const struct usb_driver *__usbd_drv;

#define REGISTER_USB_HOST_OPERATION(drv) \
	const struct usb_driver *__usbh_drv \
            SEC_USED(.usb_drv) = &drv

extern const struct usb_driver *__usbh_drv;

#endif
