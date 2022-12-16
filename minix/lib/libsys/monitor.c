
#include <minix/monitor.h>
#include <string.h>
#include "syslib.h"

static int do_invoke_monitor(message *m, int type)
{
	
	int r;
	

	r = _taskcall(MONITOR_PROC_NR, type, m);
	return r;
}

int monitor_check_address(struct vumap_phys phys[])
{
	message m;
	int r;	
	
	memset(&m, 0, sizeof(m));
	memcpy(m.m_monitor_check_address.phys, phys, sizeof(m.m_monitor_check_address.phys));

	r = do_invoke_monitor(&m, MONITOR_CHECK_ADDRESS);	
	return r;
}
int monitor_virtio_init()
{
	message m;
	int r;	
	memset(&m, 0, sizeof(m));
	r = do_invoke_monitor(&m, MONITOR_VIRTIO_INIT);	
	return r;
}

int monitor_virtio_to_queue(struct vumap_phys phys[])
{
	message m;
	int r;	
	
	memset(&m, 0, sizeof(m));
	memcpy(m.m_monitor_check_address.phys, phys, sizeof(m.m_monitor_check_address.phys));
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