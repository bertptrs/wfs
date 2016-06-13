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

			// Check if the system properly supports an entire WFS filesystem.
			constexpr char entry = '*';
			constexpr size_t maxPos = 16 // Magic numbers
				+ 64 * 64 // root table entries
				+ 0x4000 * 2 // Block table
				+ 0x4000 * 512 // Blocks
				-1 ; // Zero indexing

			dev.write(maxPos, entry);
			char readBack;
			dev.read(maxPos, readBack);


			TS_ASSERT_EQUALS(entry, readBack);
		}

};

#endif
