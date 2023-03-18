//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/g_zone.h
//  header for the zone allocation daemon
//----------------------------------------------------------
#ifndef _Z_ZONE_
#define _Z_ZONE_

#pragma once

enum {
	TAG_FREE = 0,
	TAG_STATIC = 1,
	TAG_PURGELEVEL = 100,
	TAG_SCOPE = 101,
	TAG_CACHE = 102,
	
	NUMTAGS
};

#ifdef _NOMAD_DEBUG
#define LOG_CALL(func) LOG_DEBUG("%s called from %s:%s:%iu",func,_FILE__,__func__,__LINE__)
#else
#define LOG_CALL(func)
#endif

extern "C" void Z_Init();

extern "C" void Z_FreeTags(nomadubyte_t lowtag, nomadubyte_t hightag);
extern "C" void Z_CleanCache();
extern "C" void Z_CheckHeap();
extern "C" void Z_ChangeUser(void *user, void *ptr);
extern "C" void Z_ChangeTag(void *ptr, nomadubyte_t tag);
extern "C" void Z_ChangeTag2(void *ptr, nomadubyte_t tag, const char *file, nomaduint_t line);
extern "C" nomaduint_t Z_ZoneSize();
extern "C" void *Z_Malloc(nomaduint_t size, nomadubyte_t tag, void *user);
extern "C" void *Z_Realloc(void *user, nomaduint_t nsize);
extern "C" void *Z_Calloc(void *user, nomaduint_t nelem, nomaduint_t elemsize, nomadubyte_t tag);
extern "C" void Z_Free(void *ptr);

template<typename T>
class zone_ptr
{
private:
	T *ptr;
public:
	zone_ptr(T *_ptr) noexcept
		: ptr(_ptr) { }
	template<typename... Args>
	zone_ptr(T *_ptr, Args&&... args) noexcept
		: ptr(_ptr)
	{
		new (ptr) T(std::forward<Args>(args)...);
	}
	zone_ptr(const zone_ptr &) noexcept = default;
	zone_ptr(zone_ptr &&) noexcept = default;

	constexpr zone_ptr(std::nullptr_t) noexcept : zone_ptr(){}
	constexpr zone_ptr() noexcept = default;

	~zone_ptr()
	{
		ptr->~T();
		Z_Free(ptr);
	}

	inline zone_ptr& operator=(const zone_ptr &) noexcept = default;
	inline T* operator->(void) noexcept { return ptr; }
};

// broken - dont use
#if 0
template<typename T, typename... Args>
inline T* make_ptr<T>(Args&&... args)
{
	T* ptr = static_cast<T*>(Z_Malloc(sizeof(T), TAG_STATIC, NULL));
	new (ptr) T(std::forward<Args>(args)...);
	return ptr;
}

template <class T>
struct nomad_allocator
{
	nomad_allocator() noexcept { }

	typedef T value_type;
    template <class U>
    constexpr nomad_allocator(const nomad_allocator<U>&) noexcept { }

    [[nodiscard]] T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_array_new_length();
        if (auto p = static_cast<T*>(Z_Malloc(n * sizeof(T), TAG_STATIC, NULL))) {
            return p;
        }

        throw std::bad_alloc();
    }

	void deallocate(T* p, std::size_t n) noexcept {
	    Z_Free(p);
	}
};
#endif


template<typename T>
class linked_list
{
public:
	class linked_list_node
	{
	public:
		T val;
		linked_list_node *next;
		linked_list_node *prev;
	public:
		linked_list_node() = default;
		linked_list_node(const linked_list_node &) = delete;
		linked_list_node(linked_list_node &&) = default;
		~linked_list_node() = default;
		inline linked_list_node& operator++(int) noexcept
		{
			next->next->prev = this;
			next->prev = prev;
			next->next = this;
			prev->next = next;
			return *this;
		}
		inline linked_list_node& operator++() noexcept
		{
			next->next->prev = this;
			next->prev = prev;
			next->next = this;
			prev->next = next;
			return *this;
		}
	};
	
	typedef linked_list_node* node;
	typedef linked_list_node list_node;
	typedef linked_list_node* iterator;
	typedef const linked_list_node* const_iterator;
	typedef std::size_t size_type;
private:
	linked_list<T>::node ptr_list = NULL;
	std::size_t _size = 0;
	
	linked_list<T>::node alloc_node(void) {
		linked_list<T>::node ptr = (linked_list<T>::node)Z_Malloc(
			sizeof(linked_list<T>::list_node), TAG_STATIC, NULL);
		if (ptr == NULL)
			N_Error("linked_list::alloc_node: memory allocation failed");
		
		return ptr;
	}
	void dealloc_node(linked_list<T>::node ptr) noexcept {
		if (ptr == NULL)
			return;
		Z_Free(ptr);
	}
public:
	linked_list() noexcept = default;
	~linked_list() noexcept
	{
		if (ptr_list == NULL || begin() == NULL)
			return;

		for (linked_list<T>::iterator it = begin(); it != end(); it = it->next)
			dealloc_node(it);
	}
	linked_list(linked_list &&) = default;
	linked_list(const linked_list &) = default;
	
