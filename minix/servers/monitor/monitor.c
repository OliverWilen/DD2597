#include "inc.h"
#include "monitor.h"
#include <minix/vm.h>
#include <minix/virtio.h>
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
	phys_bytes checkBase = m_ptr->m_monitor_check_address.phys[1].vp_addr;
	phys_bytes checkLimit = m_ptr->m_monitor_check_address.phys[1].vp_size + checkBase;
	endpoint_t who_e = m_ptr->m_source;
	
	//printf("\n Monitor check address: %lx-%lx size: %d for process: %d\n", checkBase, checkLimit, m_ptr->m_monitor_check_address.phys[1].vp_size, who_e);
	
	int r;
	do{
		r = vm_info_phys_region(who_e, vri, MAX_VRI_COUNT, &next);
		for(int i = 0; i < r; i++){
			if(checkBase >= vri[i].vri_addr && checkLimit <= vri[i].vri_addr + vri[i].vri_length){
				//printf("Address belongs to  %lx-%lx\n", vri[i].vri_addr, vri[i].vri_addr + vri[i].vri_length);
				return(OK);
			}
			
		}
	}while(r == MAX_VRI_COUNT);

	
  return(EFAULT);
}
int do_virtio_to_queue(message *m_ptr) {
	struct virtio_device *dev;
	int ret;

	cp_grant_id_t grantID = m_ptr->m_monitor_check_address.grantID;
	endpoint_t who_e = m_ptr->m_source;
	printf("%p\n", &dev);
	printf("%p\n", dev);
	ret = sys_safecopyfrom(who_e, grantID, 0, (vir_bytes) &dev, 48);
	printf("%p\n", &dev);
	printf("%p\n", dev);

	if (ret != OK){
		printf("SAFECOPY FAILED: %d", ret);
		//return ret;
	}

	int qidx = m_ptr->m_monitor_check_address.qidx;
	struct vumap_phys *bufs = m_ptr->m_monitor_check_address.phys;
	size_t num = m_ptr->m_monitor_check_address.num;
	void *data= m_ptr->m_monitor_check_address.data;
	virtio_to_queue(dev, qidx, bufs, num, data);
	printf("VIRTIO: %d\n", m_ptr->m_monitor_check_address.qidx);
	return(OK);
}

int do_virtio_from_queue(message *m_ptr) {
	printf("VIRTIO FROM QUEUE");
	return(OK);
}

