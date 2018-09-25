extern crate fuse;
extern crate libc;
extern crate slice_cast;
extern crate time;

use backend::Backend;
use self::libc::c_int;
use wfs::entry::FileEntry;

/// Number of total blocks in FS
const TOTAL_BLOCKS: usize = 16834;
/// Block indicating end of data
const BLOCK_FINAL: u16 = 0xfffe;
/// Block indicating no such block
const BLOCK_EMPTY: u16 = 0x0000;
/// Block size, in bytes
const BLOCK_SIZE: usize = 512;
/// Root dir offset in medium
const ROOT_DIR_OFFSET: usize = 16;
const ROOT_DIR_SIZE: usize = 4096;
/// Block table offset in medium
const BLOCK_TABLE_OFFSET: usize = ROOT_DIR_OFFSET + ROOT_DIR_SIZE;
const BLOCK_DATA_OFFSET: usize = BLOCK_TABLE_OFFSET + TOTAL_BLOCKS * 2;

pub struct FileSystem<T: Backend> {
    backend: T,
    fat: [u16; TOTAL_BLOCKS],
}

const MAGIC_CONST: [u32; 4] = [
    0x00c0ffeeu32,
    0x00000000,
    0xf00d1350,
    0x0000beef,
];

const FILE_TIMESPEC: time::Timespec = time::Timespec{sec: 0, nsec: 0};
const TTL_TIMESEC: time::Timespec = time::Timespec{sec: 60, nsec: 0};


impl <T: Backend> FileSystem<T> {

    /**
     * Check the magic numbers in the file system.
     *
     * Will panic on error.
     */
    fn magic_check(&mut self) -> Result<(), c_int> {
        let mut rbuf = [0u8; 16];
        if let Err(_) = self.backend.read(0, &mut rbuf) {
            panic!("Could not read magic number")
        }

        let magic_read: &[u32] = unsafe { slice_cast::cast(&rbuf) };

        if magic_read != MAGIC_CONST {
            // Panic because this method cannot return error states.
            panic!("Invalid magic number");
        }
        Ok(())
    }

    /**
     * Load the File Allocation Table into memory
     */
    fn load_fat(&mut self) -> Result<(), c_int> {
        unsafe {
            let block_buf: &mut[u8] = slice_cast::cast_mut(&mut self.fat);
            if let Err(_) = self.backend.read(BLOCK_TABLE_OFFSET, block_buf) {
                panic!("Unable to read block table");
            }
        }
        Ok(())
    }

    fn read_dir<'a>(&self, data: &'a[u8]) -> &'a[FileEntry] {
        unsafe { slice_cast::cast(&data) }
    }

    fn chain_length(&self, ino: u64) -> usize {
        let mut i: usize = 0;
        let mut cur = ino as usize;
        while self.fat[cur - 2] != BLOCK_FINAL {
            cur = self.fat[cur - 2] as usize;
            i += 1;
        }

        return i;
    }

    fn produce_dir_attr(&self, ino: u64) -> fuse::FileAttr {
        fuse::FileAttr{
            ino: ino,
            atime: FILE_TIMESPEC,
            ctime: FILE_TIMESPEC,
            crtime: FILE_TIMESPEC,
            mtime: FILE_TIMESPEC,
            size: match ino {
                1 => 4096,
                _ => self.chain_length(ino) as u64,
            },
            blocks: 0,
            perm: 0o777,
            kind: fuse::FileType::Directory,
            nlink: 1,
            uid: 0,
            gid: 0,
            rdev: 0,
            flags: 0,
        }
    }

    /**
     * Construct a new file system, given the specified back-end.
     */
    pub fn from_backend(backend: T) -> FileSystem<T> {
        FileSystem{
            backend: backend,
            fat: [0u16; TOTAL_BLOCKS]
        }
    }

}

impl <T: Backend> fuse::Filesystem for FileSystem<T> {

    fn init(&mut self, _req: &fuse::Request) -> Result<(), c_int> {
        self.magic_check()?;
        self.load_fat()
    }

    fn getattr(&mut self, _req: &fuse::Request, ino: u64, reply: fuse::ReplyAttr) {
        match ino {
            1 => {
                reply.attr(&TTL_TIMESEC, &self.produce_dir_attr(ino));
            }
            _ => reply.error(libc::ENOENT),
        };
    }

}

#[cfg(test)]
mod tests {
    use super::*;
    use backend::RAMBackend;

    #[test]
    fn test_magic_number() {
        let mut backend: RAMBackend = Default::default();
        let magic_string: &[u8] = unsafe { slice_cast::cast(&MAGIC_CONST) };
        let _ = backend.write(0, &magic_string);
        let mut instance = FileSystem::from_backend(backend);
        match instance.magic_check() {
            Ok(()) => {},
            Err(_) => assert!(false)
        };
    }

    #[test]
    #[should_panic]
    fn test_magic_number_failure() {
        let backend: RAMBackend = Default::default();
        let mut instance = FileSystem::from_backend(backend);
        let _ = instance.magic_check();
    }
}
