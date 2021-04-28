/* allocate dynamic memory */
/* [TODO]: Implement a proper allocator */

Mem_Pool *
mem_pool_new(size_t size)
{
	static int ID = 0;
	Mem_Pool *pool = malloc(size + sizeof(Mem_Pool));
	if (pool == NULL) {
		/* [TODO]: (mem_pool) Error handling */
		return NULL;
	}
	pool->next = (uint8_t *)(pool + sizeof(Mem_Pool));
	pool->end = pool->next + size;
	pool->id = ID; ID++;
	return pool;
}

void
mem_pool_free(Mem_Pool *pool) {
	free(pool);
}

size_t
mem_pool_available(Mem_Pool *pool)
{
	return pool->end - pool->next;
}

size_t
mem_pool_allocated(Mem_Pool *pool)
{
	return (size_t)pool->next - (size_t)(pool + sizeof(Mem_Pool));
}

void *
mem_pool_push(Mem_Pool *pool, size_t size)
{
	if (mem_pool_available(pool) < size) {
		fprintf(ERR_LOG, "ERROR push size %ld exceeds available memory\n", size);
		fprintf(ERR_LOG, "INFO for mem pool id: %d\n", pool->id);
		exit(1); /* [NOTE]: exit on failure, I need to allocate more memory */
	}
	void *mem = (void *)pool->next;
	pool->next += size;
	return mem;
}

inline void 
mem_pool_pop_all(Mem_Pool *pool)
{
	mem_pool_pop(pool, mem_pool_allocated(pool));
}

void
mem_pool_pop(Mem_Pool *pool, size_t size)
{
	if ((size_t)(pool + sizeof(Mem_Pool)) > (size_t)(pool->next - size)) {
		fprintf(ERR_LOG, "WARNING pop size %ld, greater than allocated ammount\n", size);
		pool->next = (uint8_t *)(pool + sizeof(Mem_Pool));
	} else {
		pool->next -= size;
	}
}
