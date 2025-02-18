#ifndef __IO_H__
#define __IO_H__

#include "types.h"

void
io_outl(
    _In_ uint16_t port,
    _In_ uint32_t data
    );

uint32_t
io_inl(
    _In_ uint16_t port
    );

#endif // __IO_H__
