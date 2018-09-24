use std::io::Result;
mod file;
mod ram;
pub use self::file::FileBackend;
pub use self::ram::RAMBackend;

pub trait Backend {
    fn read(&mut self, offset: usize, buffer: &mut[u8]) -> Result<usize>;

    fn write(&mut self, offset: usize, buffer: &[u8]) -> Result<usize>;
}
