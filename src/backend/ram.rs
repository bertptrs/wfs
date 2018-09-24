use backend::Backend;
use std::cmp;
use std::io::Result;

#[derive(Default)]
pub struct RAMBackend {
    data: Vec<u8>
}

impl Backend for RAMBackend {
    fn read(&mut self, offset: usize, buffer: &mut[u8]) -> Result<usize> {
        let copy_len = cmp::min(self.data.len() - offset, buffer.len());
        buffer[..copy_len].copy_from_slice(&self.data[offset..offset + copy_len]);

        for i in copy_len..buffer.len() {
            buffer[i] = 0;
        }

        Ok(buffer.len())
    }

    fn write(&mut self, offset: usize, buffer: &[u8]) -> Result<usize> {
        if offset + buffer.len() > self.data.len() {
            self.data.resize(offset + buffer.len(), 0);
        }

        self.data[offset..offset + buffer.len()].copy_from_slice(buffer);

        Ok(buffer.len())
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_read_write() {
        let mut instance: RAMBackend = Default::default();

        // Ensure that we can write something
        let mut wbuf = [0u8; 1024];
        wbuf[12] = 36;
        match instance.write(4, &wbuf) {
            Ok(1024) => {},
            _ => assert!(false)
        }

        // Ensure that we can read it back again.
        let mut rbuf = [1u8; 2048];
        match instance.read(1, &mut rbuf) {
            Ok(2048) => {
                // Ensure that our written value is there
                assert_eq!(36, rbuf[15]);

                // Ensure that all the ones are zeros now
                assert_eq!(36u8, rbuf.iter().sum())
            },
            _ => assert!(false)
        }
    }
}
