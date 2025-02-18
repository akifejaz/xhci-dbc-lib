#ifndef __XHCI_DBC_H__
#define __XHCI_DBC_H__

#include "pci.h"
#include "mmio.h"
#include "mem.h"

#define XHCI_DBC_CLASS_TYPE_SERIAL_BUS_CONTROLLER               0x0C
#define XHCI_DBC_SUBCLASS_TYPE_USB_CONTROLLER                   3
#define XHCI_DBC_PROGRAMMING_INTERFACE_XHCI_CONTROLLER          0x30

#define XHCI_HCCPARAMS1_OFFSET                                  0x10

#define XHCI_DBC_DCERSTSZ_OFFSET                                8
#define XHCI_DBC_DCERSTBA_OFFSET                                0x10
#define XHCI_DBC_DCERDP_OFFSET                                  0x18
#define XHCI_DBC_DCCP_OFFSET                                    0x30
#define XHCI_DBC_DCCTRL_OFFSET                                  0x20
#define XHCI_DBC_DCST_OFFSET                                    0x24
#define XHCI_DBC_DCPORTSC_OFFSET                                0x28
#define XHCI_DBC_DCDDI1_OFFSET                                  0x38
#define XHCI_DBC_DCDDI2_OFFSET                                  0x3C

#define XHCI_DBC_CAP_ID                                         10

#define ERST_ENTRY_BASE_ADDRESS_LOW_MASK                        0xFFFFFFC0
#define ERST_ENTRY_BASE_ADDRESS_HIGH_MASK                       0xFFFFFFFF
#define EVENT_RING_SEGMENT_MIN_TRB_COUNT                        16

#define DBCIC_STRING_DESCRIPTOR_BASE_ADDRESS_LOW_MASK           0xFFFFFFFE
#define DBCIC_STRING_DESCRIPTOR_BASE_ADDRESS_HIGH_MASK          0xFFFFFFFF

#define USB_STRING_DESCRIPTOR_TYPE                              3
#define USB_STRING_DESCRIPTOR_LANGUAGE_ID_EN_US                 0x0409

#define DBC_ENDPOINT_CONTEXT_TYPE_OUT_BULK                      2
#define DBC_ENDPOINT_CONTEXT_TYPE_IN_BULK                       6
#define DBC_ENDPOINT_CONTEXT_DEFAULT_MAX_PACKET_SIZE            1024

#define DBC_ENDPOINT_CONTEXT_DEFAULT_AVERAGE_TRB_LENGTH         1024    // this needs to be adjusted based on how big the buffers transferred from your debug target to the debug host are!

#define DBC_ENDPOINT_CONTEXT_MIN_TRB_COUNT                      16      // this might be an arbitrary value

#define TRB_TYPE_LINK                                           6

#define LINK_TRB_RING_SEGMENT_POINTER_LOW_MASK                  0xFFFFFFF0
#define LINK_TRB_RING_SEGMENT_POINTER_HIGH_MASK                 0xFFFFFFFF

#define ENDPOINT_CONTEXT_TR_DEQUEUE_POINTER_LOW_MASK            0xFFFFFFF0
#define ENDPOINT_CONTEXT_TR_DEQUEUE_POINTER_HIGH_MASK           0xFFFFFFFF

#define EVENT_RING_SEGMENT_TABLE_BASE_ADDRESS_REGISTER_MASK     0xFFFFFFFFFFFFFFF0
#define EVENT_RING_DEQUEUE_POINTER_REGISTER_MASK                0xFFFFFFFFFFFFFFF0
#define DEBUG_CAPABILITY_CONTEXT_POINTER_REGISTER_MASK          0xFFFFFFFFFFFFFFF0

#define DBC_DDI1_PROTOCOL                                       0
#define DBC_DDI1_VENDOR_ID                                      0x1D6B
#define DBC_DDI2_PRODUCT_ID                                     0x0010
#define DBC_DDI2_DEVICE_REVISION                                0

typedef struct _XHC_INFORMATION
{
    uint64_t mmio_phys_base;            // xHC main base address
    uint32_t mmio_length;               // xHC total length
    uint64_t ext_cap_struct_base;       // xHC's xECP base
    uint64_t debug_cap_struct_base;     // xHC's DbC base
} XHC_INFO;

