#include "xhci_dbc.h"

bool
xhci_dbc_send(
    _In_  XHC_INFO* xhc_info,
    _In_  void*     data,
    _In_  int       size,
    _In_  uint64_t  addr    
    )
{
    // To-Do: Spin Lock should be added so only one hart can send data at one time

    // For data transfer, first check if the size of the data packet doesn’t exceed the max limit.
    if (size > DBC_ENDPOINT_CONTEXT_DEFAULT_MAX_PACKET_SIZE)
    {
        return FALSE;
    }

    // Check if connection is stable
    
    // Get DCCTRL value
    dcctrl.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET )
        );    
    
    // If Debug Device is not in Configured state or device is busy (previous transfer is not completed)
    if ( (!dcctrl.named_fields.dbc_run) ||
         (dcctrl.named_fields.halt_out_tr) )
    {
        return FALSE;
    }

    // If we reach this step, we are ready to send data to the remote machine  
    
    // Create a new TRB

    // Set BIT(2) of DCCTRL i.e. halt OUT TR
    
    // Get DCCTRL value
    dcctrl.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET )
        );
    
    // Update HALT_OUT_TR bit
    dcctrl.named_fields.halt_out_tr = 1; 

    // Write back to memory
    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET ),
        dcctrl.value
        );        
    
    // Queue it to the existing ones

    // Ring doorbell
    xhci_dbc_ring_doorbell(xhc_info, DBC_DCDB_OUT_EP);
}

bool
xhci_dbc_receive(
    _In_  XHC_INFO* xhc_info,
    _In_  int       size,
    _In_  uint64_t  addr
    )
{
    // To-Do: Spin Lock should be added so only one hart can send data at one time

    // For data transfer, first check if the size of the data packet doesn’t exceed the max limit.
    if (size > DBC_ENDPOINT_CONTEXT_DEFAULT_MAX_PACKET_SIZE)
    {
        return FALSE;
    }

    // Check if connection is stable
    
    // Get DCCTRL value
    dcctrl.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET )
        );    
    
    // If Debug Device is not in Configured state or device is busy (previous transfer is not completed)
    if ( (!dcctrl.named_fields.dbc_run) ||
         (dcctrl.named_fields.halt_in_tr) )
    {
        return FALSE;
    }

    // If we reach this step, we are ready to receive data to the remote machine  
    
    // Create a new TRB

    // Set BIT(3) of DCCTRL i.e. halt In TR
    
    // Get DCCTRL value
    dcctrl.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET )
        );
    
    // Update HALT_IN_TR bit
    dcctrl.named_fields.halt_in_tr = 1; 

    // Write back to memory
    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET ),
        dcctrl.value
        );    
        
    // Queue it to the existing ones

    // Ring doorbell
    xhci_dbc_ring_doorbell(xhc_info, DBC_DCDB_IN_EP);
}

void
xhci_dbc_ring_doorbell(
    _In_  XHC_INFO* xhc_info,
    _In_  uint32_t doorbell
    )
{
    // Read current value from MMIO reg
    dcdb.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCDB_OFFSET )
        );
    
    // Clear out 15:8 bits
    dcdb.value &= ~(0xFF << 8);
    // Update value 
    dcdb.named_fields.doorbell_target = doorbell;
    
    // Write back updated value to MMIO reg
    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCDB_OFFSET ),
        dcdb.value
        );    
}

bool
xhci_dbc_handshake(
    _In_  XHC_INFO* xhc_info,     
    _In_  uint8_t reg_offset,
    _In_  uint8_t mask_bit,
    _In_  uint8_t exp_val
    )
{
    uint32_t result;

    while ( TRUE )
    {
        result = mmio_read_u32( ( xhc_info->debug_cap_struct_base + reg_offset ) );
        if ( (result & (1U << mask_bit)) == exp_val )
            break;
    }

    return TRUE;
}

