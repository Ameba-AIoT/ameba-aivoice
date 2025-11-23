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

#include "parcel.h"

#include <string.h>
#include <stdlib.h>

#include "voice_utils.h"

struct Parcel {
	uint8_t *data;
	size_t read_cursor;
	size_t write_cursor;
	size_t data_size;
	size_t data_capacity;
	size_t max_data_capacity;
	release_func owner;
};

size_t Parcel_GetWritableBytes(Parcel *parcel);
size_t Parcel_GetReadableBytes(Parcel *parcel);
size_t Parcel_CalculateNewCapacity(Parcel *parcel, size_t min_capacity);
bool Parcel_CheckCapacity(Parcel *parcel, size_t desire_capacity);
bool Parcel_WriteDataBytes(Parcel *parcel, const void *data, size_t size);

#define BASIC_TYPE(NAME, TYPENAME)                                     \
bool Write##NAME(Parcel *parcel, TYPENAME value) {                     \
    size_t desire_capacity = sizeof(TYPENAME);                         \
    size_t delta = parcel->write_cursor % desire_capacity;             \
    LOGV("write_cursor: %d, desire_capacity: %d, delta: %d\n",         \
            parcel->write_cursor, desire_capacity, delta);             \
    if (delta > 0) {                                                   \
        parcel->write_cursor += desire_capacity - delta;               \
        parcel->data_size += desire_capacity - delta;                  \
    }                                                                  \
    LOGV("write_cursor: %d, desire_capacity: %d\n",                    \
            parcel->write_cursor, desire_capacity);                    \
    if (Parcel_CheckCapacity(parcel, desire_capacity)) {               \
        *(TYPENAME *)(parcel->data + parcel->write_cursor) = value;    \
        parcel->write_cursor += desire_capacity;                       \
        parcel->data_size += desire_capacity;                          \
        if (parcel->owner) {                                           \
            parcel->owner(parcel, parcel->data, parcel->data_size);    \
            parcel->owner = NULL;                                      \
        }                                                              \
        return true;                                                   \
    }                                                                  \
    return false;                                                      \
}                                                                      \
                                                                       \
TYPENAME Read##NAME(Parcel *parcel) {                                  \
    TYPENAME value;                                                    \
    size_t desire_capacity = sizeof(TYPENAME);                         \
    size_t delta = parcel->read_cursor % desire_capacity;              \
    LOGV("read_cursor: %d, desire_capacity: %d, delta: %d\n",          \
            parcel->read_cursor, desire_capacity, delta);              \
    if (delta > 0)                                                     \
        parcel->read_cursor += desire_capacity - delta;                \
    LOGV("read_cursor: %d, desire_capacity: %d\n",                     \
            parcel->read_cursor, desire_capacity);                     \
    if (desire_capacity <= Parcel_GetReadableBytes(parcel)) {          \
        const void *data = parcel->data + parcel->read_cursor;         \
        parcel->read_cursor += desire_capacity;                        \
        value = *(TYPENAME *)(data);                                   \
        return value;                                                  \
    }                                                                  \
    return 0;                                                          \
}

BASIC_TYPE(Bool, int32_t)
BASIC_TYPE(Int8, int8_t)
BASIC_TYPE(Int16, int16_t)
BASIC_TYPE(Int32, int32_t)
BASIC_TYPE(Int64, int64_t)
BASIC_TYPE(Uint8, uint8_t)
BASIC_TYPE(Uint16, uint16_t)
BASIC_TYPE(Uint32, uint32_t)
BASIC_TYPE(Uint64, uint64_t)
BASIC_TYPE(Float, float)
BASIC_TYPE(Double, double)
BASIC_TYPE(Pointer, void *)

static const size_t DEFAULT_CPACITY = 2 * 1024; // 2K

// ----------------------------------------------------------------------
// Private Interfaces
size_t Parcel_GetWritableBytes(Parcel *parcel)
{
	if (parcel && (parcel->data_capacity > parcel->write_cursor)) {
		return parcel->data_capacity - parcel->write_cursor;
	}

	return 0;
}

