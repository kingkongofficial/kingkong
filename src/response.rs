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

impl Body {
    fn as_str(&self) -> &str {
        if let Body::String(s) = self {
            s.as_ref()
        } else {
            ""
        }
    }
}