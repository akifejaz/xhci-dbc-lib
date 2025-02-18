#ifndef __MMIO_H__
#define __MMIO_H__

#include "types.h"

uint8_t
mmio_read_u8(
    _In_ uint64_t physical_address
    );

void
mmio_write_u8(
    _In_ uint64_t physical_address,
    _In_ uint8_t  data
    );

uint16_t
mmio_read_u16(
    _In_ uint64_t physical_address
    );

void
mmio_write_u16(
    _In_ uint64_t physical_address,
    _In_ uint16_t data
    );

uint32_t
mmio_read_u32(
    _In_ uint64_t physical_address
    );

void
mmio_write_u32(
    _In_ uint64_t physical_address,
    _In_ uint32_t data
    );

uint64_t
mmio_read_u64(
    _In_ uint64_t physical_address
    );

void
mmio_write_u64(
    _In_ uint64_t physical_address,
    _In_ uint64_t data
    );

#endif // __MMIO_H__