	// memory management
	inline void clear(void) noexcept
	{
		if (ptr_list == NULL)
			return;
		
		for (linked_list<T>::iterator it = begin(); it != end(); it = it->next)
			dealloc_node(it);
	}
	inline void free_node(linked_list<T>::node ptr) noexcept
	{
		if (ptr == NULL)
			return;
		
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		dealloc_node(ptr);
		--_size;
	}
	inline void pop_back() noexcept
	{
		if (ptr_list == NULL)
			return;
		
		linked_list<T>::node endptr = back_node();
		endptr->prev->next = NULL;
		dealloc_node(endptr);
		--_size;
	}
	inline void erase(linked_list<T>::node ptr) noexcept
	{
		if (ptr == NULL || ptr_list == NULL)
			return;
		
		if (ptr == front_node())
			pop_front();
		else if (ptr == back_node())
			pop_back();
		else
			free_node(ptr);
	}
	inline void pop_front() noexcept
	{
		linked_list<T>::node frontptr = front_node();
		if (frontptr->next != NULL) {
			frontptr->next->prev = NULL;
			frontptr = frontptr->next;
			ptr_list = frontptr;
		}
		dealloc_node(ptr_list);
		--_size;
	}
	inline linked_list<T>::node push_back(void) noexcept
	{
		linked_list<T>::node ptr;
		if (ptr_list == NULL) {
			ptr_list = alloc_node();
			ptr = ptr_list;
			ptr_list->prev = NULL;
		}
		else {
			ptr = alloc_node();
			linked_list<T>::node endptr = back_node();
			endptr->next = ptr;
			ptr->prev = endptr;
		}
		ptr->next = NULL;
		++_size;
		return ptr;
	}
	inline linked_list<T>::node emplace_back(void) noexcept
	{ return push_back(); }
	
	// random algos
	inline void swap(linked_list& list)
	{
		if (list.ptr_list == NULL || ptr_list == NULL)
			return;
	}
	inline void swap_nodes(linked_list<T>::iterator iter1, linked_list<T>::iterator iter2) noexcept
	{
		if (ptr_list == NULL || iter1 == NULL || iter2 == NULL)
			return;
		
		if (iter1 == front_node())
			iter1->next->prev = iter2;
		else if (iter1 == back_node())
			iter1->prev->next = iter2;
		else {
			iter1->prev->next = iter2;
			iter1->next->prev = iter2;
		}
		
		if (iter2 == front_node())
			iter2->next->prev = iter1;
		else if (iter2 == back_node())
			iter2->prev->next = iter1;
		else {
			iter2->prev->next = iter1;
			iter2->next->prev = iter1;
		}
	}
	linked_list<T>::node find_node(linked_list<T>::iterator begin_it, std::size_t n,
		const T &val) noexcept
	{
		if (ptr_list == NULL || begin_it == NULL || n == 0)
			return NULL;
		
		for (linked_list<T>::iterator it = begin_it; --n; it = it->next) {
			if (it->val == val) return it;
		}
		return ptr_list;
	}
	inline void erase_n(linked_list<T>::iterator begin_it, std::size_t n) noexcept
	{
		if (begin_it == NULL || ptr_list == NULL || n == 0)
			return;
		
		for (linked_list<T>::iterator it = begin_it; --n; it = it->next)
			erase(it);
	}
	inline void erase_range(linked_list<T>::iterator begin_it, linked_list<T>::iterator end_it) noexcept
	{
		if (begin_it == NULL || end_it == NULL || ptr_list == NULL)
			return;
		
		for (linked_list<T>::iterator it = begin_it; it != end_it; it = it->next)
			erase(it);
	}

	// iterators/access
	inline T& operator[](std::size_t index) noexcept
	{
		if (ptr_list == NULL || index > _size) {
			LOG_WARN("ptr_list is NULL or index > _size in linked_list::operator[]");
			return 0;
		}
		
		linked_list<T>::iterator it = begin();
		while (--index && it != end())
			it = it->next;
		
		return it->val;
	}
	inline std::size_t size(void) const noexcept
	{ return _size; }
	inline linked_list<T>::iterator begin(void) {
		if (ptr_list == NULL)
			LOG_WARN("ptr_list is NULL in linked_list::begin");
		
		return ptr_list;
	}
	inline linked_list<T>::iterator end(void) {
		if (ptr_list == NULL) {
			LOG_WARN("ptr_list is NULL in linked_list::end");
			return NULL;
		}
		
		linked_list<T>::iterator _endptr;
		for (_endptr = begin();; _endptr = _endptr->next) {
			if (_endptr == NULL) break;
		}
		return _endptr;
	}
	inline linked_list<T>::node back_node(void) {
		if (ptr_list == NULL) {
			LOG_WARN("ptr_list is NULL in linked_list::back_node");
			return NULL;
		}
		linked_list<T>::node _endptr;
		for (_endptr = begin();; _endptr = _endptr->next) {
			if (_endptr->next == NULL) break;
		}
		return _endptr;
	}
	inline linked_list<T>::node front_node(void) {
		if (ptr_list == NULL)
			LOG_WARN("ptr_list is NULL in linked_list::front_node");
		
		return ptr_list;
	}
	inline T& back(void) noexcept {
		if (ptr_list == NULL)
			LOG_WARN("ptr_list is NULL in linked_list::back");
		
		return back_node()->val;
	}
	inline T& front(void) noexcept {
		if (ptr_list == NULL)
			LOG_WARN("ptr_list is NULL in linked_list::front");

		return front_node()->val;
	}
};


#endif