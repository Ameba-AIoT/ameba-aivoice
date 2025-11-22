/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "ring_buffer"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ring_buffer.h"

#define RB_LOGV(x, ...) printf("[%s][%s] " x, LOG_TAG, __func__, ##__VA_ARGS__)
#define RB_LOGD(x, ...) printf("[%s][%s] " x, LOG_TAG, __func__, ##__VA_ARGS__)
#define RB_LOGI(x, ...) printf("[%s][%s] " x, LOG_TAG, __func__, ##__VA_ARGS__)
#define RB_LOGW(x, ...) printf("[%s][%s] warn: " x, LOG_TAG, __func__, ##__VA_ARGS__)
#define RB_LOGE(x, ...) printf("[%s][%s] error: " x, LOG_TAG, __func__, ##__VA_ARGS__)

#define mb()   __sync_synchronize()
#define rmb()  __sync_synchronize()
#define wmb()  __sync_synchronize()

// ---------------------------------------------------------------
// Local RingBuffer
uint32_t local_ring_buffer_capacity(const struct ring_buffer *rb)
{
	return rb->header->capacity;
}

uint32_t local_ring_buffer_space(const struct ring_buffer *rb)
{
	return rb->header->capacity - (rb->header->head - rb->header->tail);
}

uint32_t local_ring_buffer_available(const struct ring_buffer *rb)
{
	return rb->header->head - rb->header->tail;
}

void local_ring_buffer_reset(struct ring_buffer *rb)
{
	rb->header->head = 0;
	rb->header->tail = 0;
}

uint32_t local_ring_buffer_write(struct ring_buffer *rb, const void *data, uint32_t count)
{
	uint32_t head = rb->header->head;
	uint32_t tail = rb->header->tail;  // Get snapshot of tail
	uint32_t space = rb->header->capacity - (head - tail);

	// Only do a full write, return 0
	// if space is not enough.
	if (count == 0 || count > space) {
		return 0;
	}

	uint32_t offset = head & rb->header->mask;
	uint32_t first_chunk = rb->header->capacity - offset;

	if (count <= first_chunk) {
		memcpy((void *)((char *)rb->header->buffer + offset), data, count);
	} else {
		memcpy((void *)((char *)rb->header->buffer + offset), data, first_chunk);
		memcpy((void *)(char *)rb->header->buffer, (void *)((char *)data + first_chunk), count - first_chunk);
	}

	wmb();
	rb->header->head += count;

	return count;
}

uint32_t
local_ring_buffer_read(struct ring_buffer *rb, void *data, uint32_t count)
{
	uint32_t head = rb->header->head;  // Get snapshot of head
	uint32_t tail = rb->header->tail;
	uint32_t available = head - tail;

	// Only do a full read, return 0
	// if available is not enough.
	if (count == 0 || count > available) {
		return 0;
	}

	uint32_t offset = tail & rb->header->mask;
	uint32_t first_chunk = rb->header->capacity - offset;

	if (count <= first_chunk) {
		memcpy(data, (void *)((char *)rb->header->buffer + offset), count);
	} else {
		memcpy(data, (void *)((char *)rb->header->buffer + offset), first_chunk);
		memcpy((void *)((char *)data + first_chunk), (void *)rb->header->buffer, count - first_chunk);
	}

	rmb();
	rb->header->tail += count;

	return count;
}

// ---------------------------------------------------------------
// IPC RingBuffer
uint32_t ipc_ring_buffer_capacity(const struct ring_buffer *rb)
{
	DCache_Invalidate((void *)rb->header, sizeof(ring_buffer_header));
	return rb->header->capacity;
}

uint32_t ipc_ring_buffer_space(const struct ring_buffer *rb)
{
	DCache_Invalidate((void *)rb->header, sizeof(ring_buffer_header));
	return rb->header->capacity - (rb->header->head - rb->header->tail);
}

uint32_t ipc_ring_buffer_available(const struct ring_buffer *rb)
{
	DCache_Invalidate((void *)rb->header, sizeof(ring_buffer_header));
	return rb->header->head - rb->header->tail;
}

void ipc_ring_buffer_reset(struct ring_buffer *rb)
{
	rb->header->head = 0;
	rb->header->tail = 0;
	DCache_Clean((void *)rb->header, sizeof(ring_buffer_header));
}

