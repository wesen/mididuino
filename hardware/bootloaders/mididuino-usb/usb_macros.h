/* USB access macros */

#ifndef USB_MACROS_H__
#define USB_MACROS_H__

#include <inttypes.h>

#include <avr/io.h>
#include "common.h"

/* endpoint numbers */

#define EP_CONTROL   0
#define EP_1         1
#define EP_2         2
#define EP_3         3
#define EP_4         4
#define EP_5         5
#define EP_6         6
#define EP_7         7

#define USB_EP_COUNT 9

/* usb endpoint masks */
#define USB_MSK_EP_DIR            0x7F 
#define USB_MSK_UADD              0x7F
#define USB_MSK_EPTYPE            0xC0
#define USB_MSK_EPSIZE            0x70
#define USB_MSK_EPBK              0x0C
#define USB_MSK_DTSEQ             0x0C
#define USB_MSK_NBUSYBK           0x03
#define USB_MSK_CURRBK            0x03
#define USB_MSK_DAT               0xFF 
#define USB_MSK_BYCTH             0x07 
#define USB_MSK_BYCTL             0xFF 
#define USB_MSK_EPINT             0x7F 
#define USB_MSK_HADDR             0xFF 

/* usb pipe masks */
#define USB_MSK_PNUM              0x07 
#define USB_MSK_PRST              0x7F 
#define USB_MSK_PTYPE             0xC0 
#define USB_MSK_PTOKEN            0x30
#define USB_MSK_PEPNUM            0x0F
#define USB_MSK_PSIZE             0x70 
#define USB_MSK_PBK               0x0C

/* usb endpoint types */
#define USB_EP_TYPE_CONTROL     0
#define USB_EP_TYPE_ISOCHRONOUS 1
#define USB_EP_TYPE_BULK        2
#define USB_EP_TYPE_INTERRUPT   3

/* usb endpoint directions */
#define USB_EP_DIR_OUT   0
#define USB_EP_DIR_IN    1

/* usb endpoint sizes */
#define USB_EP_SIZE_8             0
#define USB_EP_SIZE_16            1
#define USB_EP_SIZE_32            2
#define USB_EP_SIZE_64            3
#define USB_EP_SIZE_128           4
#define USB_EP_SIZE_256           5
#define USB_EP_SIZE_512           6
#define USB_EP_SIZE_1024          7

/* usb endpoint banks */
#define USB_EP_ONE_BANK  0
#define USB_EP_TWO_BANKS 1

/* usb endpoint nyet */
#define USB_EP_NYET_ENABLED    0
#define USB_EP_NYET_DISABLED   1

#define USB_AVR_EP_CONTROL_LENGTH   64

/** USB register macros */

/* UHWCON macros */
#define USB_ENABLE_REGULATOR()       SET_BIT  (UHWCON, UVREGE)
#define USB_DISABLE_REGULATOR()      CLEAR_BIT(UHWCON, UVREGE)

#define USB_DISABLE_UID_PIN()        CLEAR_BIT(UHWCON, UIDE)
#define USB_ENABLE_UID_PIN()         SET_BIT  (UHWCON, UIDE)

#define USB_FORCE_DEVICE_MODE()      (USB_DISABLE_UID_PIN(), SET_BIT(UHWCON, UIMOD))
#define USB_FORCE_HOST_MODE()        (USB_DISABLE_UID_PIN(), CLEAR_BIT(UHWCON, UIMOD))

#define USB_ENABLE_UVCON_PIN()       SET_BIT(UHWCON, UVCONE)

#define USB_SET_FULL_SPEED_MODE()    SET_BIT  (UHWCON, UDSS)
#define USB_SET_LOW_SPEED_MODE()     CLEAR_BIT(UHWCON, UDSS)

/* USBCON macros */
#define USB_ENABLE()                 (USBCON |= ((1 << USBE) | (1 << OTGPADE)))
#define USB_DISABLE()                (USBCON &= ~((1 << USBE) | (1 << OTGPADE)))

#define USB_SELECT_DEVICE()          CLEAR_BIT(USBCON, HOST)
#define USB_SELECT_HOST()            SET_BIT(USBCON, HOST)

#define USB_FREEZE_CLOCK()           SET_BIT(USBCON, FRZCLK)
#define USB_UNFREEZE_CLOCK()         CLEAR_BIT(USBCON, FRZCLK)

