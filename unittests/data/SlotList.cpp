
#include <data/SlotList.h>
#include <Test.h>

using namespace matrix;
using namespace data;

#define ELEMENT_COUNT 1024

static TestSubResult runDataSlotListAddTest()
{
	SlotList<u32> list;

	for (u32 i = 0; i < ELEMENT_COUNT; i++)
	{
		list.add(i);
	}

	MATRIX_TEST_EQUAL(list.size() == ELEMENT_COUNT);

	for (u32 i = 0; i < ELEMENT_COUNT; i++)
	{
		MATRIX_TEST_EQUAL(list[i] == i);
	}

	MATRIX_TEST_SUCCESS();
}

static TestSubResult runDataSlotListMultiAddTest()
{
	Array<u32> input(ELEMENT_COUNT);
	Array<u32> indices(ELEMENT_COUNT);

	for (u32 i = 0; i < ELEMENT_COUNT; i++)
	{
		input[i] = i;
	}

	SlotList<u32> list;
	list.add(input.data(), input.size(), indices.data());

	MATRIX_TEST_EQUAL(list.size() == ELEMENT_COUNT);

	for (u32 i = 0; i < ELEMENT_COUNT; i++)
	{
		MATRIX_TEST_EQUAL(list[i] == i);
		MATRIX_TEST_EQUAL(indices[i] == i);
	}

	MATRIX_TEST_SUCCESS();
}

static TestSubResult runDataSlotListForEachTest()
{
	SlotList<u32> list;

	for (u32 i = 0; i < ELEMENT_COUNT; i++)
	{
		list.add(i);
	}

	MATRIX_TEST_EQUAL(list.size() == ELEMENT_COUNT);

	bool success = true;
	u32 i = 0;
	list.forEach(
		[&](const u32 &element)
		{
			if (element != i)
			{
				success = false;
			}
			i++;
		});
	MATRIX_TEST_EQUAL(success);

	MATRIX_TEST_EQUAL(i == ELEMENT_COUNT);

	MATRIX_TEST_SUCCESS();
}

static TestSubResult runDataSlotListForEachInlineTest()
{
	SlotList<u32> list;

	for (u32 i = 0; i < ELEMENT_COUNT; i++)
	{
		list.add(i);
	}

	MATRIX_TEST_EQUAL(list.size() == ELEMENT_COUNT);

	u32 i = 0;
	MATRIX_DATA_SLOTLIST_FOR_EACH(
		list,
		{
			MATRIX_TEST_EQUAL(element == i);
			i++;
		},
		element);

	MATRIX_TEST_EQUAL(i == ELEMENT_COUNT);

	MATRIX_TEST_SUCCESS();
}

MATRIX_TEST_SET(
	"data/SlotList",
	{
		MATRIX_TEST("SlotList::add(element)", runDataSlotListAddTest());
		MATRIX_TEST("SlotList::add(element, count, indices)", runDataSlotListMultiAddTest());
		MATRIX_TEST("SlotList::forEach(body)", runDataSlotListForEachTest());
		MATRIX_TEST("MATRIX_DATA_SLOTLIST_FOR_EACH", runDataSlotListForEachInlineTest());
	})
