#ifndef _STDIO_IMPL_H
#define _STDIO_IMPL_H

#include <stdio.h>
#include <type_impl.h>
//#include "syscall.h"

#define UNGET 8

#define FFINALLOCK(f) ((f)->lock>=0 ? __lockfile((f)) : 0)
#define FLOCK(f) int __need_unlock = ((f)->lock>=0 ? __lockfile((f)) : 0)
#define FUNLOCK(f) do { if (__need_unlock) __unlockfile((f)); } while (0)

#define F_PERM 1
#define F_NORD 4
#define F_NOWR 8
#define F_EOF 16
#define F_ERR 32
#define F_SVB 64
#define F_APP 128

struct _IO_FILE {
	unsigned flags;
	unsigned char *rpos, *rend;
	int (*close)(FILE_IMPL *);
	unsigned char *wend, *wpos;
	unsigned char *mustbezero_1;
	unsigned char *wbase;
	size_t (*read)(FILE_IMPL *, unsigned char *, size_t);
	size_t (*write)(FILE_IMPL *, const unsigned char *, size_t);
	off_t (*seek)(FILE_IMPL *, off_t, int);
	unsigned char *buf;
	size_t buf_size;
	FILE_IMPL *prev, *next;
	int fd;
	int pipe_pid;
	long lockcount;
	int mode;
	volatile int lock;
	int lbf;
	void *cookie;
	off_t off;
	char *getln_buf;
	void *mustbezero_2;
	unsigned char *shend;
	off_t shlim, shcnt;
	FILE_IMPL *prev_locked, *next_locked;
	struct __locale_struct *locale;
};

extern hidden FILE_IMPL *volatile __stdin_used;
extern hidden FILE_IMPL *volatile __stdout_used;
extern hidden FILE_IMPL *volatile __stderr_used;

hidden int __lockfile(FILE_IMPL *);
hidden void __unlockfile(FILE_IMPL *);

hidden size_t __stdio_read(FILE_IMPL *, unsigned char *, size_t);
hidden size_t __stdio_write(FILE_IMPL *, const unsigned char *, size_t);
hidden size_t __stdout_write(FILE_IMPL *, const unsigned char *, size_t);
hidden off_t __stdio_seek(FILE_IMPL *, off_t, int);
hidden int __stdio_close(FILE_IMPL *);

hidden int __toread(FILE_IMPL *);
hidden int __towrite(FILE_IMPL *);

hidden void __stdio_exit(void);
hidden void __stdio_exit_needed(void);

#if defined(__PIC__) && (100*__GNUC__+__GNUC_MINOR__ >= 303)
__attribute__((visibility("protected")))
#endif
int __overflow(FILE_IMPL *, int), __uflow(FILE_IMPL *);

hidden int __fseeko(FILE_IMPL *, off_t, int);
hidden int __fseeko_unlocked(FILE_IMPL *, off_t, int);
hidden off_t __ftello(FILE_IMPL *);
hidden off_t __ftello_unlocked(FILE_IMPL *);
hidden size_t __fwritex(const unsigned char *, size_t, FILE_IMPL *);
hidden int __putc_unlocked(int, FILE_IMPL *);

hidden FILE_IMPL *__fdopen(int, const char *);
hidden int __fmodeflags(const char *);

hidden FILE_IMPL *__ofl_add(FILE_IMPL *f);
hidden FILE_IMPL **__ofl_lock(void);
hidden void __ofl_unlock(void);

struct __pthread;
hidden void __register_locked_file(FILE_IMPL *, struct __pthread *);
hidden void __unlist_locked_file(FILE_IMPL *);
hidden void __do_orphaned_stdio_locks(void);

#define MAYBE_WAITERS 0x40000000

hidden void __getopt_msg(const char *, const char *, const char *, size_t);

#define feof(f) ((f)->flags & F_EOF)
#define ferror(f) ((f)->flags & F_ERR)

#define getc_unlocked(f) \
	( ((f)->rpos != (f)->rend) ? *(f)->rpos++ : __uflow((f)) )

#define putc_unlocked(c, f) \
	( (((unsigned char)(c)!=(f)->lbf && (f)->wpos!=(f)->wend)) \
	? *(f)->wpos++ = (unsigned char)(c) \
	: __overflow((f),(unsigned char)(c)) )

/* Caller-allocated FILE * operations */
hidden FILE_IMPL *__fopen_rb_ca(const char *, FILE_IMPL *, unsigned char *, size_t);
hidden int __fclose_ca(FILE_IMPL *);


int vfscanf(FILE_IMPL *__restrict, const char *__restrict, __isoc_va_list);


#undef EOF
#define EOF (-1)

#endif
