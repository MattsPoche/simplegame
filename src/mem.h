#ifndef MEM_H_
#define MEM_H_

#define MEM_POOL_TAG_LEN 24

typedef struct {
	int id;
	uint8_t *next;
	uint8_t *end;
} Mem_Pool;

typedef struct {
	Mem_Pool *model_pool;
	Mem_Pool *model_lookup;
	Mem_Pool *entity_pool;
	Mem_Pool *control_pool;
	Mem_Pool *audio_pool;
	Mem_Pool *temp;
} Game_Memory;

Mem_Pool *mem_pool_new(size_t size);
void mem_pool_free(Mem_Pool *pool);
size_t mem_pool_available(Mem_Pool *pool);
size_t mem_pool_allocated(Mem_Pool *pool);
void *mem_pool_push(Mem_Pool *pool, size_t size);
void mem_pool_pop(Mem_Pool *pool, size_t size);
void mem_pool_pop_all(Mem_Pool *pool);

#endif /* MEM_H_ */
