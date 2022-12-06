
#include <minix/monitor.h>
#include <string.h>
#include "syslib.h"
#include <unistd.h>

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
	memcpy(&m.m_monitor_check_address.phys, &phys, sizeof(m.m_monitor_check_address.phys));
	r = do_invoke_monitor(&m, MONITOR_DO_CHECK_ADDRESS);	
	return r;
}