#define USB_ENABLE_ID_INTERRUPT()     SET_BIT(USBCON, IDTE)
#define USB_DISABLE_ID_INTERRUPT()    CLEAR_BIT(USBCON, IDTE)
#define USB_IS_ID_INTERRUPT_ENABLED() IS_BIT_SET(USBCON, IDTE)

#define USB_ENABLE_VBUS_INTERRUPT()  SET_BIT(USBCON, VBUSTE)
#define USB_DISABLE_VBUS_INTERRUPT() CLEAR_BIT(USBCON, VBUSTE)
#define USB_IS_VBUS_INTERRUPT_ENABLED() IS_BIT_SET(USBCON, VBUSTE)

#define USB_IS_ENABLED()             IS_BIT_SET(USBCON, USBE)
#define USB_IS_HOST_ENABLED()        IS_BIT_SET(USBCON, HOST)
#define USB_IS_CLOCK_FROZEN()        IS_BIT_SET(USBCON, FRZCLK)

/* USBSTA macros */
#define USB_IS_VBUS_HIGH()           IS_BIT_SET(USBSTA, VBUS)
#define USB_IS_HIGH_SPEED()          IS_BIT_SET(USBSTA, SPEED)
#define USB_IS_ID_DEVICE()           IS_BIT_SET(USBSTA, ID)

/* USBINT macros */
#define USB_ACK_ID_TRANSITION()      (USBINT = ~(1 << IDTI))
#define USB_ACK_VBUS_TRANSITION()    (USBINT = ~(1 << VBUSTI))
#define USB_IS_ID_TRANSITION()       IS_BIT_SET(USBINT, IDTI)
#define USB_IS_VBUS_TRANSITION()     IS_BIT_SET(USBINT, VBUSTI)

/* UDPADDH, UEDATX macros */
#define USB_ENTER_DPRAM_MODE()       (UDPADDH = (1 << DPACC))
#define USB_LEAVE_DPRAM_MODE()       (UDPADDH = ~(1 << DPACC))
#define USB_SET_DPRAM_ADDR(addr)     (UDPADDH = ((1 << DPACC) + ((uint16_t)addr >> 8)), \
                                      UDPADDL = ((uint8_t)addr & 0xFF))
#define USB_WRITE_DPRAM_BYTE(val)    (UEDATX = val)
#define USB_READ_DPRAM_BYTE()        (UEDATX)

/** OTG register macros */

/* OTGCON macros */
#define USB_ENABLE_VBUS()            SET_BIT  (OTGCON, VBUSREQ)
#define USB_DISABLE_VBUS()           CLEAR_BIT(OTGCON, VBUSREQ)

#define USB_DEVICE_INIT_HNP()        SET_BIT  (OTGCON, HNPREQ)
#define USB_HOST_ACCEPT_HNP()        SET_BIT  (OTGCON, HNPREQ)
#define USB_HOST_REJECT_HNP()        CLEAR_BIT(OTGCON, HNPREQ)

#define USB_DEVICE_INIT_SRP()        SET_BIT  (OTGCON, SRPREQ)
#define USB_SELECT_VBUS_SRP_METHOD() SET_BIT  (OTGCON, SRPSEL)
#define USB_SELECT_DATA_SRP_METHOD() CLEAR_BIT(OTGCON, SRPSEL)

#define USB_ENABLE_VBUS_HW_CONTROL() CLEAR_BIT(OTGCON, VBUSHWC)
#define USB_DISABE_VBUS_HW_CONTROL() SET_BIT  (OTGCON, VBUSHWC)

#define USB_IS_VBUS_ENABLED()        IS_BIT_SET(OTGCON, VBUSREQ)
#define USB_IS_HNP()                 IS_BIT_SET(OTGCON, HNPREQ)
#define USB_IS_DEVICE_SRP()          IS_BIT_SET(OTGCON, SRPREQ)

/* OTGIEN macros */
#define USB_ENABLE_SUSPEND_TIMEOUT_INTERRUPT()    SET_BIT   (OTGIEN, STOE)
#define USB_DISBALE_SUSPEND_TIMEOUT_INTERRUPT()   CLEAR_BIT (OTGIEN, STOE)
#define USB_ACK_SUSPEND_TIMEOUT_INTERRUPT()       CLEAR_BIT (OTGINT, STOI)
#define USB_IS_SUSPEND_TIMEOUT_INTERRUPT()        IS_BIT_SET(OTGINT, STOI)

