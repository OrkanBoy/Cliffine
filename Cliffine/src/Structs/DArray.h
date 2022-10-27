#pragma once
#include "Defines.h"
#include "Core/Mem.h"

namespace Clf
{
	template<typename T>
	struct DArray
	{
	private:
		T* buffer = nullptr;
		u32 len = 0;

		u32 maxLen = 0;
		u32 incLen = 4;
	public:

		friend bool operator<(const T& elem, const DArray& arr) { return arr > elem; }

		DArray(u32 maxLen) : maxLen(maxLen) {
			this->buffer = (T*)Mem::Alloc(Mem::MEM_TYPE_DARRAY, sizeof(T) * maxLen);
		}

		DArray(T* buffer, u32 len) : len(len), maxLen(len) {
			u64 bytes = sizeof(T) * len;
			this->buffer = (T*)Mem::Alloc(Mem::MEM_TYPE_DARRAY, bytes);
			Mem::Copy(this->buffer, buffer, bytes);
		}

		DArray(const DArray& other) : len(other.len), maxLen(other.maxLen), incLen(other.incLen) {
			this->buffer = (T*)Mem::Alloc(Mem::MEM_TYPE_DARRAY, sizeof(T) * this->maxLen);
			Mem::Copy(this->buffer, other.buffer, sizeof(T) * other.len);
		}

		~DArray() {
			Mem::Free(Mem::MEM_TYPE_DARRAY, this->buffer, sizeof(T) * this->maxLen);
		}

		const bool Contains(const T& elem) const { return *this > elem; }
		const u32 GetLen() const { return this->len; }

		T& operator[](i32& index) {
			if (index < 0)
			{
				CLF_ASSERT(-index >= this->len, "Index out of bounds");
				return this->buffer[this->len + index];
			}

			CLF_ASSERT(index < this->len, "Index out of bounds");
			return this->buffer[index];
		}

		const u32& GetMaxLen() const { return this->maxLen; }

		const void SetMaxLen(const u32& maxLen) {
			//TODO:
			//if (maxLen < this->maxLen)
			//	Mem::Free(Mem::MEM_TYPE_DARRAY, this->buffer + this->maxLen, sizeof(T) * (maxLen - this->maxLen));

			T* newBuffer = Mem::Alloc(Mem::MEM_TYPE_DARRAY, sizeof(T) * maxLen);
			Mem::Copy(newBuffer, this->buffer, sizeof(T) * this->len);
		}

		const DArray& operator+=(const T& elem) {
			if (this->len == this->maxLen)
			{
				this->maxLen += this->incLen;
				T* newBuffer = (T*)Mem::Alloc(Mem::MEM_TYPE_DARRAY, this->maxLen * sizeof(T));
			}

			Mem::Copy(this->buffer + this->len, &elem, sizeof(T));
			this->len++;
			return *this;
		}

		const bool operator-=(const T& elem) {
			for (u32 i = 0; i < this->len; i++)
				if (elem == this->buffer[i])
				{
					this->buffer[i].~T();
					this->len--;

					//TODO:
					//constexpr u32 DISP_FCTR = 3;
					//gapLens = this->maxLen - this->len;
					//if (gapLen < this->incLen * DISP_FCTR)
					//	Mem::Free(Mem::MEM_TYPE_DARRAY, this->buffer + this->len, sizeof(T) * gapLen);

					return true;
				}
			return false;
		}

		const bool operator>(const T& elem)
		{
			for (u32 i = 0; i < this->len; i++)
				if (this->buffer[i] == elem)
					return true;
			return false;
		}
	};
}