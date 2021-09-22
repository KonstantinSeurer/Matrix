/*
 * Types.h
 *
 *  Created on: Jun 2, 2021
 *      Author: konstantin
 */

#ifndef BASE_H_
#define BASE_H_

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <stdlib.h>
#include <deque>
#include <any>
#include <optional>
#include <iostream>
#include "nlohmann/json.hpp"

#ifndef CONCATENATE
#define CONCATENATE(s1, s2)     s1##s2
#define EXPAND_THEN_CONCATENATE(s1, s2) CONCATENATE(s1, s2)
#endif /* CONCATENATE */

#ifndef UNIQUE_IDENTIFIER
#ifdef __COUNTER__
#define UNIQUE_IDENTIFIER(prefix) EXPAND_THEN_CONCATENATE(prefix, __COUNTER__)
#else
#define UNIQUE_IDENTIFIER(prefix) EXPAND_THEN_CONCATENATE(prefix, __LINE__)
#endif /* COUNTER */
#else
#endif /* UNIQUE_IDENTIFIER */

#define static_block STATIC_BLOCK_IMPL1(UNIQUE_IDENTIFIER(_static_block_))

#define STATIC_BLOCK_IMPL1(prefix) \
	STATIC_BLOCK_IMPL2(CONCATENATE(prefix,_fn),CONCATENATE(prefix,_var))

#define STATIC_BLOCK_IMPL2(function_name,var_name) \
static void function_name(); \
static int var_name __attribute((unused)) = (function_name(), 0) ; \
static void function_name()

namespace matrix
{

typedef char s8;
typedef unsigned char u8;

typedef short s16;
typedef unsigned short u16;

typedef int s32;
typedef unsigned int u32;

typedef long int s64;
typedef unsigned long long int u64;

typedef float f32;
typedef double f64;

#define null nullptr

template<typename T> using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args> inline Ref<T> allocate(Args &&... args)
{
	return std::make_shared<T>(args...);
}

template<typename Destination, typename Source> Ref<Destination> CastUp(const Ref<Source> &ref)
{
	return std::static_pointer_cast<Destination>(ref);
}

template<typename Destination, typename Source> Ref<Destination> CastDown(const Ref<Source> &ref)
{
	return std::dynamic_pointer_cast<Destination>(ref);
}

template<typename Destination, typename Source> Destination* CastDown(Source *ref)
{
	return dynamic_cast<Destination*>(ref);
}

using Any = std::any;

template<typename Destination> Destination* CastDown(Any ref)
{
	return std::any_cast<Destination*>(ref);
}

using String = std::string;

inline String toString(s8 i)
{
	return std::to_string(i);
}

inline String toString(u8 i)
{
	return std::to_string(i);
}

inline String toString(s16 i)
{
	return std::to_string(i);
}

inline String toString(u16 i)
{
	return std::to_string(i);
}

inline String toString(s32 i)
{
	return std::to_string(i);
}

inline String toString(u32 i)
{
	return std::to_string(i);
}

inline String toString(s64 i)
{
	return std::to_string(i);
}

inline String toString(u64 i)
{
	return std::to_string(i);
}

inline String toString(f32 f)
{
	return std::to_string(f);
}

inline String toString(f64 f)
{
	return std::to_string(f);
}

inline String toString(bool b)
{
	return b ? "true" : "false";
}

template<typename T> using Array = std::vector<T>;

#define MATRIX_HASH(Class) template<> struct Hash<Class>\
{\
	template<> struct Hash<Class>\
	{\
		std::size_t operator() (const Class& data)\
		{\
			std::size_t result = 0;\
			const u8* bytes = (u8*) &data;\
			const u32 indexMask = sizeof(std::size_t) - 1;\
			for (u32 i = 0; i < sizeof(Class); i++)\
			{\
				result ^= bytes[i] << ((i & indexMask) * 8);\
			}\
			return result;\
		}\
	};\
}

#define MATRIX_EQUAL(Class)

#define MATRIX_HASH_EQUALS(Class) MATRIX_HASH(Class) MATRIX_EQUAL(Class)

// template<typename K, typename V> using UnorderedMap = std::unordered_map<K, V, Hash<K>>;
#define UnorderedMap std::unordered_map

template<typename K, typename V> using OrderedMap = std::map<K, V>;

template<typename T> using UnorderedSet = std::unordered_set<T>;
template<typename T> using OrderedSet = std::set<T>;

template<typename T> using Deque = std::deque<T>;

template<typename Signature> using Function = std::function<Signature>;

template<typename A, typename B> using Pair = std::pair<A, B>;
template<typename T> using Optional = std::optional<T>;

using JSON = nlohmann::json;

inline void* memAlloc(u64 size)
{
	return malloc(size);
}

inline void memFree(void *address)
{
	free(address);
}

#define MATRIX_ASSERT(expression) if (!(expression)) { std::cout << "Assert failed: " << #expression << std::endl; exit(1); }

}

#endif /* BASE_H_ */
