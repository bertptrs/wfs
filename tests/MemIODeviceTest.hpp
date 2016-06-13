#include <cxxtest/TestSuite.h>
#include "../MemIODevice.hpp"
#include "IODeviceTest.hpp"

#include <cstdint>

using namespace wfs;
using namespace std;

class MemIODeviceTest : public CxxTest::TestSuite, public IODeviceTest
{
	public:
		void testIO(void)
		{
			MemIODevice dev;
			testIODevice(dev);
		}
};
