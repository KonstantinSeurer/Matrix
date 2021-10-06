#ifndef INCLUDE_DATA_SLOTLIST
#define INCLUDE_DATA_SLOTLIST

#include <Base.h>
#include <math/Base.h>
#include <Logging.h>

namespace matrix
{

	namespace data
	{

		template <typename T>
		class SlotList
		{
		private:
			T *data;
			u64 *state;
			u32 length = 0;
			u32 capacity;

			void grow(u32 minimumGrowth)
			{
				const u32 minimumBucketGrowth = (minimumGrowth >> 6) + 1; // Greater or equal to 1
				const u32 newCapacity = capacity + math::max(minimumBucketGrowth, capacity >> 1);

				T *newData = new T[newCapacity << 6];
				u64 *newState = new u64[newCapacity];

				memcpy(newData, data, sizeof(T) * (capacity << 6));
				memcpy(newState, state, sizeof(u64) * capacity);
				memset(&(newState[capacity]), 0x0, (newCapacity - capacity) * sizeof(u64));

				delete data;
				delete state;

				data = newData;
				state = newState;
				capacity = newCapacity;
			}

		public:
			SlotList(u32 initialCapacity) : capacity(initialCapacity)
			{
				state = new u64[initialCapacity];
				memset(state, 0x0, initialCapacity * sizeof(u64));

				data = new T[initialCapacity << 6];
			}

			SlotList() : SlotList(1)
			{
			}

			~SlotList()
			{
				delete data;
				delete state;
			}

			u32 add(const T &element)
			{
				if (length == capacity << 6)
				{
					grow(1);
				}

				length++;

				for (u32 bucketIndex = 0; bucketIndex < capacity; bucketIndex++)
				{
					const u64 bucketState = state[bucketIndex];

					if (bucketState == ~(u64)0)
					{
						continue;
					}

					for (u64 bitIndex = 0; bitIndex < 64; bitIndex++)
					{
						if ((bucketState & ((u64)1 << bitIndex)) == (u64)0)
						{
							const u32 index = (bucketIndex << 6) + bitIndex;

							data[index] = element;
							state[bucketIndex] |= ((u64)1 << bitIndex);

							return index;
						}
					}
				}

				return 0xFFFFFFFF; // unreachable
			}

			void add(const T *elements, u32 count, u32 *indices)
			{
				if (length + count - 1 >= capacity << 6)
				{
					grow(length + count - capacity);
				}

				length += count;

				u32 sourceIndex = 0;

				for (u32 bucketIndex = 0; bucketIndex < capacity; bucketIndex++)
				{
					const u64 bucketState = state[bucketIndex];

					if (bucketState == ~(u64)0)
					{
						continue;
					}

					for (u64 bitIndex = 0; bitIndex < 64; bitIndex++)
					{
						if ((bucketState & ((u64)1 << bitIndex)) == (u64)0)
						{
							const u32 index = (bucketIndex << 6) + bitIndex;

							indices[sourceIndex] = index;
							data[index] = elements[sourceIndex];
							state[bucketIndex] |= ((u64)1 << bitIndex);

							sourceIndex++;

							if (sourceIndex == count)
							{
								return;
							}
						}
					}
				}
			}

			void remove(u32 index)
			{
				data[index]->~T();
				state[index >> 6] &= ~((u64)1 << ((u64)index & (u64)0b111111));
			}

			const T &operator[](u32 index) const
			{
				return data[index];
			}

			T &operator[](u32 index)
			{
				state[index >> 6] |= ((u64)1 << ((u64)index & (u64)0b111111));
				return data[index];
			}

			u32 size() const
			{
				return length;
			}

			bool empty() const
			{
				return length == 0;
			}

			bool exists(u32 index) const
			{
				return (state[index >> 6] & ((u64)1 << ((u64)index & (u64)0b111111))) != (u64)0;
			}

			u64 bucketState(u32 bucketIndex) const
			{
				return state[bucketIndex];
			}

			u32 bucketCount() const
			{
				return capacity;
			}

#define MATRIX_DATA_SLOTLIST_FOR_EACH(list, body, elementName)                                          \
	for (u32 list##bucketIndex = 0; list##bucketIndex < list /**/.bucketCount(); list##bucketIndex++)   \
	{                                                                                                   \
		const u64 list##bucketState = list /**/.bucketState(list##bucketIndex);                         \
		if (list##bucketState == 0)                                                                     \
		{                                                                                               \
			continue;                                                                                   \
		}                                                                                               \
		if (list##bucketState == ~0)                                                                    \
		{                                                                                               \
			const u32 list##endIndex = (list##bucketIndex + 1) << 6;                                    \
			for (u32 list##index = list##bucketIndex << 6; list##index < list##endIndex; list##index++) \
			{                                                                                           \
				auto &elementName = list[list##index];                                                  \
				body                                                                                    \
			}                                                                                           \
			continue;                                                                                   \
		}                                                                                               \
		const u32 list##baseIndex = list##bucketIndex << 6;                                             \
		for (u64 list##index = 0; list##index < 64; list##index++)                                      \
		{                                                                                               \
			if ((list##bucketState & ((u64)1 << list##index)) == (u64)0)                                \
			{                                                                                           \
				continue;                                                                               \
			}                                                                                           \
			auto &elementName = list[list##index + list##baseIndex];                                    \
			body                                                                                        \
		}                                                                                               \
	}

			void forEach(const Function<void(const T &)> &function) const
			{
				const SlotList<T> &list = *this;
				MATRIX_DATA_SLOTLIST_FOR_EACH(
					list, { function(element); }, element)
			}

			void forEach(const Function<void(T &)> &function)
			{
				SlotList<T> &list = *this;
				MATRIX_DATA_SLOTLIST_FOR_EACH(
					list, { function(element); }, element)
			}
		};

	}
}

#endif /* INCLUDE_DATA_SLOTLIST */
