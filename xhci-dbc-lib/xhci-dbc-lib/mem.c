#include "mem.h"

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

bool
mem_alloc(
    _In_  uint32_t        length,
    _Out_ MEM_ALLOC_INFO* alloc_info
    )
{
    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS alloc_address;

    uint32_t page_count = length / EFI_PAGE_SIZE;

    if ( length % EFI_PAGE_SIZE != 0 || length == 0 )
    {
        page_count += 1;
    }

    status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiRuntimeServicesData, // EfiMemoryMappedIO,
        page_count,
        &alloc_address
        );

    if ( EFI_ERROR(status) == TRUE )
    {
        return FALSE;
    }

    gBS->SetMem(
        (void*)alloc_address,
        ( page_count * EFI_PAGE_SIZE ),
        0
        );

    alloc_info->virtual_address  = (void*)alloc_address;
    alloc_info->physical_address = (uint64_t)alloc_address;
    alloc_info->length           = length;

    return TRUE;
}

void
mem_free(
    _In_ MEM_ALLOC_INFO* alloc_info
    )
{
    uint32_t length = alloc_info->length;

    uint32_t page_count = length / EFI_PAGE_SIZE;

    if ( length % EFI_PAGE_SIZE != 0 || length == 0 )
    {
        page_count += 1;
    }

    gBS->FreePages( (EFI_PHYSICAL_ADDRESS)alloc_info->virtual_address, page_count );
}