#define USB_ENABLE_HNP_ERROR_INTERRUPT()          SET_BIT   (OTGIEN, HNPERRE)
#define USB_DISABLE_HNP_ERROR_INTERRPUPT()        CLEAR_BIT (OTGIEN, HNPERRE)
#define USB_ACK_HNP_ERROR_INTERRUPT()             CLEAR_BIT (OTGINT, HNPERRI)
#define USB_IS_HNP_ERROR_INTERRUPT()              IS_BIT_SET(OTGINT, HNPERRI)

#define USB_ENABLE_ROLE_EXCHANGE_INTERRUPT()      SET_BIT   (OTGIEN, ROLEEXE)
#define USB_DISABLE_ROLE_EXCHANGE_INTERRUPT()     CLEAR_BIT (OTGIEN, ROLEEXE)
#define USB_ACK_ROLE_EXCHANGE_INTERRUPT()         CLEAR_BIT (OTGINT, ROLEEXI)
#define USB_IS_ROLE_EXCHANGE_INTERRUPT()          IS_BIT_SET(OTGINT, ROLEEXI)

#define USB_ENABLE_BCONNECTION_ERROR_INTERRUPT()  SET_BIT   (OTGIEN, BCERRE)
#define USB_DISABLE_BCONNECTION_ERROR_INTERRUPT() CLEAR_BIT (OTGIEN, BCERRE)
#define USB_ACK_BCONNECTION_ERROR_INTERRUPT()     CLEAR_BIT (OTGINT, BCERRI)
#define USB_IS_BCONNECTION_ERROR_INTERRUPT()      IS_BIT_SET(OTGINT, BCERRI)

#define USB_ENABLE_VBUS_ERROR_INTERRUPT()         SET_BIT   (OTGIEN, VBERRE)
#define USB_DISABLE_VBUS_ERROR_INtERRUPT()        CLEAR_BIT (OTGIEN, VBERRE)
#define USB_ACK_VBUS_ERROR_INTERRUPT()            CLEAR_BIT (OTGINT, VBERRI)
#define USB_IS_VBUS_ERROR_INTERRUPT()             IS_BIT_SET(OTGINT, VBERRI)

#define USB_ENABLE_SRP_INTERRUPT()                SET_BIT   (OTGIEN, SRPE)
#define USB_DISABLE_SRP_INTERRUPT()               CLEAR_BIT (OTGIEN, SRPE)
#define USB_ACK_SRP_INTERRUPT()                   CLEAR_BIT (OTGINT, SRPI)
#define USB_IS_SRP_INTERRUPT()                    IS_BIT_SET(OTGINT, SRPI)

/** USB device management macros */

/* UDCON macros */
#define USB_INITIATE_REMOTE_WAKE_UP()       SET_BIT(UDCON, RMWKUP)
#define USB_IS_PENDING_REMOTE_WAKE_UP()     IS_BIT_SET(UDCON, RMWKUP)

#define USB_DETACH()                        SET_BIT  (UDCON, DETACH)
#define USB_ATTACH()                        CLEAR_BIT(UDCON, DETACH)
#define USB_IS_DETACHED()                   IS_BIT_SET(UDCON, DETACH)

/* UDINT macros */

#define USB_GET_DEVICE_INTERRUPT()             (UDINT & (1 << UDIEN))
#define USB_ACKK_ALL_DEVICE_INTERRUPT()        (UDINT = ~(1 << UDIEN))

#define USB_ACK_REMOTE_WAKE_UP_START()         (UDINT = ~(1 << UPRSMI))
#define USB_IS_REMOTE_WAKE_UP_START()          IS_BIT_SET(UDINT, UPRSMI)
#define USB_ENABLE_REMOTE_WAKE_UP_INTERRUPT()  SET_BIT   (UDIEN, UPRSME)
#define USB_DISABLE_REMOTE_WAKE_UP_INTERRUPT() CLEAR_BIT (UDIEN, UPRSME)
#define USB_IS_REMOTE_WAKE_UP_INTERRUPT_ENABLED() IS_BIT_SET(UDIEN, UPRSME)

#define USB_ACK_RESUME()                       (UDINT = ~(1 << EORSMI))
#define USB_IS_RESUME()                        IS_BIT_SET(UDINT, EORSMI)
#define USB_ENABLE_RESUME_INTERRUPT()          SET_BIT   (UDIEN, EORSME)
#define USB_DISABLE_RESUME_INTERRUPT()         CLEAR_BIT (UDIEN, EORSME)
#define USB_IS_RESUME_INTERRUPT_ENABLED()      IS_BIT_SET(UDIEN, EORSME)

