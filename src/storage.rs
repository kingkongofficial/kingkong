use crate::TypeCache;

static mut STORAGE: Option<TypeCache> = None;

pub fn init() {
    unsafe {
        STORAGE = Some(TypeCache::new());
    }
}

pub fn get<T: Send + Sync + 'static>() -> &'static T {

}

pub fn set<T: Send + Sync + 'static>(o: T) {
}