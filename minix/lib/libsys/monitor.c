
#include <minix/monitor.h>
#include <string.h>

#include "syslib.h"

static int do_invoke_monitor(message *m, int type)
{
	
	int r;
	

	r = _taskcall(MONITOR_PROC_NR, type, m);
	return r;
}

int monitor_check_address(void)
{
	message m;
	int r;

	memset(&m, 0, sizeof(m));
	r = do_invoke_monitor(&m, MONITOR_DO_CHECK_ADDRESS);	
	return r;
}
