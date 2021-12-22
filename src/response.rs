use {
    crate::{asset, util, Result},
    std::{
        collections::HashMap,
        error, fmt, fs,
        io::{self, BufReader},
    },
};

enum Body {
    None,
    String(String),
    Reader(Box<dyn io::Read>),
}