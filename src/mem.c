/* allocate dynamic memory */
static Mem_Pool *
mem_pool_new(size_t size)
{
	Mem_Pool *pool = malloc(size + sizeof(Mem_Pool));
	if (pool == NULL) {
		/* [TODO]: (mem_pool) Error handling */
		return NULL;
	}
	pool->next = (uint8_t *)pool;
	pool->end = pool->next + size;
	return pool;
}

static void
mem_pool_free(Mem_Pool *pool) {
	free(pool);
}

static size_t
mem_pool_available(Mem_Pool *pool)
{
	return pool->end - pool->next;
}

UNUSED_FUNC size_t
mem_pool_size(Mem_Pool *pool)
{
	return pool->end - (uint8_t *)pool;
}

static void *
mem_pool_alloc(Mem_Pool *pool, size_t size)
{
	if (mem_pool_available(pool) < size) {
		return NULL;
	}
	void *mem = (void *)pool->next;
	pool->next += size;
	return mem;
}

