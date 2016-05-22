#include <memory>
#include "FileIODevice.hpp"
#include "FileSystem.hpp"

using namespace std;
using namespace wfs;

int main()
{
	shared_ptr<IODevice> dev = make_shared<FileIODevice>("testfs.bin");
	FileSystem fs(dev);

	fs.init();

	return 0;
}
