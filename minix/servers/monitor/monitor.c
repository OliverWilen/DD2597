#include "inc.h"
#include "monitor.h"
#include <minix/vm.h>
#include <minix/virtio.h>
#include <minix/netdriver.h>


/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *info)
{	

	return(OK);
}

/*===========================================================================*
 *				do_publish				     *
 *===========================================================================*/
int do_check_address(message *m_ptr) {	
	vir_bytes next = 0;
	struct vm_region_info vri[MAX_VRI_COUNT];
	phys_bytes checkBase = m_ptr->m_monitor_check_address.phys[1].vp_addr; //Physical address of start of data buffer
	phys_bytes checkLimit = m_ptr->m_monitor_check_address.phys[1].vp_size + checkBase; //Physical address of end of data buffer
	phys_bytes checkBaseHdr = m_ptr->m_monitor_check_address.phys[0].vp_addr; //Physical address of start of header buffer
	phys_bytes checkLimitHdr = m_ptr->m_monitor_check_address.phys[0].vp_size + checkBaseHdr; //Physical address of end of header buffer
	endpoint_t who_e = m_ptr->m_source; //Sender/source endpoint number
	

	int r;

	int hdr_ok = 0;
	int data_ok = 0;

	do{
		r = vm_info_phys_region(who_e, vri, MAX_VRI_COUNT, &next);
		for(int i = 0; i < r; i++){
			if(checkBaseHdr >= vri[i].vri_addr && checkLimitHdr <= vri[i].vri_addr + vri[i].vri_length){
				//Address of header is within the right memory space
				hdr_ok = 1;
			}
			if(checkBase >= vri[i].vri_addr && checkLimit <= vri[i].vri_addr + vri[i].vri_length){
				//Address of data buffer is within the right memory space
				data_ok = 1;
			}
			if(hdr_ok && data_ok){
				//both conditions are ok
				return(OK);
			}			
		}
	}while(r == MAX_VRI_COUNT);

	
  return(EFAULT);
}
