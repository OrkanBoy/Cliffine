#pragma once
#include "Defines.h"

namespace Clf
{
	//Considering Typedeffing String as DArray<char>
	struct String
	{
	public:
		static const u32& GetLen(const char* buffer);

		//implement iterator
	private:
		char* buffer = nullptr;
		u32 len = 0;

		//TODO: implement batched memory allocation
		//u32 maxLen = 0;
		//u32 incLen = 0;
		String(char* buffer, const u32& len) : buffer(buffer), len(len) {}
	public:
		String(const char* buffer);
		String(const String& other);
		String(String&& other) = default;
		//TODO: implement to string conversion for built-in types
		~String();

		const u32& GetLen() const { return len; }
		const char* GetRaw() const { return buffer; }
		operator const char* () { return buffer; }

		const String Sub(const u32& start, const u32& end) const;
		const bool IsSub(const String& other) { *this <= other; }

		char& operator[](const i32& index);

		void operator=(const String& other);

		const String operator+(const String& rhs) const;
		void operator+=(const String& other);

		const String operator*(const u32& n) const;
		void operator*=(const u32& n);

		//TODO: implement split(/) operator 
		//const DArray<const String> operator/(const String& other) const;

		const bool operator==(const String& other) const;
		const bool operator!=(const String& other) const { return !(*this == other); }

		const bool operator<=(const String& other) const;
		const bool operator>=(const String& other) const { return other <= *this; }
	};

	static const String operator""_s(const char* buffer) { return String(buffer); }
}