global-incdirs-y += include cfile/include/ cfile/include/impl/ cfile/src/internal/
srcs-y += float.c printf.c

include ./sub_c_ext.mk

include ./sub_math.mk

# To remove a certain compiler flag, add a line like this
#cflags-template_ta.c-y += -Wno-strict-prototypes
