#include "mmio.h"

#include <Library/IoLib.h>

uint8_t
mmio_read_u8(
    _In_ uint64_t physical_address
    )
{
    return MmioRead8( physical_address );
}

void
mmio_write_u8(
    _In_ uint64_t physical_address,
    _In_ uint8_t  data
    )
{
    MmioWrite8( physical_address, data );
}

uint16_t
mmio_read_u16(
    _In_ uint64_t physical_address
    )
{
    return MmioRead16( physical_address );
}

void
mmio_write_u16(
    _In_ uint64_t physical_address,
    _In_ uint16_t data
    )
{
    MmioWrite16( physical_address, data );
}

uint32_t
mmio_read_u32(
    _In_ uint64_t physical_address
    )
{
    return MmioRead32( physical_address );
}

void
mmio_write_u32(
    _In_ uint64_t physical_address,
    _In_ uint32_t data
    )
{
    MmioWrite32( physical_address, data );
}

uint64_t
mmio_read_u64(
    _In_ uint64_t physical_address
    )
{
    return MmioRead64( physical_address );
}

void
mmio_write_u64(
    _In_ uint64_t physical_address,
    _In_ uint64_t data
    )
{
    MmioWrite64( physical_address, data );
}
