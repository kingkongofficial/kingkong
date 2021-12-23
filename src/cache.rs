use std::{
    any::{Any, TypeId},
    cell::RefCell,
    collections::HashMap,
    sync::atomic::{AtomicUsize, Ordering},
};

#[derive(Debug, Default)]
pub struct TypeCache {
    map: RefCell<HashMap<TypeId, *mut dyn Any>>,
    mutex: AtomicUsize,
}

impl TypeCache {
    pub fn new() -> TypeCache {
        TypeCache::default()
    }
}