// MyTestSuite1.h
#include <cxxtest/TestSuite.h>

/**
 * Simple test case to see that cxxtest is configured correctly.
 *
 * If this fails, here be dragons.
 */
class TestSuiteTest : public CxxTest::TestSuite
{
	public:
		void testAddition(void)
		{
			TS_ASSERT(1 + 1 > 1);
			TS_ASSERT_EQUALS(1 + 1, 2);
		}
};