size_t Parcel_GetReadableBytes(Parcel *parcel)
{
	if (parcel && (parcel->data_size > parcel->read_cursor)) {
		return parcel->data_size - parcel->read_cursor;
	}

	return 0;
}

size_t Parcel_CalculateNewCapacity(Parcel *parcel, size_t min_capacity)
{
	if (!parcel) {
		return 0;
	}

	size_t capacity = 64;
	while (capacity < min_capacity) {
		capacity = capacity * 2;
	}

	if ((parcel->max_data_capacity > 0) && (capacity > parcel->max_data_capacity)) {
		capacity = parcel->max_data_capacity;
	}

	return capacity;
}

bool Parcel_CheckCapacity(Parcel *parcel, size_t desire_capacity)
{
	if (!parcel) {
		return false;
	}

	if (desire_capacity <= Parcel_GetWritableBytes(parcel)) {
		LOGV("Check capacity ok! desire_capacity: %d is less than writable bytes: %d", desire_capacity, Parcel_GetWritableBytes(parcel));
		return true;
	}

	size_t min_new_capacity = desire_capacity + parcel->write_cursor;
	size_t new_capacity = Parcel_CalculateNewCapacity(parcel, min_new_capacity);
	if ((new_capacity <= parcel->data_capacity) || (new_capacity < min_new_capacity)) {
		LOGV("Failed to check parcel capacity, new_capacity = %d,"
			 "parcel data_capacity = %d, min_new_capacity = %d",
			 new_capacity, parcel->data_capacity, min_new_capacity);
		return false;
	}

	LOGV("new_capacity: %d, min_new_capacity: %d", new_capacity, min_new_capacity);

	void *new_data = realloc(parcel->data, new_capacity);
	if (new_data != NULL) {
		parcel->data = (uint8_t *)new_data;
		parcel->data_capacity = new_capacity;
		LOGV("new_capacity: %d realloced ok!", new_capacity);
		return true;
	}

	LOGV("Failed to realloc parcel capacity, new_capacity = %d, parcel->data_capacity = %d",
		 new_capacity, parcel->data_capacity);
	return false;
}

bool Parcel_WriteDataBytes(Parcel *parcel, const void *data, size_t size)
{
	void *dest = parcel->data + parcel->write_cursor;
	size_t writable_bytes = Parcel_GetWritableBytes(parcel);
	if (size > writable_bytes) {
		return false;
	}

	memcpy(dest, data, size);
	parcel->write_cursor += size;
	parcel->data_size += size;
	return true;
}


// ----------------------------------------------------------------------
// Parcel
Parcel *Parcel_Create(void)
{
	Parcel *parcel = malloc(sizeof(Parcel));
	memset(parcel, 0, sizeof(Parcel));
	if (!parcel) {
		LOGE("Fail to alloc parcel.");
		return NULL;
	}

	parcel->max_data_capacity = DEFAULT_CPACITY;
	parcel->owner = NULL;
	return parcel;
}

void Parcel_IpcSetData(Parcel *parcel, uint8_t *data, size_t data_size, release_func rel_func)
{
	if (parcel->data) {
		free(parcel->data);
	}

	parcel->data = data;
	parcel->data_size  = data_size;
	parcel->owner = rel_func;
}

uint8_t *Parcel_IpcData(Parcel *parcel)
{
	if (!parcel) {
		return NULL;
	} else {
		return parcel->data;
	}
}

size_t Parcel_IpcDataSize(Parcel *parcel)
{
	if (!parcel) {
		return 0;
	} else {
		return parcel->data_size;
	}
}


void Parcel_Destroy(Parcel *parcel)
{
	if (parcel) {
		if (parcel->data) {
			if (!parcel->owner) {
				free(parcel->data);
			} else {
				parcel->owner(parcel, parcel->data, parcel->data_size);
				parcel->owner = NULL;
			}
		}
		free((void *)parcel);
	}
}

bool Parcel_WriteBool(Parcel *parcel, bool value)
{
	return WriteBool(parcel, (int32_t)value);
}

bool Parcel_ReadBool(Parcel *parcel)
{
	int32_t temp = ReadBool(parcel);
	return (temp != 0);
}

bool Parcel_WriteInt8(Parcel *parcel, int8_t value)
{
	return WriteInt8(parcel, value);
}

