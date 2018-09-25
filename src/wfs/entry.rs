use std::fmt;
use std::ffi::OsStr;
use std::os::unix::ffi::OsStrExt;

#[repr(packed)]
#[repr(C)]
pub struct FileEntry {
    name: [u8; 58],
    start_block: u16,
    file_size: u32
}

impl FileEntry {
    pub fn name(&self) -> &OsStr {
        match self.name.iter().enumerate().find(|&r| *r.1 == 0) {
            Some((pos, _)) => OsStr::from_bytes(&self.name[..pos]),
            None => OsStr::new("")
        }
    }

    pub fn is_empty(&self) -> bool {
        self.start_block == 0 || self.name[0] == 0
    }

    pub fn size(&self) -> usize {
        (self.file_size as usize) & 0xffffff
    }

    pub fn is_dir(&self) -> bool {
        ((self.file_size) & (1 << 31)) != 0
    }
}

impl fmt::Debug for FileEntry {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> Result<(), fmt::Error> {
        let name = self.name().to_string_lossy();
        write!(formatter, "FileEntry({}, size: {}, start: {}, dir: {})",
            name, self.size(), self.start_block, self.is_dir())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_empty_entry() {
        let entry = FileEntry{
            name: [0u8; 58],
            file_size: 0,
            start_block: 0
        };

        assert_eq!("", entry.name());
        assert_eq!(0, entry.size());
        assert!(entry.is_empty());
    }

    #[test]
    fn test_valid_entry() {
        let mut entry = FileEntry{
            name: [0u8; 58],
            file_size: 4,
            start_block: 42
        };

        entry.name[..13].copy_from_slice(b"Hello world!\0");

        assert_eq!("Hello world!", entry.name());
        assert_eq!(4, entry.size());
        assert!(!entry.is_empty());
    }
}
