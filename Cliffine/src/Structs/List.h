#pragma once
#include "Defines.h"
#include "Core/Mem.h"
#include "Core/Asserts.h"

namespace Clf
{
	template<typename T>
	struct List
	{
	private:
		struct Node
		{
			T elem;
			Node* next = nullptr;

			~Node()
			{
				Mem::Free(Mem::MEM_TYPE_LIST, this, sizeof(Node));
			}
		};

		Node* head = nullptr;
		Node* tail = nullptr;
		u32 len = 0;

	public:
		const u32 GetLen() { return len; }

		T& operator[](i32 index) {
			if (index < 0)
			{
				CLF_ASSERT(-index <= len, "Index out of bounds");
				return this->operator[](len + index);
			}

			CLF_ASSERT(index < len, "Index out of bounds");
			Node* node = head;
			for (u32 i = 1; i < index; i++)
				node = node->next;
			return node->elem;
		}

		void operator+=(const T& elem) {
			Node* tail = (Node*)Mem::Alloc(Mem::MEM_TYPE_LIST, sizeof(Node));
			tail->elem = elem;
			if (!this->head)
				this->head = tail;
			if (this->tail)
				this->tail->next = tail;
			this->tail = tail;
			len++;
		}

		const T* operator-=(T elem) {
			Node*& node = this->head;
			for (u32 i = 0; i < len; i++)
				if (node->elem == elem)
				{
					Node* oldNode = node;
					node = node->next;

					Mem::Free(Mem::MEM_TYPE_LIST, oldNode, sizeof(Node));
					this->len--;
					return &node->elem;
				}
			return nullptr;
		}

		const bool operator>(const T& elem)
		{
			Node* node = head;
			for (u32 i = 1; i < len; i++)
			{
				if (node->elem == elem)
					return true;
				node = node->next;
			}
			return false;
		}

		friend const bool operator<(const T& elem, const List& list) { return *list > elem; }
	};
}