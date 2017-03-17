#ifndef _USBSTACK_CONFIG_H_
#define _USBSTACK_CONFIG_H_

///<USB Stack Segment Location
#define AT_USB_PHY_CODE             AT(.usb_phy_code)

#define AT_USB_HOST_PROTOCOL_CODE   AT(.usb_host_protocol_code)

#define AT_USB_DEVICE_PROTOCOL_CODE AT(.usb_device_protocol_code)

//class
#define AT_USB_HOST_MSC_CODE        AT(.usb_host_msc_code)

#define AT_USB_HOST_SPK_CODE        AT(.usb_host_spk_code)

#define AT_USB_HOST_MIC_CODE        AT(.usb_host_mic_code)



// pipe handler
typedef void (*usbstack_packet_handler_t)(uint8_t type, uint8_t *packet, uint16_t size);

// packet callback supporting multiple registrations
typedef struct {
    linked_item_t    item;
    usbstack_packet_handler_t callback;
} usbstack_packet_callback_registration_t;

#endif