bool
xhci_dbc_find_xhc(
    _Out_ PCI_ADDRESS* pci_address
    )
{
    uint16_t vendor_id = 0;

    PCI_REG_THREE_INFO reg_three_info;
    PCI_REG_TWO_INFO   reg_two_info;

    /*
     * Iterate through all possible devices and try to find the target
     *  xHC
     */

    for ( uint8_t device_idx = 0; device_idx < PCI_MAX_DEVICE_COUNT; device_idx++ )
    {
        for ( uint8_t function_idx = 0; function_idx < PCI_MAX_FUNCTION_COUNT; function_idx++ )
        {
            vendor_id = pci_read_vendor_id( 0, device_idx, function_idx );

            if ( vendor_id != PCI_VENDOR_ID_INTEL )
            {
                continue;
            }

            reg_three_info.value = pci_read_config_u32( 0, device_idx, function_idx, 0xC );

            if ( reg_three_info.fields.header_type != PCI_HEADER_TYPE_STANDARD_DEVICE
                && reg_three_info.fields.header_type != PCI_HEADER_TYPE_MULTI_FUNCTION_DEVICE )
            {
                continue;
            }

            /*
             * We successfully found a device, so check its class and subclass values
             *  to see whether or not this is an xHC
             */

            reg_two_info.value = pci_read_config_u32( 0, device_idx, function_idx, 8 );

            if ( reg_two_info.fields.class_code == XHCI_DBC_CLASS_TYPE_SERIAL_BUS_CONTROLLER
                && reg_two_info.fields.subclass == XHCI_DBC_SUBCLASS_TYPE_USB_CONTROLLER
                && reg_two_info.fields.programming_interface == XHCI_DBC_PROGRAMMING_INTERFACE_XHCI_CONTROLLER )
            {
                pci_address->fields.register_offset = 0;
                pci_address->fields.function_number = function_idx;
                pci_address->fields.device_number   = device_idx;
                pci_address->fields.bus_number      = 0;
                pci_address->fields._reserved_0     = 0;
                pci_address->fields.enabled         = 1;

                return TRUE;
            }
        }
    }

    return FALSE;
}

bool
xhci_dbc_get_xhc_info(
    _In_  PCI_ADDRESS* pci_address,
    _Out_ XHC_INFO*    xhc_info
    )
{
    uint32_t bar_0 = 0;
    uint32_t bar_1 = 0;

    uint32_t bar_max_base_address = 0;

    uint64_t bar_address = 0;
    uint32_t bar_length  = 0;

    HCCPARAMS1_INFO hccparams1;

    uint64_t ext_cap_struct_base         = 0;
    uint64_t current_ext_cap_struct_base = 0;

    XHCI_EXT_CAP_PTR_REG ext_cap_ptr_reg;

    uint64_t debug_cap_struct_base = 0;

    bar_0 = pci_read_config_u32(
        (uint8_t)pci_address->fields.bus_number,
        (uint8_t)pci_address->fields.device_number,
        (uint8_t)pci_address->fields.function_number,
        PCI_STANDARD_DEVICE_BAR0_OFFSET
        );

    /*
     * Ensure this BAR points to a region of MMIO space (i.e. it's not an I/O space BAR)
     */

    if ( (bar_0 & PCI_BAR_IO_SPACE_TYPE_MASK) == 1 )
    {
        return FALSE;
    }

    /*
     * Ensure this BAR is 64-bits in width
     */

    if ( (bar_0 & PCI_BAR_TYPE_MASK) != PCI_BAR_TYPE_64_BIT )
    {
        return FALSE;
    }

    /*
     * Arrange the base address of this BAR (BAR #0) by combining its 32-bit value with the
     *  32-bit value of the adjacent BAR #1 register
     */

    bar_1 = pci_read_config_u32(
       (uint8_t)pci_address->fields.bus_number,
       (uint8_t)pci_address->fields.device_number,
       (uint8_t)pci_address->fields.function_number,
       PCI_STANDARD_DEVICE_BAR1_OFFSET
       );

    bar_address = ( ((uint64_t)bar_1 & 0xFFFFFFFF) << 32 ) + ( bar_0 & 0xFFFFFFF0 );

    /*
     * Obtain the length of the memory region described by this BAR by writing a value of all
     *  1s to the BAR. Reading this value back will have the least significant bits that
     *  correspond to the minumum alignment of the BAR's allocation unset, and we can then do
     *  some bit math to have obtained the size.
     */

    pci_write_config_u32(
        (uint8_t)pci_address->fields.bus_number,
        (uint8_t)pci_address->fields.device_number,
        (uint8_t)pci_address->fields.function_number,
        PCI_STANDARD_DEVICE_BAR0_OFFSET,
        0xFFFFFFFF
        );

    bar_max_base_address = pci_read_config_u32(
        (uint8_t)pci_address->fields.bus_number,
        (uint8_t)pci_address->fields.device_number,
        (uint8_t)pci_address->fields.function_number,
        PCI_STANDARD_DEVICE_BAR0_OFFSET
        );

    bar_length = ( ~(bar_max_base_address & 0xFFFFFFF0) ) + 1;

    /*
     * Restore this BAR's original value
     */

    pci_write_config_u32(
        (uint8_t)pci_address->fields.bus_number,
        (uint8_t)pci_address->fields.device_number,
        (uint8_t)pci_address->fields.function_number,
        PCI_STANDARD_DEVICE_BAR0_OFFSET,
        bar_0
        );

    /*
     * Ensure that the xHC supports extended capabilities, and calculate the address
     *  of our xECP base (or the Extended Capability Structure) which we need to then
     *  find the Debug Capability (DbC) Structure
     */

    hccparams1.value = mmio_read_u32( bar_address + XHCI_HCCPARAMS1_OFFSET );

    if ( hccparams1.named_fields.ext_cap_pointer == 0 )
    {
        return FALSE;
    }

    ext_cap_struct_base = bar_address + ( hccparams1.named_fields.ext_cap_pointer * 4 ); // raw xECP value specifies the offset to the xECP base in DWORDs

    /*
     * Each of the extended capabilities are in a list that we need to parse, so let's
     *  do that now and look for the Debug Capability Structure which is also indicative
     *  of the fact that this xHC supports DbC operation
     */

    current_ext_cap_struct_base = ext_cap_struct_base;

    while ( current_ext_cap_struct_base != 0 )
    {
        ext_cap_ptr_reg.value = mmio_read_u32( current_ext_cap_struct_base );

        if ( ext_cap_ptr_reg.fields.cap_id == XHCI_DBC_CAP_ID )
        {
            debug_cap_struct_base = current_ext_cap_struct_base;

            break;
        }

        if ( ext_cap_ptr_reg.fields.next_ext_cap_offset == 0 )
        {
            break;
        }

        current_ext_cap_struct_base += ( ext_cap_ptr_reg.fields.next_ext_cap_offset * 4 );
    }

    if ( debug_cap_struct_base == 0 )
    {
        return FALSE;
    }

    /*
     * Return the information we gathered back to the caller
     */

    xhc_info->mmio_phys_base        = bar_address;
    xhc_info->mmio_length           = bar_length;
    xhc_info->ext_cap_struct_base   = ext_cap_struct_base;
    xhc_info->debug_cap_struct_base = debug_cap_struct_base;

    return TRUE;
}

