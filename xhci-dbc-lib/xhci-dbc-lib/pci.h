#ifndef __PCI_H__
#define __PCI_H__

#include "types.h"

#include "io.h"

#define PCI_CONFIG_ADDRESS                      0xCF8
#define PCI_CONFIG_DATA                         0xCFC

#define PCI_MAX_BUS_COUNT                       256
#define PCI_MAX_DEVICE_COUNT                    32
#define PCI_MAX_FUNCTION_COUNT                  8

#define PCI_INVALID_VENDOR_ID                   0xFFFF

#define PCI_VENDOR_ID_INTEL                     0x8086

#define PCI_HEADER_TYPE_STANDARD_DEVICE         0
#define PCI_HEADER_TYPE_MULTI_FUNCTION_DEVICE   0x80

#define PCI_STANDARD_DEVICE_BAR0_OFFSET         0x10
#define PCI_STANDARD_DEVICE_BAR1_OFFSET         0x14

#define PCI_BAR_IO_SPACE_TYPE_MASK              1
#define PCI_BAR_TYPE_MASK                       0b110
#define PCI_BAR_TYPE_64_BIT                     0b100

typedef union _PCI_CONFIG_ADDRESS_INFORMATION
{
    struct
    {
        uint32_t register_offset : 8; // 7:0
        uint32_t function_number : 3; // 10:8
        uint32_t device_number   : 5; // 15:11
        uint32_t bus_number      : 8; // 23:16
        uint32_t _reserved_0     : 7; // 30:24
        uint32_t enabled         : 1; // 31
    } fields;
    uint32_t value;
} PCI_CONFIG_ADDRESS_INFO;

typedef PCI_CONFIG_ADDRESS_INFO PCI_ADDRESS;

typedef union _PCI_REGISTER_ONE_INFORMATION
{
    struct
    {
        uint32_t command : 16; // 15:0
        uint32_t status  : 16; // 31:16
    } fields;
    uint32_t value;
} PCI_REG_ONE_INFO;

typedef union _PCI_REGISTER_TWO_INFORMATION
{
    struct
    {
        uint32_t revision_id           : 8; // 7:0
        uint32_t programming_interface : 8; // 15:8
        uint32_t subclass              : 8; // 23:16
        uint32_t class_code            : 8; // 31:24
    } fields;
    uint32_t value;
} PCI_REG_TWO_INFO;

typedef union _PCI_REGISTER_THREE_INFORMATION
{
    struct
    {
        uint32_t cache_line_size : 8; // 7:0
        uint32_t latency_timer   : 8; // 15:8
        uint32_t header_type     : 8; // 23:16
        uint32_t bist            : 8; // 31:24
    } fields;
    uint32_t value;
} PCI_REG_THREE_INFO;

uint32_t
pci_read_config_u32(
    _In_ uint8_t bus,
    _In_ uint8_t device,
    _In_ uint8_t function,
    _In_ uint8_t offset
    );

void
pci_write_config_u32(
    _In_ uint8_t  bus,
    _In_ uint8_t  device,
    _In_ uint8_t  function,
    _In_ uint8_t  offset,
    _In_ uint32_t data
    );

uint16_t
pci_read_vendor_id(
    _In_ uint8_t bus,
    _In_ uint8_t device,
    _In_ uint8_t function
    );

uint16_t
pci_read_device_id(
    _In_ uint8_t bus,
    _In_ uint8_t device,
    _In_ uint8_t function
    );

#endif // __PCI_H__
