extern crate fuse;
extern crate libc;
extern crate slice_cast;

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


impl <T: Backend> FileSystem<T> {

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

    fn load_fat(&mut self) -> Result<(), c_int> {
        let block_buf: &mut[u8] = unsafe { slice_cast::cast_mut(&mut self.fat) };
        if let Err(_) = self.backend.read(BLOCK_TABLE_OFFSET, block_buf) {
            panic!("Unable to read block table");
        }
        Ok(())
    }

    fn read_dir<'a>(&self, data: &'a[u8]) -> &'a[FileEntry] {
        unsafe { slice_cast::cast(&data) }
    }

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