void
xhci_dbc_reset(
    _In_  XHC_INFO* xhc_info
    )
{
    // Reset DbC Port by setting bit 4 of DCPORTSC register
    // Read current value from MMIO reg
    dcportsc.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCPORTSC_OFFSET )
        );
    
    // Update value i.e. reset Port by setting Bit 4
    dcportsc.named_fields.port_reset = 1;

    // Write back updated value to MMIO reg
    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCPORTSC_OFFSET ),
        dcportsc.value
        );    

    // Bit 4 (Port Reset) is cleared when the bus reset sequence is completed by the 
    // Debug Host, and the DbC shall transition to the USB default state i.e. setting bit 31 of 
    // DCCTRL and bit 1 of DCPORTSC. 

    // Wait for Reset Port (Bit 4) to be cleared
    xhci_dbc_handshake(xhc_info, XHCI_DBC_DCPORTSC_OFFSET, 4, 0);

    // Enable DbC -- reset to USB default state completed
    xhci_dbc_enable(xhc_info);
}

bool
xhci_dbc_enable(
    _In_  XHC_INFO* xhc_info
    )
{
    /*
     * Enable DbC by 
     * 1- Set CTRL_DBC_ENABLE (bit 31) of Debug Capability Control Register (DCCTRL) – Enable DbC
     * 2- Set CTRL_PORT_ENABLE (bit 1) of Debug Capability Port Status and Control Register (DCPORTSC) - Enable Port
     */

    // Enable DbC
    // Read current value from MMIO reg
    dcctrl.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET )
        );
    
    // Update value i.e. enable DbC by setting Bit 31
    dcctrl.named_fields.debug_capability_enable = 1;
    
    // Write back updated value to MMIO reg
    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET ),
        dcctrl.value
        );

    // Check if value is being written successfully
    if (!xhci_dbc_handshake(xhc_info,
                            XHCI_DBC_DCCTRL_OFFSET, 
                            31, 
                            (uint8_t)dcctrl.named_fields.debug_capability_enable))
    {
        // Error handling
        return FALSE;
    }

    // Enable Port
    // Read current value from MMIO reg
    dcportsc.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCPORTSC_OFFSET )
        );
    
    // Update value i.e. enable Port by setting Bit 1
    dcportsc.named_fields.port_enabled_disabled = 1;
    
    // Write back updated value to MMIO reg
    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCPORTSC_OFFSET ),
        dcportsc.value
        );

    // Check if value is being written successfully
    if (!xhci_dbc_handshake(xhc_info,
                            XHCI_DBC_DCPORTSC_OFFSET, 
                            1, 
                            (uint8_t)dcportsc.named_fields.port_enabled_disabled))
    {
        // Error handling
        return FALSE;
    }

    return TRUE;
}