typedef union _HCCPARAMS1_INFORMATION
{
    struct
    {
        uint32_t ac64         : 1;  // 0
        uint32_t bnc          : 1;  // 1
        uint32_t csz          : 1;  // 2
        uint32_t ppc          : 1;  // 3
        uint32_t pind         : 1;  // 4
        uint32_t lhrc         : 1;  // 5
        uint32_t ltc          : 1;  // 6
        uint32_t nss          : 1;  // 7
        uint32_t pae          : 1;  // 8
        uint32_t spc          : 1;  // 9
        uint32_t sec          : 1;  // 10
        uint32_t cfc          : 1;  // 11
        uint32_t max_psa_size : 4;  // 15:12
        uint32_t xecp         : 16; // 31:16
    } fields;
    struct
    {
        uint32_t addressing_cap_64_bit           : 1;  // 0
        uint32_t bw_negotiation_cap              : 1;  // 1
        uint32_t context_size                    : 1;  // 2
        uint32_t port_power_ctrl                 : 1;  // 3
        uint32_t power_indicators                : 1;  // 4
        uint32_t light_hc_reset_cap              : 1;  // 5
        uint32_t latency_tolerance_messaging_cap : 1;  // 6
        uint32_t no_secondary_sid_support        : 1;  // 7
        uint32_t parse_all_event_data            : 1;  // 8
        uint32_t stopped_short_packet_cap        : 1;  // 9
        uint32_t stopped_edtla_cap               : 1;  // 10
        uint32_t contiguous_frame_id_cap         : 1;  // 11
        uint32_t max_primary_stream_array_size   : 4;  // 15:12
        uint32_t ext_cap_pointer                 : 16; // 31:16
    } named_fields;
    uint32_t value;
} HCCPARAMS1_INFO;

typedef union _XHCI_EXTENDED_CAPABILITY_POINTER_REGISTER
{
    struct
    {
        uint32_t cap_id              : 8;  // 7:0
        uint32_t next_ext_cap_offset : 8;  // 15:8
        uint32_t _reserved           : 16; // 31:16
    } fields;
    uint32_t value;
} XHCI_EXT_CAP_PTR_REG;

