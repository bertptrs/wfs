extern crate fuse;
extern crate libc;
extern crate slice_cast;

use backend::Backend;
use self::libc::c_int;

#[derive(Default)]
pub struct FileSystem<T: Backend> {
    backend: T,
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

        if magic_read == MAGIC_CONST {
            Ok(())
        } else {
            // Panic because this method cannot return error states.
            panic!("Invalid magic number");
        }
    }

    pub fn from_backend(backend: T) -> FileSystem<T> {
        FileSystem{
            backend: backend
        }
    }

}

impl <T: Backend> fuse::Filesystem for FileSystem<T> {

    fn init(&mut self, _req: &fuse::Request) -> Result<(), c_int> {
        self.magic_check()
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
