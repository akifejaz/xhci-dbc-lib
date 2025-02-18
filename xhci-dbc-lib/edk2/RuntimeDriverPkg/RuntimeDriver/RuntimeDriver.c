#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DebugLib.h>

#include <intrin.h>

#include "../xhci-dbc-lib/xhci_dbc.h"

EFI_STATUS
EFIAPI
DriverEntryPoint (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    PCI_ADDRESS xhc_pci_address;
    XHC_INFO    xhc_info;
    DBC_INFO    dbc_info;

    if ( xhci_dbc_find_xhc(&xhc_pci_address) == FALSE )
    {
        Print( L"Unable to find controller!\n" );
        return EFI_LOAD_ERROR;
    }

    Print(
        L"Found controller! (bus: 0x%X (%d), device: 0x%X (%d), function: 0x%X (%d))\n",
        xhc_pci_address.fields.bus_number, xhc_pci_address.fields.bus_number,
        xhc_pci_address.fields.device_number, xhc_pci_address.fields.device_number,
        xhc_pci_address.fields.function_number, xhc_pci_address.fields.function_number
        );

    if ( xhci_dbc_get_xhc_info(&xhc_pci_address, &xhc_info) == FALSE )
    {
        Print( L"Unable to obtain information on controller!\n" );

        return EFI_LOAD_ERROR;
    }

    Print(
        L"Controller information found!\n"
        "  MMIO region address = 0x%lX\n"
        "  MMIO region length = 0x%X\n"
        "  Extended Capabilities Structure base = 0x%lX\n"
        "  Debug Capabilities Structure base = 0x%lX\n",
        xhc_info.mmio_phys_base,
        xhc_info.mmio_length,
        xhc_info.ext_cap_struct_base,
        xhc_info.debug_cap_struct_base
        );

    if ( xhci_dbc_setup_dbc(&xhc_info, &dbc_info) == FALSE )
    {
        Print( L"Unable to setup DbC operation!\n" );

        return EFI_LOAD_ERROR;
    }

    return EFI_SUCCESS;
}
