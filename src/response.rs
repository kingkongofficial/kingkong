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

impl fmt::Debug for Response {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Response")
            .field("code", &self.code)
            .field("content_type", &self.content_type())
            .field("body", &self.body.as_str())
            .finish()
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
            "text/html; charset=utf8".into(),
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

impl Response {
    pub fn new() -> Response {
        Response::default()
    }

    pub fn code(&self) -> usize {
        self.code
    }

    pub fn content_type(&self) -> &str {
        self.header("Content-Type").unwrap_or("")
    }

    pub fn body(&self) -> &str {
        self.body.as_str()
    }

    pub fn headers(&self) -> &HashMap<String, String> {
        &self.headers
    }

    pub fn header(&self, name: &str) -> Option<&str> {
        self.headers.get(&name.to_lowercase()).map(|h| h.as_ref())
    }

    pub fn set_header(&mut self, name: &str, value: &str) {
        self.headers.insert(name.to_lowercase(), value.to_string());
    }

    #[cfg(feature = "cookies")]
    pub fn cookie(&self, name: &str) -> Option<&str> {
        self.cookies.get(&name.to_lowercase()).map(|h| h.as_ref())
    }

    #[cfg(feature = "cookies")]
    pub fn set_cookie(&mut self, name: &str, value: &str) {
        self.cookies.insert(name.to_lowercase(), value.into());
    }

    #[cfg(feature = "cookies")]
    pub fn remove_cookie(&mut self, name: &str) {
        self.cookies.insert(name.to_lowercase(), "".into());
    }

    pub fn from<T: Into<Response>>(from: T) -> Response {
        from.into()
    }

    pub fn from_asset(path: &str) -> Response {
        Response::default().with_asset(path)
    }

    pub fn from_reader(reader: Box<dyn io::Read>) -> Response {
        Response::default().with_reader(reader)
    }

    pub fn from_file(path: &str) -> Response {
        Response::default().with_file(path)
    }

    pub fn from_error<E: error::Error>(err: E) -> Response {
        Response::default().with_error(err)
    }

    pub fn from_header(name: &str, value: &str) -> Response {
        Response::default().with_header(name, value)
    }

    #[cfg(feature = "cookies")]
    pub fn from_cookie(name: &str, value: &str) -> Response {
        Response::default().with_cookie(name, value)
    }

    pub fn from_body<S: AsRef<str>>(body: S) -> Response {
        Response::default().with_body(body)
    }

    pub fn from_text<S: AsRef<str>>(text: S) -> Response {
        Response::default().with_text(text)
    }

    pub fn from_code(code: usize) -> Response {
        Response::default().with_code(code)
    }

    pub fn with_code(mut self, code: usize) -> Response {
        self.code = code;
        match code {
            404 => self.with_body("404 Not Found"),
            500 => self.with_body("500 Internal Server Error"),
            _ => self,
        }
    }

    pub fn with_body<S: AsRef<str>>(mut self, body: S) -> Response {
        let body = body.as_ref();
        self.body = Body::String(body.to_string());
        self.set_header("Content-Length", &body.len().to_string());
        self
    }

    #[cfg(feature = "json_serde")]
    pub fn with_json<T: serde::Serialize>(self, value: T) -> Response {
        self.with_body(
            serde_json::to_value(value)
                .expect("Serialize failed")
                .to_string(),
        )
        .with_header("Content-Type", "application/json")
    }

    pub fn with_text<S: AsRef<str>>(self, text: S) -> Response {
        self.with_body(text)
            .with_header("Content-Type", "text/plain; charset=utf8")
    }

    pub fn with_reader(mut self, reader: Box<dyn io::Read>) -> Response {
        self.body = Body::Reader(reader);
        self
    }