int8_t Parcel_ReadInt8(Parcel *parcel)
{
	return ReadInt8(parcel);
}

bool Parcel_WriteInt16(Parcel *parcel, int16_t value)
{
	return WriteInt16(parcel, value);
}

int16_t Parcel_ReadInt16(Parcel *parcel)
{
	return ReadInt16(parcel);
}

bool Parcel_WriteInt32(Parcel *parcel, int32_t value)
{
	return WriteInt32(parcel, value);
}

int32_t Parcel_ReadInt32(Parcel *parcel)
{
	return ReadInt32(parcel);
}

bool Parcel_WriteInt64(Parcel *parcel, int64_t value)
{
	return WriteInt64(parcel, value);
}

int64_t Parcel_ReadInt64(Parcel *parcel)
{
	return ReadInt64(parcel);
}

bool Parcel_WriteUint8(Parcel *parcel, uint8_t value)
{
	return WriteUint8(parcel, value);
}

uint8_t Parcel_ReadUint8(Parcel *parcel)
{
	return ReadUint8(parcel);
}

bool Parcel_WriteUint16(Parcel *parcel, uint16_t value)
{
	return WriteUint16(parcel, value);
}

uint16_t Parcel_ReadUint16(Parcel *parcel)
{
	return ReadUint16(parcel);
}

bool Parcel_WriteUint32(Parcel *parcel, uint32_t value)
{
	return WriteUint32(parcel, value);
}

uint32_t Parcel_ReadUint32(Parcel *parcel)
{
	return ReadUint32(parcel);
}

bool Parcel_WriteUint64(Parcel *parcel, uint64_t value)
{
	return WriteUint64(parcel, value);
}

uint64_t Parcel_ReadUint64(Parcel *parcel)
{
	return ReadUint64(parcel);
}

bool Parcel_WriteFloat(Parcel *parcel, float value)
{
	return WriteFloat(parcel, value);
}

float Parcel_ReadFloat(Parcel *parcel)
{
	return ReadFloat(parcel);
}

bool Parcel_WriteDouble(Parcel *parcel, double value)
{
	return WriteDouble(parcel, value);
}

double Parcel_ReadDouble(Parcel *parcel)
{
	return ReadDouble(parcel);
}

bool Parcel_WritePointer(Parcel *parcel, void *value)
{
	return WritePointer(parcel, value);
}

void *Parcel_ReadPointer(Parcel *parcel)
{
	return ReadPointer(parcel);
}

bool Parcel_WriteBuffer(Parcel *parcel, void *data, size_t size)
{
	if (data == NULL || size == 0) {
		return false;
	}

	if (Parcel_CheckCapacity(parcel, size)) {
		if (!Parcel_WriteDataBytes(parcel, data, size)) {
			return false;
		}
		if (parcel->owner) {
			parcel->owner(parcel, parcel->data, parcel->data_size);
			parcel->owner = NULL;
		}
		return true;
	}

	return false;
}

void *Parcel_ReadBuffer(Parcel *parcel, size_t length)
{
	if (Parcel_GetReadableBytes(parcel) >= length) {
		void *buffer = parcel->data + parcel->read_cursor;
		parcel->read_cursor += length;
		return (void *)buffer;
	}

	return NULL;
}

bool Parcel_WriteCString(Parcel *parcel, char *value)
{
	if (value == NULL) {
		return false;
	}
	size_t datalength = strlen(value);
	size_t desire_capacity = (datalength + 1) * sizeof(char);
	return Parcel_WriteBuffer(parcel, value, desire_capacity);
}

char *Parcel_ReadCString(Parcel *parcel)
{
	size_t old_cursor = parcel->read_cursor;
	size_t available = Parcel_GetReadableBytes(parcel);
	char *cstr = (char *)(parcel->data + parcel->read_cursor);
	char *eos = (char *)(memchr(cstr, 0, available));
	if (eos != NULL) {
		size_t datalength = (size_t)(eos - cstr);
		parcel->read_cursor += (datalength + 1);
		return cstr;
	}
	parcel->read_cursor = old_cursor;
	return NULL;
}
