#ifndef __MEM_H__
#define __MEM_H__

#include "types.h"

typedef struct _MEMORY_ALLOCATION_INFORMATION
{
    void*    virtual_address;
    uint64_t physical_address;
    uint32_t length;
} MEM_ALLOC_INFO;

bool
mem_alloc(
    _In_  uint32_t        length,
    _Out_ MEM_ALLOC_INFO* alloc_info
    );

void
mem_free(
    _In_ MEM_ALLOC_INFO* alloc_info
    );

#endif // __MEM_H__
