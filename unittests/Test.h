#ifndef UNITTESTS_TEST
#define UNITTESTS_TEST

#include <Base.h>
#include <Time.h>

namespace matrix
{

	using TestSubResult = Optional<String>;

	struct TestResult
	{
		u32 successfulTests = 0;
		Array<String> failedTests;
		Time executionTime;
	};

	struct TestData
	{
		Function<TestResult()> run;
		String name;
	};

	void
	registerUnitTest(const TestData &test);

	const u32 getTestCount();
	const TestData &getTest(u32 index);

	Array<TestResult> runUnitTests();

}

#define MATRIX_TEST_SUCCESS() return TestSubResult()
#define MATRIX_TEST_EQUAL(condition)      \
	if (!(condition))                     \
	{                                     \
		return TestSubResult(#condition); \
	}

#define MATRIX_TEST(name, runTest)                                                                \
	testSubResult = runTest;                                                                      \
	if (testSubResult)                                                                            \
	{                                                                                             \
		testSetResult.failedTests.push_back(String(name) + String(": ") + testSubResult.value()); \
	}                                                                                             \
	else                                                                                          \
	{                                                                                             \
		testSetResult.successfulTests++;                                                          \
	}

#define MATRIX_TEST_SET(testName, body)                                         \
	static_block                                                                \
	{                                                                           \
		matrix::TestData test;                                                  \
		test.name = testName;                                                   \
		test.run = []() {                                                       \
			TestResult testSetResult;                                           \
			TestSubResult testSubResult;                                        \
			body;                                                               \
			testSetResult.executionTime = Time() - testSetResult.executionTime; \
			return testSetResult;                                               \
		};                                                                      \
		matrix::registerUnitTest(test);                                         \
	};

#endif /* UNITTESTS_TEST */
