extern crate fuse;

mod backend;
mod wfs;
use backend::FileBackend;
use std::env;
use std::ffi::OsStr;

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let backend = FileBackend::from_name(&args[1])?;
    let fs = wfs::FileSystem::from_backend(backend);
    let options = ["-o", "ro", "-o", "fsname=wfs"]
        .iter()
        .map(|o| o.as_ref())
        .collect::<Vec<&OsStr>>();
    fuse::mount(fs, &args[2], &options)?;
    Ok(())
}
