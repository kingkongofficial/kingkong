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

impl fmt::Display for Body {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Body::String(s) => write!(f, "{}", s),
            Body::Reader(..) => write!(f, "(io::Read)"),
            _ => write!(f, "None"),
        }
    }
}

pub struct Response {
    code: usize,
    headers: HashMap<String, String>,
    body: Body,
    
    #[cfg(feature = "cookies")]
    cookies: HashMap<String, String>,
}