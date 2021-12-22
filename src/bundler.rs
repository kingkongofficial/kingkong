#[allow(unused_imports)]
use {
    crate::Result,
    std::{
        env,
        fs::{self, File},
        io::Write,
        os::unix,
        path::{Path, PathBuf},
    },
};

#[doc(hidden)]
#[allow(dead_code)]
pub fn walk(dir: &str) -> std::vec::IntoIter<PathBuf> {
    if let Ok(files) = files_in_dir(dir) {
        files.into_iter()
    } else {
        vec![].into_iter()
    }
}

fn files_in_dir(path: &str) -> Result<Vec<PathBuf>> {
    let mut files = vec![];
    for entry in fs::read_dir(path)? {
        let entry = entry?;
        let path = entry.path();
        let meta = fs::metadata(&path)?;
        if meta.is_dir() {
            files.extend_from_slice(&files_in_dir(path.to_str().unwrap_or("bad"))?);
        } else {
            files.push(path);
        }
    }
    Ok(files)
}
