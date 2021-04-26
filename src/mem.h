#ifndef MEM_H_
#define MEM_H_

typedef struct {
	uint8_t *next;
	uint8_t *end;
} Mem_Pool;

static Mem_Pool *mem_pool_new(size_t size);
static void mem_pool_free(Mem_Pool *pool);
static size_t mem_pool_available(Mem_Pool *pool);
UNUSED_FUNC size_t mem_pool_size(Mem_Pool *pool);
static void *mem_pool_alloc(Mem_Pool *pool, size_t size);

#endif /* MEM_H_ */
