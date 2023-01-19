
#include <minix/monitor.h>
#include <string.h>
#include "syslib.h"

static int do_invoke_monitor(message *m, int type)
{
	
	int r;
	

	r = _taskcall(MONITOR_PROC_NR, type, m);
	return r;
}

int monitor_virtio_to_queue(void *dev, int qidx, struct vumap_phys phys[],
	size_t num, void *data, cp_grant_id_t grantID, cp_grant_id_t grantID_2)
{
	//printf("Grantid: %lu",grantID);
	message m;
	int r;	
	memset(&m, 0, sizeof(m));
	memcpy(m.m_monitor_check_address.phys, phys, sizeof(m.m_monitor_check_address.phys));
	m.m_monitor_check_address.dev = dev;
	m.m_monitor_check_address.qidx = qidx;
	m.m_monitor_check_address.num = num;
	m.m_monitor_check_address.data = data;
	m.m_monitor_check_address.grantID = grantID;
	m.m_monitor_check_address.grantID_2 = grantID_2;
	r = do_invoke_monitor(&m, MONITOR_VIRTIO_TO_QUEUE);	
	return r;
}

int monitor_virtio_from_queue(struct vumap_phys phys[])
{
	message m;
	int r;	
	
	memset(&m, 0, sizeof(m));
	memcpy(m.m_monitor_check_address.phys, phys, sizeof(m.m_monitor_check_address.phys));

	r = do_invoke_monitor(&m, MONITOR_VIRTIO_FROM_QUEUE);	
	return r;

	
}

int monitor_alloc_contig(int packet_buf_size)
{
	message m;
	int r;	
	
	memset(&m, 0, sizeof(m));
	m.m_monitor_check_address.qidx = packet_buf_size;

	r = do_invoke_monitor(&m, MONITOR_ALLOC_CONTIG);	
	return r;
	
}