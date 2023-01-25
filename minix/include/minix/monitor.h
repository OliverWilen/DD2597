#ifndef _MINIX_MONITOR_H
#define _MINIX_MONITOR_H

/* Types and constants shared between block and character drivers. */

//#define _POSIX_SOURCE	1	/* tell headers to include POSIX stuff */
//#define _NETBSD_SOURCE	1	/* tell headers to include MINIX stuff */
//#define _SYSTEM		1	/* get negative error number in <errno.h> */

/* The following are so basic, all the *.c files get them automatically. */
#include <minix/config.h>	/* MUST be first */
#include <minix/type.h>
#include <minix/ipc.h>
#include <minix/com.h>
#include <minix/callnr.h>
#include <sys/types.h>
#include <minix/const.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/endpoint.h>
#include <string.h>
#include <limits.h>
#include <stddef.h>
#include <errno.h>
#include <sys/param.h>
#include <minix/u64.h>
#include <minix/partition.h>

int monitor_virtio_to_queue(void *dev, int qidx, struct vumap_phys phys[],
	size_t num, void *data, cp_grant_id_t grantID, cp_grant_id_t grant_ID_2);
int monitor_virtio_from_queue(struct vumap_phys phys[]);
int monitor_check_address(struct vumap_phys phys[]);
#endif /* _MINIX_DRIVER_H */