#define USB_ACK_WAKE_UP()                      (UDINT = ~(1 << WAKEUPI))
#define USB_IS_WAKE_UP()                       IS_BIT_SET(UDINT, WAKEUPI)
#define USB_ENABLE_WAKE_UP_INTERRUPT()         SET_BIT   (UDIEN, WAKEUPE)
#define USB_DISABLE_WAKE_UP_INTERRUPT()        CLEAR_BIT (UDIEN, WAKEUPE)
#define USB_IS_WAKE_UP_INTERRUPT_ENABLED()     IS_BIT_SET(UDIEN, WAKEUPE)

#define USB_ACK_RESET()                        (UDINT = ~(1 << EORSTI))
#define USB_IS_RESET()                         IS_BIT_SET(UDINT, EORSTI)
#define USB_ENABLE_RESET_INTERRUPT()           SET_BIT   (UDIEN, EORSTE)
#define USB_DISABLE_RESET_INTERRUPT()          CLEAR_BIT (UDIEN, EORSTE)
#define USB_IS_RESET_INTERRUPT_ENABLED()       IS_BIT_SET(UDIEN, EORSTE)

#define USB_ACK_SOF()                          (UDINT = ~(1 << SOFI))
#define USB_IS_SOF()                           IS_BIT_SET(UDINT, SOFI)
#define USB_ENABLE_SOF_INTERRUPT()             SET_BIT   (UDIEN, SOFE)
#define USB_DISABLE_SOF_INTERRUPT()            CLEAR_BIT (UDIEN, SOFE)
#define USB_IS_SOF_INTERRUPT_ENABLED()         IS_BIT_SET(UDIEN, SOFE)

#define USB_ACK_SUSPEND()                      (UDINT = ~(1 << SUSPI))
#define USB_IS_SUSPEND()                       IS_BIT_SET(UDINT, SUSPI)
#define USB_ENABLE_SUSPEND_INTERRUPT()         SET_BIT   (UDIEN, SUSPE)
#define USB_DISABLE_SUSPEND_INTERRUPT()        CLEAR_BIT (UDIEN, SUSPE)
#define USB_IS_SUSPEND_INTERRUPT_ENABLED()     IS_BIT_SET(UDIEN, SUSPE)

/* UDADDR macros */

#define USB_ENABLE_ADDRESS()                   SET_BIT  (UDADDR, ADDEN)
#define USB_DISABLE_ADDRESS()                  CLEAR_BIT(UDADDR, ADDEN)
#define USB_CONFIGURE_ADDRESS(addr)            (UDADDR = (UDADDR & (1 << ADDEN)) \
                                                       | ((uint8_t)(addr) & USB_MSK_UADD))

/* UDFNUM macros */

#define USB_FRAME_NUMBER()                     (uint16_t)((((uint16_t)UDFNUMH) << 8) \
                                                         | ((uint16_t)UDFNUML))
#define USB_MICRO_FRAME_NUMBER()               (uint8_t)(UDMFN & (1 << MFNUM))
#define USB_IS_FRAME_NUMBER_CRC_ERROR()        IS_BIT_SET(UDMFN, FNCERR)

/** Endpoint management macros */

#define USB_SELECT_ENDPOINT(ep)                (UENUM = (uint8_t)ep)
#define USB_RESET_ENDPOINT(ep)                 (UERST = 1 << (uint8_t)ep, UERST = 0)

/* UECONX macros */
#define USB_ENABLE_ENDPOINT()                  SET_BIT   (UECONX, EPEN)
#define USB_DISABLE_ENDPOINT()                 CLEAR_BIT (UECONX, EPEN)
#define USB_IS_ENDPOINT_ENABLED()              IS_BIT_SET(UECONX, EPEN)

#define USB_ENABLE_STALL_HANDSHAKE()           SET_BIT   (UECONX, STALLRQ)
#define USB_DISABLE_STALL_HANDSHAKE()          SET_BIT (UECONX, STALLRQC)
#define USB_IS_ENDPOINT_STALL_REQUESTED()      IS_BIT_SET(UECONX, STALLRQ)

