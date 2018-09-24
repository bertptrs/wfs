use backend::Backend;
use std::io::Result;
use std::io::SeekFrom;
use std::io::prelude::*;
use std::fs::File;
use std::fs::OpenOptions;

pub struct FileBackend {
    file: File
}

impl FileBackend {
    fn from_file(file: File) -> FileBackend {
        FileBackend{
            file: file
        }
    }

    pub fn from_name(name: &str) -> Result<FileBackend> {
        let file = OpenOptions::new()
            .write(true)
            .read(true)
            .create(true)
            .open(name)?;

        Ok(FileBackend::from_file(file))
    }
}

impl Backend for FileBackend {
    fn read(&mut self, offset: usize, buffer: &mut[u8]) -> Result<usize> {
        self.file.seek(SeekFrom::Start(offset as u64))?;

        self.file.read(buffer)
    }

    fn write(&mut self, offset: usize, buffer: &[u8]) -> Result<usize> {
        self.file.seek(SeekFrom::Start(offset as u64))?;

        self.file.write(buffer)
    }
}
