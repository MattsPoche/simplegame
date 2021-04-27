#ifndef MEM_H_
#define MEM_H_

typedef struct {
	uint8_t *next;
	uint8_t *end;
} Mem_Pool;

typedef struct {
	Mem_Pool *block;
	Mem_Pool *temp;
} Game_Memory;

Mem_Pool *mem_pool_new(size_t size);
void mem_pool_free(Mem_Pool *pool);
size_t mem_pool_available(Mem_Pool *pool);
void *mem_pool_push(Mem_Pool *pool, size_t size);
void mem_pool_pop(Mem_Pool *pool, size_t size);

#endif /* MEM_H_ */
