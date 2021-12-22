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
    
}