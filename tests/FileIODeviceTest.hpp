#include <cxxtest/TestSuite.h>
#include "../FileIODevice.hpp"
#include "IODeviceTest.hpp"

#include <cstdint>
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>

using namespace wfs;
using namespace std;

class FileIODeviceTest : public CxxTest::TestSuite, public IODeviceTest
{
	private:
		string backingFilename;

	public:
		void setUp()
		{
			backingFilename = tmpnam(NULL);
			ofstream output(backingFilename);
			output << "";
		}

		void tearDown()
		{
			remove(backingFilename.c_str());
		}

		void testIO(void)
		{

			FileIODevice dev(backingFilename);
			testIODevice(dev);
		}

};
