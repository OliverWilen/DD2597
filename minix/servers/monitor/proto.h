#ifndef _MONITOR_PROTO_H
#define _MONITOR_PROTO_H

/* Function prototypes. */

/* main.c */
int main(int argc, char **argv);



int sef_cb_init_fresh(int type, sef_init_info_t *info);

/* monitor.c */
int do_check_address(message *m_ptr);
int do_virtio_init(message *m_ptr);
int do_virtio_to_queue(message *m_ptr);
int do_virtio_from_queue(message *m_ptr);
#endif
