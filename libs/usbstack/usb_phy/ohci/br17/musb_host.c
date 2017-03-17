
static const struct usb_driver musbh_drv = {
    .open                   = musb
    .ioctrl                 = musb_ioctrl,

    .control_transfer       = musbh_control_transfer,
    .bulk_transfer          = musbh_bulk_transfer,
    .iso_transfer           = musbh_iso_transfer,
    .request_packet         = musbh_request_packet,

};
REGISTER_USB_HOST_OPERATION(musbh_drv);
