/* allocate dynamic memory */
/* [TODO]: Implement a proper allocator */

Mem_Pool *
mem_pool_new(size_t size)
{
	Mem_Pool *pool = malloc(size + sizeof(Mem_Pool));
	if (pool == NULL) {
		/* [TODO]: (mem_pool) Error handling */
		return NULL;
	}
	pool->next = (uint8_t *)(pool + sizeof(Mem_Pool));
	pool->end = pool->next + size;
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

void *
mem_pool_push(Mem_Pool *pool, size_t size)
{
	if (mem_pool_available(pool) < size) {
		return NULL;
	}
	void *mem = (void *)pool->next;
	pool->next += size;
	return mem;
}

void
mem_pool_pop(Mem_Pool *pool, size_t size)
{
	if ((size_t)(pool + sizeof(Mem_Pool)) > (size_t)(pool->next - size)) {
		pool->next = (uint8_t *)(pool + sizeof(Mem_Pool));
	} else {
		pool->next -= size;
	}
}
