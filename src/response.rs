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

impl PartialEq for Response {
    fn eq(&self, other: &Self) -> bool {
        self.code == other.code
            && self.headers == other.headers
            && self.content_type() == other.content_type()
            && self.body.as_str() == other.body.as_str()
    }
}

impl fmt::Display for Response {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.body)
    }
}

impl Default for Response {
    fn default() -> Response {
        let mut headers = HashMap::new();
        headers.insert(
            "Content-Type".to_lowercase(),
            "text/html; charset=utf-8".into(),
        );
        headers.insert("Content-Length".to_lowercase(), "0".into());

        Response {
            code: 200,
            body: Body::None,
            headers,

            #[cfg(feature = "cookies")]
            cookies: HashMap::new(),
        }
    }
}