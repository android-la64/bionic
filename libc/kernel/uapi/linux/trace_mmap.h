/*
 * This file is auto-generated. Modifications will be lost.
 *
 * See https://android.googlesource.com/platform/bionic/+/master/libc/kernel/
 * for more information.
 */
#ifndef _UAPI_TRACE_MMAP_H_
#define _UAPI_TRACE_MMAP_H_
#include <linux/types.h>
struct ring_buffer_meta {
  unsigned long entries;
  unsigned long overrun;
  unsigned long read;
  unsigned long pages_touched;
  unsigned long pages_lost;
  unsigned long pages_read;
  __u32 meta_page_size;
  __u32 nr_data_pages;
  struct reader_page {
    __u32 id;
    __u32 read;
    unsigned long lost_events;
  } reader_page;
};
#endif
