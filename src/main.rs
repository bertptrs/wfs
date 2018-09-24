mod backend;
use backend::FileBackend;
use std::env;

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let mut backend = FileBackend::from_name(&args[1])?;
    Ok(())
}
