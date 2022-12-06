#include "inc.h"
#include "monitor.h"


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
int do_check_address(message *m_ptr)
{
	
	printf("Check address: %lu\n", m_ptr->m_monitor_check_address.phys[1].vp_addr);
  return(OK);
}