#pragma pack(push, 1)
typedef struct _EVENT_RING_SEGMENT_TABLE_ENTRY
{
    // uint64_t _reserved_0                    : 6;  // 5:0
    uint64_t ring_segment_base_address_low  : 32; // 31:0       // 26; // 31:6
    uint64_t ring_segment_base_address_high : 32; // 31:0
    uint64_t ring_segment_size              : 16; // 15:0
    uint64_t _reserved_1                    : 16; // 31:16
    uint64_t _reserved_2                    : 32; // 31:0
} ERST_ENTRY;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _TRANSFER_REQUEST_BLOCK
{
    uint64_t data_buffer_pointer;       // points to your data
    uint32_t status;                    // actually length
    uint32_t control;                   // 3.2.7.1 + 6.4.1
} TRB;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _NORMAL_TRANSFER_REQUEST_BLOCK
{
    uint64_t data_buffer_pointer       : 64; // 63:0
    uint64_t trb_transfer_length       : 17; // 16:0
    uint64_t td_size                   : 5;  // 21:17
    uint64_t interrupter_target        : 10; // 31:22
    uint64_t cycle                     : 1;  // 0
    uint64_t evaluate_next_trb         : 1;  // 1
    uint64_t interrupt_on_short_packet : 1;  // 2
    uint64_t no_snoop                  : 1;  // 3
    uint64_t chain                     : 1;  // 4
    uint64_t interrupt_on_completion   : 1;  // 5
    uint64_t immediate_data            : 1;  // 6
    uint64_t _reserved_0               : 2;  // 8:7
    uint64_t block_event_interrupt     : 1;  // 9
    uint64_t trb_type                  : 6;  // 15:10
    uint64_t _reserved_1               : 16; // 31:16
} NORMAL_TRB;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _LINK_TRANSFER_REQUEST_BLOCK
{
    uint32_t ring_segment_pointer_low  : 32; // 31:0
    uint32_t ring_segment_pointer_high : 32; // 31:0
    uint32_t _reserved_1               : 22; // 21:0
    uint32_t interrupter_target        : 10; // 31:22
    uint32_t cycle                     : 1;  // 0
    uint32_t toggle_cycle              : 1;  // 1
    uint32_t _reserved_2               : 2;  // 3:2
    uint32_t chain                     : 1;  // 4
    uint32_t interrupt_on_completion   : 1;  // 5
    uint32_t _reserved_3               : 4;  // 9:6
    uint32_t trb_type                  : 6;  // 15:10
    uint32_t _reserved_4               : 16; // 31:16
} LINK_TRB;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _DEBUG_CAPABILITY_CONTEXT_DATA
{
    uint8_t dbc_info_context[64];
    uint8_t out_endpoint_context[64];
    uint8_t in_endpoint_context[64];
} DBC_CONTEXT;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _DEBUG_CAPABILITY_INFO_CONTEXT_DATA
{
    uint32_t string_0_descriptor_address_low              : 32; // 31:0
    uint32_t string_0_descriptor_address_high             : 32; // 31:0
    uint32_t manufacturer_string_descriptor_address_low   : 32; // 31:0
    uint32_t manufacturer_string_descriptor_address_high  : 32; // 31:0
    uint32_t product_string_descriptor_address_low        : 32; // 31:0
    uint32_t product_string_descriptor_address_high       : 32; // 31:0
    uint32_t serial_number_string_descriptor_address_low  : 32; // 31:0
    uint32_t serial_number_string_descriptor_address_high : 32; // 31:0
    uint32_t string_0_length                              : 8;  // 7:0
    uint32_t manufacturer_string_length                   : 8;  // 15:8
    uint32_t product_string_length                        : 8;  // 23:16
    uint32_t serial_number_string_length                  : 8;  // 31:24
    uint32_t _reserved_4                                  : 32; // 31:0
    uint32_t _reserved_5                                  : 32; // 31:0
    uint32_t _reserved_6                                  : 32; // 31:0
} DBCIC;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _ENDPOINT_CONTEXT_DATA
{
    uint32_t endpoint_state                                  : 3;  // 2:0
    uint32_t _reserved_0                                     : 5;  // 7:3
    uint32_t mult                                            : 2;  // 9:8
    uint32_t max_primary_streams                             : 5;  // 14:10
    uint32_t linear_stream_array                             : 1;  // 15
    uint32_t interval                                        : 8;  // 23:16
    uint32_t max_endpoint_service_time_interval_payload_high : 8;  // 31:24
    uint32_t _reserved_1                                     : 1;  // 0
    uint32_t error_count                                     : 2;  // 2:1
    uint32_t endpoint_type                                   : 3;  // 5:3
    uint32_t _reserved_2                                     : 1;  // 6
    uint32_t host_initiate_disable                           : 1;  // 7
    uint32_t max_burst_size                                  : 8;  // 15:8
    uint32_t max_packet_size                                 : 16; // 31:16
    uint32_t dequeue_cycle_state                             : 1;  // 0
    uint32_t _reserved_3                                     : 3;  // 3:1
    uint32_t tr_dequeue_pointer_low                          : 28; // 31:4
    uint32_t tr_dequeue_pointer_high                         : 32; // 31:0
    uint32_t average_trb_length                              : 16; // 15:0
    uint32_t max_endpoint_service_time_interval_payload_low  : 16; // 31:16
    uint32_t _reserved_4                                     : 32; // 31:0
    uint32_t _reserved_5                                     : 32; // 31:0
    uint32_t _reserved_6                                     : 32; // 31:0
} ENDPOINT_CONTEXT_DATA;  
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _USB_STRING_0_DESCRIPTOR
{
    uint8_t  length;
    uint8_t  descriptor_type;
    uint16_t language_id;
} USB_STRING_0_DESCRIPTOR;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _USB_STRING_DESCRIPTOR
{
    uint8_t  length;
    uint8_t  descriptor_type;
    uint16_t string[1];
} USB_STRING_DESCRIPTOR;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_EVENT_RING_SEGMENT_TABLE_SIZE_REGISTER
{
    struct
    {
        uint32_t event_ring_segment_table_size : 16; // 15:0
        uint32_t _reserved_0                   : 16; // 31:16
    } fields;
    uint32_t value;
} DCERSTSZ;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_EVENT_RING_SEGMENT_TABLE_BASE_ADDRESS_REGISTER
{
    struct
    {
        uint64_t _reserved_0                                    : 4;  // 3:0
        uint64_t event_ring_segment_table_base_address_register : 60; // 63:4
    } fields;
    uint64_t value;
} DCERSTBA;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_EVENT_RING_DEQUEUE_POINTER_REGISTER
{
    struct
    {
        uint64_t dequeue_erst_segment_index : 3;  // 2:0
        uint64_t _reserved_0                : 1;  // 3
        uint64_t dequeue_pointer            : 60; // 63:4
    } fields;
    uint64_t value;
} DCERDP;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_CONTEXT_POINTER_REGISTER
{
    struct
    {
        uint64_t _reserved_0                               : 4;  // 3:0
        uint64_t debug_capability_context_pointer_register : 60; // 63:4
    } fields;
    uint64_t value;
} DCCP;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_CONTROL_REGISTER
{
    struct
    {
        uint32_t dcr                  : 1;  // 0
        uint32_t lse                  : 1;  // 1
        uint32_t hot                  : 1;  // 2
        uint32_t hit                  : 1;  // 3
        uint32_t drc                  : 1;  // 4
        uint32_t _reserved_0          : 11; // 15:5
        uint32_t debug_max_burst_size : 8;  // 23:16
        uint32_t device_address       : 7;  // 30:24
        uint32_t dce                  : 1;  // 31
    } fields;
    struct
    {
        uint32_t dbc_run                  : 1; // 0
        uint32_t link_status_event_enable : 1; // 1
        uint32_t halt_out_tr              : 1; // 2
        uint32_t halt_in_tr               : 1; // 3
        uint32_t dbc_run_change           : 1; // 4
        uint32_t _reserved_0              : 11; // 15:5
        uint32_t debug_max_burst_size     : 8; // 23:16
        uint32_t device_address           : 7; // 30:24
        uint32_t debug_capability_enable  : 1; // 31
    } named_fields;
    uint32_t value;
} DCCTRL;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_STATUS_REGISTER
{
    struct
    {
        uint32_t er                : 1;  // 0
        uint32_t sbr               : 1;  // 1
        uint32_t _reserved_0       : 22; // 23:2
        uint32_t debug_port_number : 8;  // 31:24
    } fields;
    struct
    {
        uint32_t event_ring_not_empty : 1;  // 0
        uint32_t dbc_system_bus_reset : 1;  // 1
        uint32_t _reserved_0          : 22; // 23:2
        uint32_t debug_port_number    : 8;  // 31:24
    } named_fields;
    uint32_t value;
} DCST;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_PORT_STATUS_AND_CONTROL_REGISTER
{
    struct
    {
        uint32_t ccs         : 1; // 0
        uint32_t ped         : 1; // 1
        uint32_t _reserved_0 : 2; // 3:2
        uint32_t pr          : 1; // 4
        uint32_t pls         : 4; // 8:5
        uint32_t _reserved_1 : 1; // 9
        uint32_t port_speed  : 4; // 13:10
        uint32_t _reserved_2 : 3; // 16:14
        uint32_t csc         : 1; // 17
        uint32_t _reserved_3 : 3; // 20:18
        uint32_t prc         : 1; // 21
        uint32_t plc         : 1; // 22
        uint32_t cec         : 1; // 23
        uint32_t _reserved_4 : 8; // 31:24
    } fields;
    struct
    {
        uint32_t current_connect_status   : 1; // 0
        uint32_t port_enabled_disabled    : 1; // 1
        uint32_t _reserved_0              : 2; // 3:2
        uint32_t port_reset               : 1; // 4
        uint32_t port_link_state          : 4; // 8:5
        uint32_t _reserved_1              : 1; // 9
        uint32_t port_speed               : 4; // 13:10
        uint32_t _reserved_2              : 3; // 16:14
        uint32_t connect_status_change    : 1; // 17
        uint32_t _reserved_3              : 3; // 20:18
        uint32_t port_reset_change        : 1; // 21
        uint32_t port_link_status_change  : 1; // 22
        uint32_t port_config_error_change : 1; // 23
        uint32_t _reserved_4              : 8; // 31:24
    } named_fields;
    uint32_t value;
} DCPORTSC;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_DEVICE_DESCRIPTOR_INFORMATION_FIELD_1
{
    struct
    {
        uint8_t  dbc_protocol; // 7:0
        uint8_t  _reserved_0;  // 15:8
        uint16_t vendor_id;    // 31:16
    } fields;
    uint32_t value;
} DCDDI1;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union _DEBUG_CAPABILITY_DEVICE_DESCRIPTOR_INFORMATION_FIELD_2
{
    struct
    {
        uint16_t product_id;      // 15:0
        uint16_t device_revision; // 31:16
    } fields;
    uint32_t value;
} DCDDI2;
#pragma pack(pop)

typedef struct _DBC_INFORMATION
{
    MEM_ALLOC_INFO event_ring_segment_table_alloc;
    MEM_ALLOC_INFO event_ring_segment_trb_array;
    MEM_ALLOC_INFO dbc_context;
    MEM_ALLOC_INFO string_0_descriptor;
    MEM_ALLOC_INFO serial_number_string_descriptor;
    MEM_ALLOC_INFO out_endpoint_context_trb_array;
    MEM_ALLOC_INFO in_endpoint_context_trb_array;
} DBC_INFO;

bool
xhci_dbc_find_xhc(
    _Out_ PCI_ADDRESS* pci_address
    );

bool
xhci_dbc_get_xhc_info(
    _In_  PCI_ADDRESS* pci_address,
    _Out_ XHC_INFO*    xhc_info
    );

bool
xhci_dbc_setup_dbc(
    _In_  XHC_INFO* xhc_info,
    _Out_ DBC_INFO* dbc_info
    );

#endif // __XHCI_DBC_H__
