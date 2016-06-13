#ifndef IODEVICETEST_HPP
#define IODEVICETEST_HPP

#include "../IODevice.hpp"
#include <cstring>

using namespace wfs;
using namespace std;

class IODeviceTest {

	protected:
		void testIODevice(IODevice& dev)
		{
			const char data[] = "Hello world!";
			const size_t dataSize = sizeof(data) - 1;
			char buffer[dataSize];

			dev.write(0x2, dataSize, data);

			dev.read(0x0, dataSize, buffer);
			TS_ASSERT_SAME_DATA("\0\0Hello worl", buffer, dataSize);

			dev.read(0x2, dataSize, buffer);
			TS_ASSERT_SAME_DATA("Hello world!", buffer, dataSize);

			dev.read(0x4, dataSize, buffer);
			TS_ASSERT_SAME_DATA("llo world!\0\0", buffer, dataSize);
		}

};

#endif
