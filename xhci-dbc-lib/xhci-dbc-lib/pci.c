#include "pci.h"

uint32_t
pci_read_config_u32(
    _In_ uint8_t bus,
    _In_ uint8_t device,
    _In_ uint8_t function,
    _In_ uint8_t offset
    )
{
    PCI_CONFIG_ADDRESS_INFO address;

    address.fields.register_offset = ( offset & 0xFC );
    address.fields.function_number = function;
    address.fields.device_number   = device;
    address.fields.bus_number      = bus;
    address.fields._reserved_0     = 0;
    address.fields.enabled         = 1;

    io_outl( PCI_CONFIG_ADDRESS, address.value );

    return io_inl( PCI_CONFIG_DATA );
}

void
pci_write_config_u32(
    _In_ uint8_t  bus,
    _In_ uint8_t  device,
    _In_ uint8_t  function,
    _In_ uint8_t  offset,
    _In_ uint32_t data
    )
{
    PCI_CONFIG_ADDRESS_INFO address;

    address.fields.register_offset = ( offset & 0xFC );
    address.fields.function_number = function;
    address.fields.device_number   = device;
    address.fields.bus_number      = bus;
    address.fields._reserved_0     = 0;
    address.fields.enabled         = 1;

    io_outl( PCI_CONFIG_ADDRESS, address.value );

    io_outl( PCI_CONFIG_DATA, data );
}

uint16_t
pci_read_vendor_id(
    _In_ uint8_t bus,
    _In_ uint8_t device,
    _In_ uint8_t function
    )
{
    uint16_t vendor_id = (uint16_t)(
        pci_read_config_u32( bus, device, function, 0 ) & 0xFFFF
    );

    return vendor_id;
}

uint16_t
pci_read_device_id(
    _In_ uint8_t bus,
    _In_ uint8_t device,
    _In_ uint8_t function
    )
{
    uint16_t device_id = (uint16_t)(
        ( pci_read_config_u32(bus, device, function, 0) >> 16 ) & 0xFFFF
    );

    return device_id;
}