uint32_t ipc_ring_buffer_write(struct ring_buffer *rb, const void *data, uint32_t count)
{
	DCache_Invalidate((void *)rb->header, sizeof(ring_buffer_header));
	uint32_t head = rb->header->head;
	uint32_t tail = rb->header->tail;  // Get snapshot of tail
	uint32_t space = rb->header->capacity - (head - tail);

	// Only do a full write, return 0
	// if space is not enough.
	if (count == 0 || count > space) {
		return 0;
	}

	uint32_t offset = head & rb->header->mask;
	uint32_t first_chunk = rb->header->capacity - offset;

	if (count <= first_chunk) {
		memcpy((void *)((char *)rb->header->buffer + offset), data, count);
		DCache_Clean((void *)((char *)rb->header->buffer + offset), count);
	} else {
		memcpy((void *)((char *)rb->header->buffer + offset), data, first_chunk);
		DCache_Clean((void *)((char *)rb->header->buffer + offset), first_chunk);
		memcpy((void *)rb->header->buffer, (void *)((char *)data + first_chunk), count - first_chunk);
		DCache_Clean((void *)rb->header->buffer, count - first_chunk);
	}

	wmb();
	rb->header->head += count;

	DCache_Clean((void *)(&(rb->header->head)), CACHE_LINE_SIZE);
	return count;
}

uint32_t ipc_ring_buffer_read(struct ring_buffer *rb, void *data, uint32_t count)
{
	DCache_Invalidate((void *)rb->header, sizeof(ring_buffer_header));
	uint32_t head = rb->header->head;  // Get snapshot of head
	uint32_t tail = rb->header->tail;
	uint32_t available = head - tail;

	// Only do a full read, return 0
	// if available is not enough.
	if (count == 0 || count > available) {
		return 0;
	}

	uint32_t offset = tail & rb->header->mask;
	uint32_t first_chunk = rb->header->capacity - offset;

	if (count <= first_chunk) {
		memcpy(data, (void *)((char *)rb->header->buffer + offset), count);
		DCache_Invalidate((void *)((char *)rb->header->buffer + offset), count);
	} else {
		memcpy(data, (void *)((char *)rb->header->buffer + offset), first_chunk);
		DCache_Invalidate((void *)((char *)rb->header->buffer + offset), first_chunk);
		memcpy((void *)((char *)data + first_chunk), (void *)rb->header->buffer, count - first_chunk);
		DCache_Invalidate((void *)rb->header->buffer, count - first_chunk);
	}

	rmb();
	rb->header->tail += count;

	DCache_Clean((void *)(&(rb->header->tail)), CACHE_LINE_SIZE);
	return count;
}

struct ring_buffer *ring_buffer_create(uint32_t capacity, enum ring_buffer_type type)
{
	struct ring_buffer *rb;
	void *buffer;

	if ((capacity & (capacity - 1)) != 0) {
		RB_LOGE("Size must be power of two.\n");
		return NULL;
	}

	rb = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));
	if (!rb) {
		return NULL;
	}

	rb->header = (struct ring_buffer_header *)malloc(sizeof(struct ring_buffer_header));
	if (!rb->header) {
		return NULL;
	}

	buffer = malloc(capacity);
	if (!buffer) {
		free(rb);
		return NULL;
	}

	rb->header->buffer = buffer;
	rb->header->capacity = capacity;
	rb->header->mask = capacity - 1;
	rb->header->buffer_id = 0;
	rb->header->type = type;
	rb->header->head = 0;
	rb->header->tail = 0;

	if (type == RINGBUFFER_IPC) {
		rb->capacity = ipc_ring_buffer_capacity;
		rb->space = ipc_ring_buffer_space;
		rb->available = ipc_ring_buffer_available;
		rb->write = ipc_ring_buffer_write;
		rb->read = ipc_ring_buffer_read;
		rb->reset = ipc_ring_buffer_reset;
	} else {
		rb->capacity = local_ring_buffer_capacity;
		rb->space = local_ring_buffer_space;
		rb->available = local_ring_buffer_available;
		rb->write = local_ring_buffer_write;
		rb->read = local_ring_buffer_read;
		rb->reset = local_ring_buffer_reset;
	}

	return rb;
}

struct ring_buffer *ring_buffer_create_by_header(ring_buffer_header *header)
{
	struct ring_buffer *rb;
	rb = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));
	if (!rb) {
		return NULL;
	}


	if (header->type == RINGBUFFER_IPC) {
		DCache_Invalidate((void *)rb->header, sizeof(ring_buffer_header));
		rb->header = header;
		rb->header->buffer = header->buffer;
		rb->capacity = ipc_ring_buffer_capacity;
		rb->space = ipc_ring_buffer_space;
		rb->available = ipc_ring_buffer_available;
		rb->write = ipc_ring_buffer_write;
		rb->read = ipc_ring_buffer_read;
		rb->reset = ipc_ring_buffer_reset;
		DCache_Clean((void *)rb, sizeof(ring_buffer));
	} else {
		rb->header = header;
		rb->header->buffer = header->buffer;
		rb->capacity = local_ring_buffer_capacity;
		rb->space = local_ring_buffer_space;
		rb->available = local_ring_buffer_available;
		rb->write = local_ring_buffer_write;
		rb->read = local_ring_buffer_read;
		rb->reset = local_ring_buffer_reset;
	}

	return rb;
}

void ring_buffer_destroy(struct ring_buffer *rb)
{
	if (rb) {
		if (rb->header->buffer) {
			free((void *)rb->header->buffer);
		}
		free(rb);
	}
}