#define USB_RESET_DATA_TOGGLE()                SET_BIT(UECONX, RSTDT)
#define USB_SELECT_EPNUM_FOR_CPU()             CLEAR_BIT(UECONX, EPNUMS)

/* usb configuration build macros */
#define USB_EP_CONFIG0(type, dir, nyet)  (((type) << 6) | ((nyet) << 1) | (dir))
#define USB_EP_CONFIG1(size, bank)       (((size) << 4) | ((bank) << 2))

#define USB_CONFIGURE_ENDPOINT(ep, type, dir, size, bank, nyet) \
   (USB_SELECT_ENDPOINT(ep),                                   \
   usb_config_ep(USB_EP_CONFIG0((type), (dir), (nyet)),       \
                 USB_EP_CONFIG1((size), (bank))))

/* UECFG0X macros */

#define USB_CONFIGURE_ENDPOINT_TYPE(type)       (UECFG0X = (UECFG0X & ~(USB_MSK_EPTYPE)) \
                                                         | ((uint8_t)type << 6))
#define USB_CONFIGURE_ENDPOINT_DIRECTION(dir)   (UECFG0X = (UECFG0X & ~(1 << EPDIR)) \
                                                         | ((uint8_t)dir))

#define USB_ENABLE_ISO_AUTOSWITCH()             SET_BIT  (UECFG0X, ISOSW)
#define USB_DISABLE_ISO_AUTOSWITCH()            CLEAR_BIT(UECFG0X, ISOSW)

#define USB_ENABLE_AUTOSWITCH()                 SET_BIT  (UECFG0X, AUTOSW)
#define USB_DISABLE_AUTOSWITCH()                CLEAR_BIT(UECFG0X, AUTOSW)

#define USB_ENABLE_NYET()                       CLEAR_BIT(UECFG0X, NYETDIS)
#define USB_DISABLE_NYET()                      SET_BIT  (UECFG0X, NYETDIS)

/* UECFG1X macros */

#define USB_CONFIGURE_ENDPOINT_SIZE(size)      (UECFG1X = (UECFG1X & ~USB_MSK_EPSIZE) \
                                                        | ((uint8_t)size << 4))

#define USB_CONFIGURE_ENDPOINT_BANK(bank)      (UECFG1X = (UECFG1X & ~USB_MSK_EPBK) \
                                                        | ((uint8_t)bank << 2))

#define USB_ALLOCATE_MEMORY()                  SET_BIT  (UECFG1X, ALLOC)
#define USB_UNALLOCATE_MEMORY()                CLEAR_BIT(UECFG1X, ALLOC)

/* UESTA0X macros */

#define USB_ACK_OVERFLOW_INTERRUPT()           CLEAR_BIT (UESTA0X, OVERFI)
#define USB_IS_OVERFLOW()                      IS_BIT_SET(UESTA0X, OVERFI)

#define USB_ACK_UNDERFLOW_INTERRUPT()          CLEAR_BIT (UESTA0X, UNDERFI)
#define USB_IS_UNDERFLOW()                     IS_BIT_SET(UESTA0X, UNDERFI)

#define USB_ACK_ZLP()                          CLEAR_BIT (UESTA0X, ZLPSEEN)
#define USB_IS_ZLP()                           IS_BIT_SET(UESTA0X, ZLPSEEN)

#define USB_NB_BUSY_BANK()                     (UESTA0X & USB_MSK_NBUSYBK)
#define USB_IS_ONE_BANK_BUSY()                 IS_BIT_CLEAR(UESTA0X, USB_MSK_NBUSYBK)

#define USB_IS_ENDPOINT_CONFIGURED()           IS_BIT_SET(UESTA0X, CFGOK)

/* UESTA1X macros */

#define USB_CONTROL_DIRECTION()                ((UESTA1X & (1 << CTRLDIR)) >> 2)
#define USB_CURRENT_BANK()                     (UESTA1X & USB_MSK_CURRBK)

/* UEINTX macros */

#define USB_ACK_FIFOCON()                      CLEAR_BIT(UEINTX, FIFOCON)
#define USB_SEND_IN()                          CLEAR_BIT(UEINTX, FIFOCON)
#define USB_FREE_OUT_BANK()                    CLEAR_BIT(UEINTX, FIFOCON)

#define USB_ACK_NAK_IN()                       CLEAR_BIT(UEINTX, NAKINI)
#define USB_IS_NAK_IN_SENT()                   IS_BIT_SET(UEINTX, NAKINI)

