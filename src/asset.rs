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
    if is_bundled() {
        Cow::from(crate::BUILD_DATE)
    } else {
        let mut hasher = DefaultHasher::new();
        last_modified(path).hash(&mut hasher);
        Cow::from(format!("{:x}", hasher.finish()))
    }
}

fn last_modified(path: &str) -> Option<String> {
    if is_bundled() {
        return None;
    }

    let path = normalize_path(path)?;
    if let Ok(meta) = fs::metadata(path) {
        if let Ok(time) = meta.modified() {
            return Some(format!("{:?}", time));
        }
    }
    None
}

pub fn normalize_path(path: &str) -> Option<String> {
    if let Some(root) = asset_dir() {
        Some(format!(
            "{}/{}",
            root.trim_end_matches('/'),
            path.trim_start_matches(root)
                .trim_start_matches('.')
                .trim_start_matches('/')
                .replace("..", ".")
        ))
    } else {
        None
    }
}

pub fn is_bundled() -> bool {
    bundled_assets().is_some()
}

fn bundled_assets() -> Option<&'static HashMap<String, &'static [u8]>> {
    unsafe { crate::BUNDLED_ASSETS.as_ref() }
}

pub fn size(path: &str) -> usize {
    if !exists(path) {
        return 0;
    }

    let path = match normalize_path(path) {
        Some(path) => path,
        None => return 0,
    };

    if is_bundled() {
        bundled_assets()
            .unwrap()
            .get(&path)
            .map(|a| a.len())
            .unwrap_or(0)
    } else {
        util::file_size(&path)
    }
}

fn asset_dir() -> Option<&'static String> {
    unsafe { crate::ASSET_DIR.as_ref() }
}

pub fn exists(path: &str) -> bool {
    if let Some(path) = normalize_path(path) {
        if is_bundled() {
            return bundled_assets().unwrap().contains_key(&path);
        } else if let Ok(file) = fs::File::open(path) {
            if let Ok(meta) = file.metadata() {
                return !meta.is_dir();
            }
        }
    }
    false
}

pub fn to_string(path: &str) -> Result<String> {
    if let Some(bytes) = read(path) {
        if let Ok(utf8) = str::from_utf8(bytes.as_ref()) {
            return Ok(utf8.to_string());
        }
    }

    Err(Error::AssetNotFound(path.into()))
}

pub fn as_reader(path: &str) -> Option<Box<dyn io::Read>> {
    let path = normalize_path(path)?;
    if is_bundled() {
        if let Some(v) = bundled_assets().unwrap().get(&path) {
            return Some(Box::new(*v));
        }
    } else if let Ok(file) = fs::File::open(path) {
        if let Ok(meta) = file.metadata() {
            if !meta.is_dir() {
                return Some(Box::new(BufReader::new(file)));
            }
        }
    }
    None
}

pub fn read(path: &str) -> Option<Cow<'static, [u8]>> {
    let path = normalize_path(path)?;
    if is_bundled() {
        if let Some(v) = bundled_assets().unwrap().get(&path) {
            return Some(Cow::from(*v));
        }
    } else {
        let mut buf = vec![];
        if let Ok(mut file) = fs::File::open(path) {
            if let Ok(meta) = file.metadata() {
                if !meta.is_dir() && file.read_to_end(&mut buf).is_ok() {
                    return Some(Cow::from(buf));
                }
            }
        }
    }
    None
}
