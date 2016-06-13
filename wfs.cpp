#include <memory>
#include <cstring>
#include <cassert>

#include <unistd.h>
#include <sys/types.h>

#include "FileSystem.hpp"
#include "FileIODevice.hpp"
#include "Exceptions.hpp"

#define FUSE_USE_VERSION 26
#include <fuse.h>

using namespace std;
using namespace wfs;

struct WFSData {
	FileSystem* fs;

	WFSData(FileSystem* fs) :
		fs(fs)
	{
	}

	~WFSData()
	{
		delete fs;
	}
};

static inline WFSData* getWFSData()
{
	return (WFSData*) fuse_get_context()->private_data;
}

void wfs_destroy(void* data)
{
	delete (WFSData*) data;
}

int wfs_opendir(const char*, struct fuse_file_info*)
{
	// No checking for now
	return 0;
}

int wfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t, struct fuse_file_info *)
{
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	// TODO: Not implemented
	assert(!strcmp(path, "/"));
	return 0;
}

int wfs_getattr(const char *path, struct stat *stbuf)
{
	memset(stbuf, 0, sizeof(struct stat));

	stbuf->st_uid = geteuid();
	stbuf->st_gid = getegid();
	if (!strcmp(path, "/")) {
		// File is actually root.
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else {
		try {
			FileEntry file = getWFSData()->fs->getFile(path);
			stbuf->st_size = file.getSize();
			stbuf->st_nlink = 1;
			stbuf->st_mode = 0755;
			if (file.isDir()) {
				stbuf->st_mode |= S_IFDIR;
			}
		} catch (NoSuchFileException) {
			return -ENOENT;
		}
	}

	return 0;
}



struct fuse_operations wfs_oper = {
};

int main(int argc, char* argv[])
{
	// Initialize all fuse operations
	wfs_oper.opendir = wfs_opendir;
	wfs_oper.readdir = wfs_readdir;
	wfs_oper.destroy = wfs_destroy;
	wfs_oper.getattr = wfs_getattr;

	FileSystem* fs = new FileSystem(make_shared<FileIODevice>("fs.img"));
	WFSData* fsData = new WFSData(fs);

	return fuse_main(argc, argv, &wfs_oper, fsData);
}