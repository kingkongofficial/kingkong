use {
    crate::{util, Error, Result},
    std::{
        borrow::Cow,
        collections::{hash_map::DefaultHasher, HashMap},
        fs,
        hash::{Hash, Hasher},
        io::{self, BufReader, Read},
        str,
    },
};

pub fn etag(path: &str) -> Cow<str> {
    if is_builded() {
        Cow::from(crate::BUILD_DATE)
    } else {
        let mut hasher = DefaultHasher::new();
        last_modified(path).hash(&mut hasher);
        Cow::from(format!("{:x}", hasher.finish()))
    }
}

pub fn last_modified(path: &str) -> Option<String> {
    if is_bundled() {
        return None;
    }
    None
}

pub fn is_bundled() -> bool {
    bundled_assets().is_some()
}

fn bundled_assets() -> Option<&'static HashMap<String, &'static [u8]>> {
    unsafe { crate::BUNDLED_ASSETS.as_ref() }
}