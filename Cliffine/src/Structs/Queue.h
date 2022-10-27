#pragma once

#include "Defines.h"
#include "Core/Mem.h"
#include "Core/Asserts.h"

namespace Clf
{
	//First-in. First-out.
	template<typename T>
	struct Queue
	{
	private:
		T* buffer = nullptr;
		u32 in = 0;
		u32 out = 0;
		u32 len = 0;

		u32 maxLen = 0;
		u32 incLen = 4;
	public:
		Queue(T* buffer, u32 len) : len(len), maxLen(len) {
			this->buffer = (T*)Mem::Alloc(Mem::MEM_TYPE_QUEUE, sizeof(T) * len);
			Mem::Copy(this->buffer, buffer, sizeof(T) * len);
		}
		~Queue() {
			Mem::Free(Mem::MEM_TYPE_QUEUE, this->buffer, sizeof(T) * this->maxLen);
		}
		Queue() = default;

		const u32 GetLen() const { return len; }

		T& operator[](i32 index)
		{
			if (index < 0)
			{
				CLF_ASSERT(-index >= this->len, "Index out of bounds");
				return this->operator[](len + index);
			}

			CLF_ASSERT(index < this->len, "Index out of bounds");
			return this->buffer[(this->out + index) % this->maxLen];
		}

		void operator+=(T elem) {
			if (this->len == this->maxLen) {
				u32 maxLen = this->maxLen + this->incLen;
				T* buffer = (T*)Mem::Alloc(Mem::MEM_TYPE_QUEUE, sizeof(T) * maxLen);

				if (this->in > this->out) {
					Mem::Copy(buffer, this->buffer + out, sizeof(T) * this->len);
				}
				else {
					u32 outToEnd = this->len - this->out;
					Mem::Copy(buffer, this->buffer + this->out, sizeof(T) * outToEnd);
					Mem::Copy(buffer + outToEnd, this->buffer, sizeof(T) * this->in);
				}
				this->~Queue();
				this->buffer = buffer;
				this->in = len;
				this->out = 0;
				this->maxLen = maxLen;
			}

			this->len++;
			this->buffer[this->in] = elem;
			this->in = (this->in + 1) % this->maxLen;
		}
		void operator--(int) {
			CLF_ASSERT(this->len > 0, "Cannot pop empty queue");
			this->len--;
			this->buffer[this->out].~T();
			this->out = (this->out + 1) % this->maxLen;
		}

		const bool operator>(const T& elem) {
			u32 i;

			for (i = this->out; i < this->maxLen; i++)
				if (this->buffer[i] == elem)
					return true;

			for (i = 0; i < this->in; i++)
				if (this->buffer[i] == elem)
					return true;

			return false;
		}

		friend const bool operator<(const T& elem, const Queue& queue) { return queue > elem; }
	};
}