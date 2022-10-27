#include "String.h"
#include "Core/Mem.h"
#include "Core/Asserts.h"

namespace Clf
{
	const u32& String::GetLen(const char* buffer) {
		u32 i = 0;
		while (buffer[i] != '\0')
			i++;
		return i;
	}

	String::String(const char* buffer) {
		this->len = GetLen(buffer);
		this->buffer = (char*)Mem::Alloc(Mem::MEM_TYPE_STRING, this->len + 1);
		Mem::Copy(this->buffer, buffer, this->len + 1);
	}

	String::String(const String& other) {
		if (this->buffer == other.buffer)
			return;

		this->len = other.len;
		this->buffer = (char*)Mem::Alloc(Mem::MEM_TYPE_STRING, this->len + 1);
		Mem::Copy(this->buffer, other.buffer, this->len + 1);
	}

	String::~String() {
		Mem::Free(Mem::MEM_TYPE_STRING, this->buffer, this->len + 1);
	}

	const String String::Sub(const u32& start, const u32& end) const {
		u32 newLen = end - start;
		char* newBuffer = (char*)Mem::Alloc(Mem::MEM_TYPE_STRING, newLen + 1);
		Mem::Copy(newBuffer, this->buffer + start, newLen + 1);

		return String{ newBuffer, newLen };
	}

	char& String::operator[](const i32& index)
	{
		if (index < 0)
		{
			CLF_ASSERT(-index >= this->len, "Index out of bounds");
			return this->buffer[this->len + index];
		}

		CLF_ASSERT(index < this->len, "Index out of bounds");
		return this->buffer[index];
	}

	void String::operator=(const String& other)
	{
		if (this->buffer == other.buffer)
			return;
		this->~String();

		this->len = other.len;
		this->buffer = (char*)Mem::Alloc(Mem::MEM_TYPE_STRING, this->len + 1);
		Mem::Copy(this->buffer, other.buffer, this->len + 1);
	}

	const String String::operator+(const String& other) const {
		u32 newLen = this->len + other.len;
		char* newBuffer = (char*)Mem::Alloc(Mem::MEM_TYPE_STRING, newLen + 1);

		Mem::Copy(newBuffer, this->buffer, this->len);
		Mem::Copy(newBuffer + this->len, other.buffer, other.len + 1);

		return String{ newBuffer, newLen };
	}

	void String::operator+=(const String& other) {
		u32 newLen = this->len + other.len;
		char* newBuffer = (char*)Mem::Alloc(Mem::MEM_TYPE_STRING, newLen + 1);

		Mem::Copy(newBuffer, other.buffer, other.len);
		Mem::Copy(newBuffer + this->len, other.buffer, other.len + 1);

		this->~String();
		this->buffer = newBuffer;
		this->len = newLen;
	}

	const String String::operator*(const u32& n) const
	{
		u32 newLen = this->len * n;
		char* newBuffer = (char*)Mem::Alloc(Mem::MEM_TYPE_STRING, newLen + 1);

		for (u32 i = 0; i < newLen; i += this->len)
			Mem::Copy(newBuffer + i, this->buffer, this->len);

		return String{ newBuffer, newLen };
	}

	void String::operator*=(const u32& n) {
		u32 newLen = this->len * n;
		char* newBuffer = (char*)Mem::Alloc(Mem::MEM_TYPE_STRING, newLen + 1);

		for (u32 i = 0; i < newLen; i += this->len)
			Mem::Copy(newBuffer + i, this->buffer, this->len);

		this->~String();
		this->buffer = newBuffer;
		this->len = newLen;
	}

	const bool String::operator==(const String& other) const {
		if (this->len != other.len)
			return false;

		for (u32 i = 0; i < this->len; i++)
			if (this->buffer[i] != other.buffer[i])
				return false;

		return true;
	}

	const bool String::operator<=(const String& other) const {
		if (this->len > other.len)
			return false;

		for (u32 i = 0; i <= other.len - this->len; i++) {
			u32 j;
			for (j = 0; j < this->len; j++)
				if (this->buffer[j] != other.buffer[i + j])
					break;

			if (j == this->len)
				return true;
		}

		return false;
	}
}