    pub fn with_asset(mut self, path: &str) -> Response {
        if let Some(path) = asset::normalize_path(path) {
            if asset::exists(&path) {
                if asset::is_bundled() {
                    if let Some(reader) = asset::as_reader(&path) {
                        self.set_header("ETag", asset::etag(&path).as_ref());
                        self.set_header("Content-Type", util::content_type(&path));
                        self.set_header("Content-Length", &asset::size(&path).to_string());
                        return self.with_reader(reader);
                    }
                } else {
                    return self.with_file(&path);
                }
            }
        }
        self.with_code(404)
    }

    pub fn with_file(mut self, path: &str) -> Response {
        if !std::path::Path::new(path).exists() {
            return Response::from(404);
        }
        match fs::File::open(path) {
            Ok(file) => {
                self.set_header("ETag", &asset::etag(path).as_ref());
                self.set_header("Content-Type", util::content_type(path));
                self.set_header("Content-Length", &util::file_size(path).to_string());
                self.with_reader(Box::new(BufReader::new(file)))
            }

            Err(e) => self.with_error(Box::new(e)),
        }
    }

    pub fn with_error<E: error::Error>(self, err: E) -> Response {
        self.with_code(500)
            .with_body(&format!("<h1>500 Internal Error</h1><pre>{:?}", err))
    }

    pub fn with_header(mut self, key: &str, value: &str) -> Response {
        self.set_header(key, value);
        self
    }

    #[cfg(feature = "cookies")]
    pub fn with_cookie(mut self, key: &str, value: &str) -> Response {
        self.set_cookie(key, value);
        self
    }

    #[cfg(feature = "cookies")]
    pub fn without_cookie(mut self, key: &str) -> Response {
        self.remove_cookie(key);
        self
    }

    pub fn len(&self) -> usize {
        match &self.body {
            Body::String(s) => s.len(),
            Body::Reader(..) => self
                .header("Content-Length")
                .unwrap_or("0")
                .parse()
                .unwrap_or(0),
            _ => 0,
        }
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    pub fn redirect_to<U: AsRef<str>>(url: U) -> Response {
        Response::from(302).with_header("location", url.as_ref())
    }

    pub fn write<W: io::Write>(self, mut w: W) -> Result<()> {
        let mut header = format!(
            "HTTP/1.1 {} OK\r\nServer: ~ kingkong {} ~\r\nDate: {}\r\nConnection: close\r\n",
            self.code,
            crate::VERSION,
            util::http_current_date(),
        );

        header.push_str(
            &self
                .headers
                .iter()
                .map(|(key, val)| format!("{}: {}", key, val))
                .collect::<Vec<_>>()
                .join("\r\n"),
        );

        if !header.ends_with("\r\n") {
            header.push_str("\r\n");
        }

        #[cfg(feature = "cookies")]
        {
            for (name, val) in self.cookies {
                header.push_str("Set-Cookie: ");
                header.push_str(&name);
                header.push('=');
                if val.is_empty() {
                    header.push_str("; Expires=Thu, 01 Jan 1970 00:00:00 GMT");
                } else {
                    header.push_str(&val);
                }
                header.push_str("\r\n");
            }
        }

        header.push_str("\r\n");
        w.write_all(header.as_bytes())?;

        match self.body {
            Body::Reader(mut reader) => {
                io::copy(&mut reader, &mut w)?;
            }
            Body::String(s) => {
                w.write_all(s.as_bytes())?;
            }
            _ => {}
        }

        w.flush()?;

        Ok(())
    }
}

impl From<&str> for Response {
    fn from(s: &str) -> Response {
        Response::from_body(s.to_string())
    }
}

impl From<&String> for Response {
    fn from(s: &String) -> Response {
        Response::from_body(s.clone())
    }
}

impl From<String> for Response {
    fn from(body: String) -> Response {
        Response::from_body(body)
    }
}

impl From<usize> for Response {
    fn from(i: usize) -> Response {
        Response::from_code(i)
    }
}

impl From<std::borrow::Cow<'_, [u8]>> for Response {
    fn from(i: std::borrow::Cow<'_, [u8]>) -> Response {
        Response::from_body(String::from_utf8_lossy(&i).to_string())
    }
}