#define USB_ACK_NAK_OUT()                      CLEAR_BIT(UEINTX, NAKOUTI)
#define USB_IS_NAK_OUT_SENT()                  IS_BIT_SET(UEINTX, NAKOUTI)

#define USB_ACK_RECEIVE_SETUP()                CLEAR_BIT (UEINTX, RXSTPI)
#define USB_IS_RECEIVE_SETUP()                 IS_BIT_SET(UEINTX, RXSTPI)

#define USB_CLEAR_RECEIVE_OUT()                CLEAR_BIT(UEINTX, RXOUTI)
#define USB_ACK_RECEIVE_OUT()                  (CLEAR_BIT(UEINTX, RXOUTI), USB_ACK_FIFOCON())
#define USB_IS_RECEIVE_OUT()                   IS_BIT_SET(UEINTX, RXOUTI)
#define USB_KILL_LAST_IN_BANK()                SET_BIT(UEINTX, RXOUTI)

#define USB_ACK_IN_READY()                     (CLEAR_BIT(UEINTX, TXINI), USB_ACK_FIFOCON())
#define USB_IS_IN_READY()                      IS_BIT_SET(UEINTX, TXINI)
#define USB_SEND_CONTROL_IN()                  CLEAR_BIT (UEINTX, TXINI)
#define USB_IS_READ_CONTROL_ENABLED()          IS_BIT_SET(UEINTX, TXINI)

#define USB_ACK_STALLED()                      (USB_MSK_STALLEDI = 0) /* XXX ??? */

#define USB_IS_READ_ENABLED()                  IS_BIT_SET(UEINTX, RWAL)
#define USB_IS_WRITE_ENABLED()                 IS_BIT_SET(UEINTX, RWAL)

#define USB_ACK_CONTROL_OUT()                  CLEAR_BIT(UEINTX, RXOUTI)

/* UEIENX macros */

#define USB_ENABLE_FLOW_ERROR_INTERRUPT()      SET_BIT  (UEIENX, FLERRE)
#define USB_DISABLE_FLOW_ERROR_INTERRUPT()     CLEAR_BIT(UEIENX, FLERRE)

#define USB_ENABLE_NAK_IN_INTERRUPT()          SET_BIT  (UEIENX, NAKINE)
#define USB_DISABLE_NAK_IN_INTERRUPT()         CLEAR_BIT(UEIENX, NAKINE)

#define USB_ENABLE_NAK_OUT_INTERRUPT()         SET_BIT  (UEIENX, NAKOUTE)
#define USB_DISABLE_NAK_OUT_INTERRUPT()        CLEAR_BIT(UEIENX, NAKOUTE)

#define USB_ENABLE_RECEIVE_SETUP_INTERRUPT()   SET_BIT  (UEIENX, RXSTPE)
#define USB_DISABLE_RECEIVE_SETUP_INTERRUPT()  CLEAR_BIT(UEIENX, RXSTPE)

#define USB_ENABLE_RECEIVE_OUT_INTERRUPT()     SET_BIT  (UEIENX, RXOUTE)
#define USB_DISABLE_RECEIVE_OUT_INTERRUPT()    CLEAR_BIT(UEIENX, RXOUTE)

#define USB_ENABLE_STALLED_INTERRUPT()         SET_BIT  (UEIENX, STALLEDE)
#define USB_DISABLE_STALLED_INTERRUPT()        CLEAR_BIT(UEIENX, STALLEDE)

#define USB_ENABLE_IN_READY_INTERRUPT()        SET_BIT  (UEIENX, TXIN)
#define USB_DISABLE_IN_READY_INTERRUPT()       CLEAR_BIT(UEIENX, TXIN)

#define USB_READ_BYTE()                        (UEDATX)
/* USB is little endian */
#define USB_READ_WORD()                        (((uint16_t)UEDATX) | (uint16_t)(UEDATX << 8))
#define USB_WRITE_BYTE(byte)                   (UEDATX = (uint8_t)byte)

#define USB_BYTE_COUNTER()                     ((((uint16_t)UEBCHX) << 8) | (UEBCLX))
#define USB_BYTE_COUNTER_8()                   ((uint8_t)UEBCLX)

#define USB_INTERRUPT_FLAGS()                  (UEINT != 0x00)
#define USB_IS_ENDPOINT_EVENT()                (USB_INTERRUPT_FLAGS())

