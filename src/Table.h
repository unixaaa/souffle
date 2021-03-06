/*
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All Rights reserved
 * 
 * The Universal Permissive License (UPL), Version 1.0
 * 
 * Subject to the condition set forth below, permission is hereby granted to any person obtaining a copy of this software,
 * associated documentation and/or data (collectively the "Software"), free of charge and under any and all copyright rights in the 
 * Software, and any and all patent rights owned or freely licensable by each licensor hereunder covering either (i) the unmodified 
 * Software as contributed to or provided by such licensor, or (ii) the Larger Works (as defined below), to deal in both
 * 
 * (a) the Software, and
 * (b) any piece of software and/or hardware listed in the lrgrwrks.txt file if one is included with the Software (each a “Larger
 * Work” to which the Software is contributed by such licensors),
 * 
 * without restriction, including without limitation the rights to copy, create derivative works of, display, perform, and 
 * distribute the Software and make, use, sell, offer for sale, import, export, have made, and have sold the Software and the 
 * Larger Work(s), and to sublicense the foregoing rights on either these or other terms.
 * 
 * This license is subject to the following condition:
 * The above copyright notice and either this complete permission notice or at a minimum a reference to the UPL must be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/************************************************************************
 *
 * @file Table.h
 *
 * An implementation of a generic Table storing a position-fixed collection
 * of objects in main memory.
 *
 ***********************************************************************/

#pragma once

#include <iterator>

template<typename T, unsigned blockSize = 4096>
class Table {

	struct Block {
		Block* next;
		std::size_t used;
		T data[blockSize];

		Block() : next(nullptr), used(0) {}

		bool isFull() const {
			return used == blockSize;
		}

		const T& append(const T& element) {
			const T& res = data[used];
			data[used] = element;
			used++;
			return res;
		}
	};

	Block* head;
	Block* tail;

	std::size_t count;

public:

	class iterator : public std::iterator<std::forward_iterator_tag,T> {
		Block* block;
		unsigned pos;

	public:

		iterator(Block* block = nullptr, unsigned pos = 0) : block(block), pos(pos) {}

		iterator(const iterator&) = default;
		iterator(iterator&&) = default;
		iterator& operator=(const iterator&) = default;

		// the equality operator as required by the iterator concept
		bool operator==(const iterator& other) const {
			return (block==nullptr && other.block==nullptr) || (block==other.block && pos == other.pos);
		}

		// the not-equality operator as required by the iterator concept
		bool operator!=(const iterator& other) const {
			return !(*this == other);
		}

		// the deref operator as required by the iterator concept
		const T& operator*() const {
			return block->data[pos];
		}

		// the increment operator as required by the iterator concept
		iterator& operator++() {
			// move on in block
			if (++pos < block->used) {
				return *this;
			}
			// or to next block
			block = block->next;
			pos = 0;
			return *this;
		}

	};


	Table() : head(nullptr), tail(nullptr), count(0) {}

	~Table() {
		clear();
	}

	bool empty() const {
		return (!head);
	}

	std::size_t size() const {
		return count;
	}

	const T& insert(const T& element) {

		// check whether the head is initialized
		if (!head) {
			head = new Block();
			tail = head;
		}

		// check whether tail is full
		if (tail->isFull()) {
			tail->next = new Block();
			tail = tail->next;
		}

		// increment counter
		count++;

		// add another element
		return tail->append(element);
	}

	iterator begin() const {
		return iterator(head);
	}

	iterator end() const {
		return iterator();
	}

	void clear() {
		while(head != nullptr) {
			auto cur = head;
			head = head->next;
			delete cur;
		}
		count = 0;
		head = nullptr;
		tail = nullptr;
	}

};