bool
xhci_dbc_check_enabled(
    _In_  XHC_INFO* xhc_info
    )
{
    // Check if CTRL_DBC_ENABLE (bit 31) of Debug Capability Control Register (DCCTRL) is set or unset
    dcctrl.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET )
        );

    if ( dcctrl.named_fields.debug_capability_enable ) 
        return TRUE;
    else
        return FALSE;
}

bool
xhci_dbc_disable(
    _In_  XHC_INFO* xhc_info
    )
{
    /*
     * Disable DbC first before any config by 
     * 1- Unset CTRL_PORT_ENABLE (bit 1) of Debug Capability Port Status and Control Register (DCPORTSC) - Enable Port
     * 2- Unset CTRL_DBC_ENABLE (bit 31) of Debug Capability Control Register (DCCTRL) – Enable DbC
     */

    // Disable Port
    // Read current value from MMIO reg
    dcportsc.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCPORTSC_OFFSET )
        );
    
    // Update value i.e. disable Port by setting Bit 1
    dcportsc.named_fields.port_enabled_disabled = 0;
    
    // Write back updated value to MMIO reg
    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCPORTSC_OFFSET ),
        dcportsc.value
        );   

    
    // Check if value is being written successfully
    if (!xhci_dbc_handshake(xhc_info,
                            XHCI_DBC_DCPORTSC_OFFSET, 
                            1, 
                            (uint8_t)dcportsc.named_fields.port_enabled_disabled))
    {
        // Error handling
        return FALSE;
    }
    
    // Disable DbC
    // Read current value from MMIO reg
    dcctrl.value = mmio_read_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET )
        );
    
    // Update value i.e. disable DbC by unsetting Bit 31
    dcctrl.named_fields.debug_capability_enable = 0;
    
    // Write back updated value to MMIO reg
    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET ),
        dcctrl.value
        );

    // Check if value is being written successfully
    if (!xhci_dbc_handshake(xhc_info,
                            XHCI_DBC_DCCTRL_OFFSET, 
                            31, 
                            (uint8_t)dcctrl.named_fields.debug_capability_enable))
    {
        // Error handling
        return FALSE;
    }

    return TRUE;
}

