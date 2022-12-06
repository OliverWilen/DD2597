/* Copied and altered Data Store Server. 
 * This service implements a little publish/subscribe data store that is 
 * crucial for the system's fault tolerance. Components that require state
 * can store it here, for later retrieval, e.g., after a crash and subsequent
 * restart by the reincarnation server. 
 * 
 * Created:
 *   Oct 19, 2005	by Jorrit N. Herder
 */

#include "inc.h"	/* include master header file */
#include <minix/endpoint.h>

/* Allocate space for the global variables. */
static endpoint_t who_e;	/* caller's proc number */
static int callnr;		/* system call number */

/* Declare some local functions. */
static void get_work(message *m_ptr);
static void reply(endpoint_t whom, message *m_ptr);

/* SEF functions and variables. */
static void sef_local_startup(void);

/*===========================================================================*
 *				main                                         *
 *===========================================================================*/
int main(int argc, char **argv)
{
/* This is the main routine of this service. The main loop consists of 
 * three major activities: getting new work, processing the work, and
 * sending the reply. The loop never terminates, unless a panic occurs.
 */
  message m;
  int result;                 
  printf("Monitor started\n");

  /* SEF local startup. */
  env_setargs(argc, argv);
  sef_local_startup();

  /* Main loop - get work and do it, forever. */         
  while (TRUE) {              

      /* Wait for incoming message, sets 'callnr' and 'who'. */
      get_work(&m);

      if (is_notify(callnr)) {
          printf("Monitor: warning, got illegal notify from: %d\n", m.m_source);
          result = EINVAL;
          goto send_reply;
      }
        //endpoint_t endP;
        //int k = getprocnr((pid_t)256, &endP);
        //printf("k =  %d endpoint = %d, this: %d \n", k, endP, who_e);
	    phys_bytes add = m.m_monitor_check_address.phys[1].vp_addr;
	    phys_bytes adLen = m.m_monitor_check_address.phys[1].vp_size;
        int v = 0;
      switch (callnr) {
      case MONITOR_DO_CHECK_ADDRESS:
 	    v = sys_privquery_mem(who_e, add, adLen);
        printf("V: %d\n", v);
          result = do_check_address(&m);
          break;
      
      default: 
          printf("Monitor: warning, got illegal request from %d\n", m.m_source);
          result = EINVAL;
      }

send_reply:
      /* Finally send reply message, unless disabled. */
      if (result != EDONTREPLY) {
          m.m_type = result;  		/* build reply message */
	  reply(who_e, &m);		/* send it away */
      }
  }
  return(OK);				/* shouldn't come here */
}

/*===========================================================================*
 *			       sef_local_startup			     *
 *===========================================================================*/
static void sef_local_startup()
{
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_restart(sef_cb_init_fresh);


  /* Let SEF perform startup. */
  sef_startup();
}

/*===========================================================================*
 *				get_work                                     *
 *===========================================================================*/
static void get_work(
  message *m_ptr			/* message buffer */
)
{
    int status = sef_receive(ANY, m_ptr);   /* blocks until message arrives */
    if (OK != status)
        panic("failed to receive message!: %d", status);
    who_e = m_ptr->m_source;        /* message arrived! set sender */
    callnr = m_ptr->m_type;       /* set function call number */
}

/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
static void reply(
  endpoint_t who_e,			/* destination */
  message *m_ptr			/* message buffer */
)
{
    int s = ipc_send(who_e, m_ptr);    /* send the message */
    if (OK != s)
        printf("Monitor: unable to send reply to %d: %d\n", who_e, s);
}

