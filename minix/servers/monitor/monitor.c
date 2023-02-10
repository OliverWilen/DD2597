#include "inc.h"
#include "monitor.h"
#include <minix/vm.h>
#include <minix/virtio.h>
#include <minix/netdriver.h>
#include "virtio_ring.h"
#include <minix/syslib.h>


int *secret;
phys_bytes psecret;
/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *info)
{	
	secret = alloc_contig(sizeof(int), AC_ALIGN4K, &psecret);
	memset(secret, 0x01, sizeof(int));
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
	phys_bytes checkBaseHdr = m_ptr->m_monitor_check_address.phys[0].vp_addr;
	phys_bytes checkLimitHdr = m_ptr->m_monitor_check_address.phys[0].vp_size + checkBase;
	endpoint_t who_e = m_ptr->m_source;
	//printf("\n Monitor check address: %lx-%lx size: %d for process: %d\n", checkBase, checkLimit, m_ptr->m_monitor_check_address.phys[1].vp_size, who_e);
	printf("PSECRET: %lx\n SECRETVAR: %p\n SECRETPOINTER: %d\n", psecret, secret, *secret);
	//printf("\ncB: %lx, cL: %lx\ncBhdr: %lx, cLhdr: %lx", checkBase, checkLimit, checkBaseHdr, checkLimitHdr);
	int r;

	int hdr_ok = 0;
	int data_ok = 0;

	struct virtio_device *net_dev = m_ptr->m_monitor_check_address.dev;
	int qidx = m_ptr->m_monitor_check_address.qidx;
	//printf("queue addr: %lu", get_virtio_queue_address(net_dev, qidx));

	do{
		r = vm_info_phys_region(who_e, vri, MAX_VRI_COUNT, &next);
		for(int i = 0; i < r; i++){
			if(checkBaseHdr >= vri[i].vri_addr && checkLimitHdr <= vri[i].vri_addr + vri[i].vri_length){
				//printf("Address belongs to  %lx-%lx\n", vri[i].vri_addr, vri[i].vri_addr + vri[i].vri_length);
				hdr_ok = 1;
			}
			if(checkBase >= vri[i].vri_addr && checkLimit <= vri[i].vri_addr + vri[i].vri_length){
				//printf("Address belongs to  %lx-%lx\n", vri[i].vri_addr, vri[i].vri_addr + vri[i].vri_length);
				data_ok = 1;
			}
			if(hdr_ok && data_ok){
				return(OK);
			}			
		}
	}while(r == MAX_VRI_COUNT);

	
  return(EFAULT);
}

int do_virtio_to_queue(message *m_ptr) {
	struct virtio_device *dev;
	struct virtio_device *net_dev = m_ptr->m_monitor_check_address.dev;
	int qidx = m_ptr->m_monitor_check_address.qidx;
	struct vumap_phys *bufs = m_ptr->m_monitor_check_address.phys;
	size_t num = m_ptr->m_monitor_check_address.num;
	void *data= m_ptr->m_monitor_check_address.data;

	struct vring vring;
	int ret;
	cp_grant_id_t grantID = m_ptr->m_monitor_check_address.grantID;
	cp_grant_id_t grantID_2 = m_ptr->m_monitor_check_address.grantID_2;
	endpoint_t who_e = m_ptr->m_source;

	int dev_size = get_device_size(dev);
	
	ret = sys_safecopyfrom(who_e, grantID, 0, (vir_bytes) &dev, dev_size);
	//print_device_name(dev);
	
	printf("Ret: %d\n", ret);
	u32_t ring_size = get_device_ring_size(dev, qidx);
	printf("VRING NUM BEFORE: %d\n", vring.num);
	ret = sys_safecopyfrom(who_e, grantID_2, 0, (vir_bytes) &vring, ring_size);
	printf("Ret2: %d\n", ret);
	printf("VRING NUM AFTER: %d\n", vring.num);
	virtio_to_queue(dev, qidx, bufs, num, data);
	//printf("VIRTIO: %d\n", m_ptr->m_monitor_check_address.qidx);
	return(OK);
}

int do_virtio_from_queue(message *m_ptr){
	return(OK);
}