bool
xhci_dbc_setup_dbc(
    _In_  XHC_INFO* xhc_info,   
    _Out_ DBC_INFO* dbc_info
    )
{
    /*
     * This function attempts to remain true to the steps listed within the xHCI
     *  specification, section 7.6.4.1 "Debug Capability Initialization", which 
     *  outlines an ordered series of steps software should take when initializing
     *  DbC and entering into DbC operation
     */

    bool status = FALSE;

    NORMAL_TRB* event_trb_array;
    ERST_ENTRY* event_ring_segment_table_entry;

    DBC_CONTEXT*           dbc_context_data;
    DBCIC*                 dbcic;
    ENDPOINT_CONTEXT_DATA* out_endpoint_context;
    ENDPOINT_CONTEXT_DATA* in_endpoint_context;

    USB_STRING_0_DESCRIPTOR* string_0_descriptor;

    NORMAL_TRB* out_trb_array;
    LINK_TRB*   out_link_trb;

    NORMAL_TRB* in_trb_array;
    LINK_TRB*   in_link_trb;

    // Disable DbC
    if ( !xhci_dbc_disable(xhc_info) )
    {
        return FALSE;
    }

    /*
     * Allocate and initialize all DbC memory data structures
     */

    // Setup the DbC Event Ring Segment Table and the Event Ring Segment that it points to

    if ( mem_alloc(
        ( sizeof(ERST_ENTRY) * 1 ),
        &dbc_info->event_ring_segment_table_alloc) == FALSE )
    {
        return FALSE;
    }

    if ( mem_alloc(
        ( sizeof(TRB) * EVENT_RING_SEGMENT_MIN_TRB_COUNT ),
        &dbc_info->event_ring_segment_trb_array) == FALSE )
    {
        goto __cleanup;
    }

    event_trb_array = (NORMAL_TRB*)dbc_info->event_ring_segment_trb_array.virtual_address;

    event_trb_array[0].cycle = 1;

    event_ring_segment_table_entry = (ERST_ENTRY*)dbc_info->event_ring_segment_table_alloc.virtual_address;

    event_ring_segment_table_entry->ring_segment_base_address_low = 
        ( dbc_info->event_ring_segment_trb_array.physical_address & ERST_ENTRY_BASE_ADDRESS_LOW_MASK );

    event_ring_segment_table_entry->ring_segment_base_address_high = 
        ( (dbc_info->event_ring_segment_trb_array.physical_address >> 32) & ERST_ENTRY_BASE_ADDRESS_HIGH_MASK );

    event_ring_segment_table_entry->ring_segment_size = EVENT_RING_SEGMENT_MIN_TRB_COUNT;

    // Setup the DbC IN and OUT Endpoint Contexts and the Transfer Rings that they point to
    //  (Note: what the manual doesn't tell you outright with this step is that the IN and OUT
    //  Endpoint Context structures are nested within something called the Debug Context Data
    //  structure, which also includes the DbCIC structure which has to be setup)

    if ( mem_alloc(
        sizeof( DBC_CONTEXT ),
        &dbc_info->dbc_context) == FALSE )
    {
        goto __cleanup;
    }

    dbc_context_data = (DBC_CONTEXT*)dbc_info->dbc_context.virtual_address;

    dbcic                = (DBCIC*)&dbc_context_data->dbc_info_context;
    out_endpoint_context = (ENDPOINT_CONTEXT_DATA*)&dbc_context_data->out_endpoint_context;
    in_endpoint_context  = (ENDPOINT_CONTEXT_DATA*)&dbc_context_data->in_endpoint_context;

    // Initialize the DBCIC to only what is required

    if ( mem_alloc(
        sizeof( USB_STRING_0_DESCRIPTOR ),
        &dbc_info->string_0_descriptor) == FALSE )
    {
        goto __cleanup;
    }

    string_0_descriptor = (USB_STRING_0_DESCRIPTOR*)dbc_info->string_0_descriptor.virtual_address;

    string_0_descriptor->length          = sizeof( USB_STRING_0_DESCRIPTOR );
    string_0_descriptor->descriptor_type = USB_STRING_DESCRIPTOR_TYPE;
    string_0_descriptor->language_id     = USB_STRING_DESCRIPTOR_LANGUAGE_ID_EN_US;

    dbcic->string_0_descriptor_address_low =
        ( dbc_info->string_0_descriptor.physical_address & DBCIC_STRING_DESCRIPTOR_BASE_ADDRESS_LOW_MASK );

    dbcic->string_0_descriptor_address_high =
        ( (dbc_info->string_0_descriptor.physical_address >> 32) & DBCIC_STRING_DESCRIPTOR_BASE_ADDRESS_HIGH_MASK );

    dbcic->string_0_length = string_0_descriptor->length;

    // Now we have to setup the Endpoint Context structures, which are done in accordance with
    //  sections 7.6.3.2 "Endpoint Contexts and Transfer Rings" and 4.8.3 "Endpoint Context State"
    //  of the xHCI specification

    // Setup the OUT TRB, and ensure that this Transfer Ring correctly wraps around to the first entry
    //  via the setup of a Link TRB at the end of the buffer

    if ( mem_alloc(
        ( sizeof(TRB) * DBC_ENDPOINT_CONTEXT_MIN_TRB_COUNT ),
        &dbc_info->out_endpoint_context_trb_array) == FALSE )
    {
        goto __cleanup;
    }

    out_trb_array = (NORMAL_TRB*)dbc_info->out_endpoint_context_trb_array.virtual_address;

    out_trb_array[0].cycle = 1; // no idea if i actually need this, but supposedly it should match the out_endpoint_context's dequeue cycle state field and be non-zero

    out_link_trb = (LINK_TRB*)&out_trb_array[DBC_ENDPOINT_CONTEXT_MIN_TRB_COUNT - 1];

    out_link_trb->ring_segment_pointer_low =
        ( dbc_info->out_endpoint_context_trb_array.physical_address & LINK_TRB_RING_SEGMENT_POINTER_LOW_MASK );

    out_link_trb->ring_segment_pointer_high =
        ( (dbc_info->out_endpoint_context_trb_array.physical_address >> 32) & LINK_TRB_RING_SEGMENT_POINTER_HIGH_MASK );

    out_link_trb->trb_type = TRB_TYPE_LINK;

    // Setup the OUT Endpoint Context

    out_endpoint_context->max_packet_size         = DBC_ENDPOINT_CONTEXT_DEFAULT_MAX_PACKET_SIZE;
    out_endpoint_context->max_burst_size          = 0;
    out_endpoint_context->endpoint_type           = DBC_ENDPOINT_CONTEXT_TYPE_OUT_BULK;

    out_endpoint_context->tr_dequeue_pointer_low  =
        ( dbc_info->out_endpoint_context_trb_array.physical_address & ENDPOINT_CONTEXT_TR_DEQUEUE_POINTER_LOW_MASK );

    out_endpoint_context->tr_dequeue_pointer_high =
        ( (dbc_info->out_endpoint_context_trb_array.physical_address >> 32) & ENDPOINT_CONTEXT_TR_DEQUEUE_POINTER_HIGH_MASK );

    out_endpoint_context->average_trb_length      = DBC_ENDPOINT_CONTEXT_DEFAULT_AVERAGE_TRB_LENGTH;

    out_endpoint_context->dequeue_cycle_state     = 1; // no idea if i actually need this, but supposedly it should match the first TRB's cycle field

    // Setup the IN TRB, and ensure that this Transfer Ring correctly wraps around to the first entry
    //  via the setup of a Link TRB at the end of the buffer

    if ( mem_alloc(
        ( sizeof(TRB) * DBC_ENDPOINT_CONTEXT_MIN_TRB_COUNT ),
        &dbc_info->in_endpoint_context_trb_array) == FALSE )
    {
        goto __cleanup;
    }

    in_trb_array = (NORMAL_TRB*)dbc_info->in_endpoint_context_trb_array.virtual_address;

    in_trb_array[0].cycle = 1; // no idea if i actually need this, but supposedly it should match the out_endpoint_context's dequeue cycle state field and be non-zero

    in_link_trb = (LINK_TRB*)&in_trb_array[DBC_ENDPOINT_CONTEXT_MIN_TRB_COUNT - 1];

    in_link_trb->ring_segment_pointer_low =
        ( dbc_info->in_endpoint_context_trb_array.physical_address & LINK_TRB_RING_SEGMENT_POINTER_LOW_MASK );

    in_link_trb->ring_segment_pointer_high =
        ( (dbc_info->in_endpoint_context_trb_array.physical_address >> 32) & LINK_TRB_RING_SEGMENT_POINTER_HIGH_MASK );

    in_link_trb->trb_type = TRB_TYPE_LINK;

    // Setup the IN Endpoint Context

    in_endpoint_context->max_packet_size         = DBC_ENDPOINT_CONTEXT_DEFAULT_MAX_PACKET_SIZE;
    in_endpoint_context->max_burst_size          = 0;
    in_endpoint_context->endpoint_type           = DBC_ENDPOINT_CONTEXT_TYPE_IN_BULK;

    in_endpoint_context->tr_dequeue_pointer_low  =
        ( dbc_info->in_endpoint_context_trb_array.physical_address & ENDPOINT_CONTEXT_TR_DEQUEUE_POINTER_LOW_MASK );

    in_endpoint_context->tr_dequeue_pointer_high =
        ( (dbc_info->in_endpoint_context_trb_array.physical_address >> 32) & ENDPOINT_CONTEXT_TR_DEQUEUE_POINTER_HIGH_MASK );

    in_endpoint_context->average_trb_length      = DBC_ENDPOINT_CONTEXT_DEFAULT_AVERAGE_TRB_LENGTH;

    in_endpoint_context->dequeue_cycle_state     = 1; // no idea if i actually need this, but supposedly it should match the first TRB's cycle field

    // Setup each of the DbC registers that point to everything we've setup

    // Initialize the Debug Capability Event Ring Segment Table Size Register (DCERSTSZ) with the number
    //  of entries in the Event Ring Segment Table

    dcerstsz.value = 0;

    dcerstsz.fields.event_ring_segment_table_size = 1;

    mmio_write_u32(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCERSTSZ_OFFSET ),
        dcerstsz.value
        );

    // Initialize the Debug Capability Event Ring Segment Table Base Address Register (DCERSTBA) with the
    //  physical memory address of the Event Ring Segment Table

    dcerstba.value = 0;

    dcerstba.fields.event_ring_segment_table_base_address_register =
        ( dbc_info->event_ring_segment_table_alloc.physical_address & EVENT_RING_SEGMENT_TABLE_BASE_ADDRESS_REGISTER_MASK );

    mmio_write_u64(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCERSTBA_OFFSET ),
        dcerstba.value
        );

    // Initialize the Debug Capability Event Ring Segment Table Dequeue Pointer Register (DCERDP) with the
    //  physical memory address of the Event Ring Segment pointer to by the Event Ring Segment Table entry 0

    dcerdp.value = 0;

    dcerdp.fields.dequeue_pointer =
        ( dbc_info->event_ring_segment_table_alloc.physical_address & EVENT_RING_DEQUEUE_POINTER_REGISTER_MASK );

    mmio_write_u64(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCERDP_OFFSET ),
        dcerdp.value
        );

    // Initialize the Debug Capability Context Pointer (DCCP) with the physical memory address of the Debug
    //  Capability Context

    dccp.value = 0;

    dccp.fields.debug_capability_context_pointer_register =
        ( dbc_info->dbc_context.physical_address & DEBUG_CAPABILITY_CONTEXT_POINTER_REGISTER_MASK );

    mmio_write_u64(
        ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCP_OFFSET ),
        dccp.value
        );
    

    // Enable DbC
    if ( !xhci_dbc_enable(xhc_info) )
    {
        return FALSE;
    }

    // Poll the Event Ring Not Empty bit in the Debug Capability Status Register (DCST)
    //  in order to detect whether or not we may have received a host connection

    do
    {
        dcst.value = mmio_read_u32(
            ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCST_OFFSET )
            );
    } while ( dcst.named_fields.event_ring_not_empty == 0 );

    // Poll the Current Connect Status Bit of DCPORTSC to check if we actually have a debug host connected

    do
    {
        dcportsc.value = mmio_read_u32(
            ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCPORTSC_OFFSET )
            );
    } while ( dcportsc.named_fields.current_connect_status == 0 );

    // After the Debug Host connection is detected, software shall wait for the Debug Device to be configured
    //  by the Debug Host. The transition of the DbC Run (DCR) bit to '1' indicates the successful configuration
    //  of the Debug Device

    do
    {
        dcctrl.value = mmio_read_u32(
            ( xhc_info->debug_cap_struct_base + XHCI_DBC_DCCTRL_OFFSET )
            );
    } while ( dcctrl.named_fields.dbc_run == 0 );

    // Note: if this process takes to long, toggle DCE off, then back on and try again...

    status = TRUE;

__cleanup:

    if ( status == FALSE )
    {
        if ( dbc_info->event_ring_segment_table_alloc.virtual_address != NULL )
        {
            mem_free( &dbc_info->event_ring_segment_table_alloc );
        }

        if ( dbc_info->event_ring_segment_trb_array.virtual_address != NULL )
        {
            mem_free( &dbc_info->event_ring_segment_trb_array );
        }

        if ( dbc_info->dbc_context.virtual_address != NULL )
        {
            mem_free( &dbc_info->dbc_context );
        }

        if ( dbc_info->string_0_descriptor.virtual_address != NULL )
        {
            mem_free( &dbc_info->string_0_descriptor );
        }

        if ( dbc_info->out_endpoint_context_trb_array.virtual_address != NULL )
        {
            mem_free( &dbc_info->out_endpoint_context_trb_array );
        }

        if ( dbc_info->in_endpoint_context_trb_array.virtual_address != NULL )
        {
            mem_free( &dbc_info->in_endpoint_context_trb_array );
        }
    }

    return status;
}
