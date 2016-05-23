#include <memory>
#include "MemIODevice.hpp"
#include "FileSystem.hpp"

using namespace std;
using namespace wfs;

int main()
{
	shared_ptr<IODevice> dev = make_shared<MemIODevice>();
	FileSystem fs(dev);

	fs.init();

	return 0;
}
