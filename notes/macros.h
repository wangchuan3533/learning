#define ngx_align_ptr(p, a)                                                   \
           (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))