/* advanced macro */
#define USB_SELECT_EP_FOR_CPU(ep)              (USB_SELECT_EP_FOR_CPU(), USB_SELECT_ENDPOINT(ep))

/** USB host management macros */

#define USB_HOST_ENABLE()                 SET_BIT(USBCON, HOST)

#define USB_HOST_ENABLE_SOF()             SET_BIT  (UHCON, SOFEN)
#define USB_HOST_DISABLE_SOF()            CLEAR_BIT(UHCON, SOFEN)

#define USB_HOST_SEND_RESET()             SET_BIT   (UHCON, RESET)
#define USB_HOS_IS_RESET()                IS_BIT_SET(UHCON, RESET)

#define USB_HOST_SEND_RESUME()            SET_BIT   (UHCON, RESUME)
#define USB_HOST_IS_RESUME()              IS_BIT_SET(UHCON, RESUME)

#define USB_HOST_ACK_SOF()                CLEAR_BIT (UHINT, HSOFI)
#define USB_HOST_IS_SOF()                 IS_BIT_SET(UHINT, HSOFI)

/* pipe management macros */

#define USB_HOST_SELECT_PIPE(p)           (UPNUM = (uint8_t)p)

#define USB_HOST_ENABLE_PIPE()            SET_BIT  (UPCONX, PEN)
#define USB_HOST_DISABLE_PIPE()           CLEAR_BIT(UPCONX, PEN)

#define USB_HOST_SET_TOKEN_SETUP()        (UPCFG0X = UPCFG0X & ~(1 << TOKEN_SETUP))
#define USB_HOST_SET_TOKEN_IN()           (UPCFG0X = (UPCFG0X ~ ~(1 << TOKEN_SETUP)) \
                                                   | (1 << TOKEN_IN))
#define USB_HOST_SET_TOKEN_OUT()          (UPCFG0X = (UPCFG0X ~ ~(1 << TOKEN_SETUP)) \
                                                   | (1 << TOKEN_OUT))

#define USB_HOST_SET_INTERRUPT_FREQUENCY(frq)   (UPCFG2X = (uint8_t)frq)

#define USB_HOST_IS_PIPE_CONFIGURED()     IS_BIT_SET(UPSTAX, CFGOK)

#define USB_HOST_WRITE_BYTE(dat)          (UPDATX = (frq))
#define USB_HOST_READ_BYTE()              (UPDATX)

#define USB_HOST_UNFREEZE_PIPE()          SET_BIT  (UPCONX, PFREEZE)
#define USB_HOST_FREEZE_PIPE()            CLEAR_BIT(UPCONX, PFREEZE)

#define USB_HOST_IS_SETUP_SENT()          (TXSTPI)
#define USB_HOST_IS_CONTROL_IN_RECEIVED() (FIFOCON)
#define USB_HOST_IS_CONTROL_OUT_SEND()    (FIFOCON)
#define USB_HOST_IS_STALL()               (RXSTALLI)
#define USB_HOST_IS_ERROR()               (PERRI)

#define USB_HOST_SEND_SETUP()             (FIFOCON = 0)
#define USB_HOST_SEND_CONTROL_IN()        (FIFOCON = 0)
#define USB_HOST_SEND_CONTROL_OUT()       (FIFOCON = 0)
#define USB_HOST_ACK_CONTROL_OUT()        (TXOUTI = 0)
#define USB_HOST_ACK_CONTORL_IN()         (RXINI = 0)

#define USB_HOST_STANDARD_IN_MODE()       CLEAR_BIT(UPCONX, INMODE)
#define USB_HOST_CONTINUOUS_IN_MODE()     SET_BIT  (UPCONX, INMODE)

#define USB_HOST_IN_REQUEST_NUMBER(num)   (UPINRQX = (uint8_t)(num))

#define USB_HOST_DATA_LENGTH_U8()         (UPBCLX)
#define USB_HOST_DATA_LENGtH_U16()        (((uint16_t)UPBCHX << 8) | UPBCLX)

#define USB_HOST_GET_PIPE_LENGTH()        ((uint16_t)0x08 << ((UPCFG1X & (1 << PSIZE)) >> 4))

#define USB_HOST_ERROR_STATUS()           (UPERRX & (1 << ERROR))
#define USB_HOST_ACK_ALL_ERRORS()         (UPERRX = 0)


#endif /* USB_MACROS_H__ */
