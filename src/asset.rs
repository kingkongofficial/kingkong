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