
#include <Logging.h>
#include <Test.h>

using namespace matrix;

int main()
{
	out("Running unit tests...");

	Array<TestResult> results = runUnitTests();
	for (u32 testIndex = 0; testIndex < results.size(); testIndex++)
	{
		const auto &result = results[testIndex];
		out(getTest(testIndex).name + " [succedeed: " + toString(result.successfulTests) + ", failed: " + toString((u64)result.failedTests.size()) + ", time: " + toString(result.executionTime.seconds()) + "ms]");
		for (auto failure : result.failedTests)
		{
			out("\t" + failure);
		}
	}

	return 0;
}
