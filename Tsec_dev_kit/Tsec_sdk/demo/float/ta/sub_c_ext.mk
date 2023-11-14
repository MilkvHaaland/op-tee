
srcs-y += cfile/src/stdio/sscanf.c
srcs-y += cfile/src/stdio/vsscanf.c
srcs-y += cfile/src/stdio/vfscanf.c
srcs-y += cfile/src/stdio/__toread.c
srcs-y += cfile/src/stdio/__uflow.c
srcs-y += cfile/src/stdlib/strtod.c
srcs-y += cfile/src/stdlib/strtol.c
srcs-y += cfile/src/stdlib/atof.c
srcs-y += cfile/src/internal/shgetc.c
srcs-y += cfile/src/internal/intscan.c
srcs-y += cfile/src/internal/floatscan.c
#srcs-y += cfile/stdio/vscanf.c
srcs-y += cfile/src/multibyte/internal.c
srcs-y += cfile/src/multibyte/mbrtowc.c
srcs-y += cfile/src/multibyte/mbsinit.c
#fence
srcs-y += cfile/src/fenv/fegetexceptflag.c
#srcs-y += cfile/src/fenv/fenv.c
srcs-y += cfile/src/fenv/fesetround.c
srcs-y += cfile/src/fenv/__flt_rounds.c
srcs-y += cfile/src/fenv/feholdexcept.c
srcs-y += cfile/src/fenv/fesetexceptflag.c
srcs-y += cfile/src/fenv/feupdateenv.c
srcs-y += cfile/src/fenv/riscv64/fenv.S
srcs-y += cfile/src/fenv/riscv64/fenv-sf.c

# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
