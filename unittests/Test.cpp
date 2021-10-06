
#include <Test.h>

namespace matrix
{

	static Array<TestData> tests;

	void registerUnitTest(const TestData &test)
	{
		tests.push_back(test);
	}

	const u32 getTestCount()
	{
		return tests.size();
	}

	const TestData &getTest(u32 index)
	{
		return tests[index];
	}

	Array<TestResult> runUnitTests()
	{
		Array<TestResult> results(tests.size());
		for (u32 testIndex = 0; testIndex < tests.size(); testIndex++)
		{
			results[testIndex] = tests[testIndex].run();
		}
		return results;
	}

}
