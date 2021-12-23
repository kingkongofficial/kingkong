use {
    crate::{http_parser, util, Error, Result, TypeCache},
    std::{borrow::Cow, collections::HashMap, fmt, io, mem, rc::Rc, str},
};

#[cfg(feature = "cookies")]
use cookie2::Cookie;

#[derive(Debug, Default, Copy, Clone, PartialEq)]
pub struct Span(pub usize, pub usize);

impl Span {
    pub fn new() -> Span {
        Span::default()
    }

    pub fn is_empty(&self) -> bool {
        self.0 == 0 && self.1 == 0
    }

    pub fn from_buf<'buf>(&self, buf: &'buf [u8]) -> &'buf str {
        if self.is_empty() {
            ""
        } else if self.1 >= self.0 && self.1 <= buf.len() {
            str::from_utf8(&buf[self.0..self.1]).unwrap_or("?")
        } else {
            "?"
        }
    }
}
pub struct Request {
    buffer: Vec<u8>,

    path: Span,

    method: Span,

    headers: Vec<(Span, Span)>,

    body: Span,

    args: HashMap<String, String>,
    form: HashMap<String, String>,

    cache: Rc<TypeCache>,

    #[cfg(feature = "cookies")]
    cookies: Vec<(String, String)>,
}

impl fmt::Debug for Request {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let mut headers = HashMap::new();
        for (k, v) in &self.headers {
            headers.insert(k.from_buf(&self.buffer), v.from_buf(&self.buffer));
        }
        f.debug_struct("Request")
            .field("method", &self.method.from_buf(&self.buffer))
            .field("path", &self.path.from_buf(&self.buffer))
            .field("full_path", &self.path.from_buf(&self.buffer))
            .field("headers", &headers)
            .finish()
    }
}

impl Request {
    pub fn new(
        method: Span,
        path: Span,
        headers: Vec<(Span, Span)>,
        body: Span,
        buffer: Vec<u8>,
    ) -> Request {
        Request {
            method,
            path,
            headers,
            body,
            buffer,
            ..Request::default()
        }
    }

    pub fn default() -> Request {
        Request {
            path: Span::new(),
            method: Span::new(),
            body: Span::new(),
            headers: Vec::new(),
            args: HashMap::new(),
            form: HashMap::new(),
            buffer: Vec::new(),
            cache: Rc::new(TypeCache::new()),

            #[cfg(feature = "cookies")]
            cookies: vec![],
        }
    }

    pub fn from_reader<R: io::Read>(mut reader: R) -> Result<Request> {
        let mut buffer = Vec::with_capacity(512);
        let mut read_buf = [0u8; 512];

        let mut req = loop {
            let n = reader.read(&mut read_buf)?;
            if n == 0 {
                return Err(Error::ConnectionClosed);
            }
            buffer.extend_from_slice(&read_buf[..n]);
            match http_parser::parse(mem::replace(&mut buffer, vec![]))? {
                http_parser::Status::Complete(req) => break req,
                http_parser::Status::Partial(b) => {
                    let _ = mem::replace(&mut buffer, b);
                }
            }
        };

        if let Some(size) = req.header("Content-Length") {
            let size = size.parse().unwrap_or(0);
            let start = req.body.0;

            while req.buffer[start..].len() < size {
                let n = reader.read(&mut read_buf)?;
                if n == 0 {
                    break;
                }
                req.buffer.extend_from_slice(&read_buf[..n]);
            }
            req.body.1 = req.body.0 + size;
            req.parse_form();
        }

        #[cfg(feature = "cookies")]
        {
            if let Some(cookie) = req.header("Cookie") {
                let cookie = Cookie::parse(cookie).map_err(|e| Error::Other(e.to_string()))?;
                let name = cookie.name().to_owned();
                let val = util::percent_decode(cookie.value()).unwrap();
                req.cookies.push((name, val));
            }
        }

        Ok(req)
    }

    pub fn path(&self) -> &str {
        let span = if let Some(idx) = self.full_path().find('?') {
            Span(self.path.0, self.path.0 + idx)
        } else {
            self.path
        };
        span.from_buf(&self.buffer)
    }

    pub fn full_path(&self) -> &str {
        self.path.from_buf(&self.buffer)
    }

    pub fn from_path(path: &str) -> Request {
        Request::default().with_path(path)
    }

    pub fn set_path(&mut self, path: &str) {
        self.path = Span(self.buffer.len(), self.buffer.len() + path.len());
        self.buffer.extend(path.as_bytes());
    }

    pub fn with_path(mut self, path: &str) -> Request {
        self.set_path(path);
        self
    }

    pub fn body(&self) -> &str {
        self.body.from_buf(&self.buffer)
    }

    pub fn set_body<S: AsRef<str>>(&mut self, body: S) {
        self.body = Span(self.buffer.len(), self.buffer.len() + body.as_ref().len());
        self.buffer.extend(body.as_ref().as_bytes());
    }

    pub fn with_body<S: AsRef<str>>(mut self, body: S) -> Request {
        self.set_body(body);
        self
    }

    #[cfg(feature = "json_serde")]
    pub fn json<'a, T: serde::Deserialize<'a>>(&'a self) -> serde_json::Result<T> {
        Ok(serde_json::from_str(self.body())?)
    }

    pub fn method(&self) -> &str {
        self.method.from_buf(&self.buffer)
    }

    pub fn set_method(&mut self, method: &str) {
        self.method = Span(self.buffer.len(), self.buffer.len() + method.len());
        self.buffer.extend(method.as_bytes());
    }

    pub fn with_method(mut self, method: &str) -> Request {
        self.set_method(method);
        self
    }

    pub fn arg(&self, name: &str) -> Option<&str> {
        self.args.get(name).map(|v| v.as_ref())
    }

    pub fn set_arg(&mut self, name: String, value: String) {
        self.args.insert(name, value);
    }

    #[doc(hidden)]
    pub fn headers(&self) -> &Vec<(Span, Span)> {
        &self.headers
    }

    pub fn header(&self, name: &str) -> Option<Cow<str>> {
        let name = name.to_lowercase();
        let mut headers = self
            .headers
            .iter()
            .filter(|(n, _)| n.from_buf(&self.buffer).to_ascii_lowercase() == name)
            .map(|(_, v)| v.from_buf(&self.buffer).trim_end());

        let count = headers.clone().count();
        if count == 0 {
            None
        } else if count == 1 {
            Some(Cow::from(headers.next().unwrap()))
        } else {
            Some(Cow::from(headers.collect::<Vec<_>>().join(", ")))
        }
    }

    pub fn has_form(&mut self, name: &str) -> bool {
        self.form(name).is_some()
    }

    pub fn form(&self, name: &str) -> Option<&str> {
        self.form.get(name).map(|s| s.as_ref())
    }

    pub fn set_form(&mut self, name: &str, value: &str) {
        self.form.insert(name.to_string(), value.to_string());
    }

    #[doc(Hidden)]
    pub fn parse_form(&mut self) {
        let mut map = HashMap::new();
        for kv in self.body().split('&') {
            let mut parts = kv.splitn(2, '=');
            if let Some(key) = parts.next() {
                if let Some(val) = parts.next() {
                    map.insert(key.to_string(), util::decode_form_value(val));
                } else {
                    map.insert(key.to_string(), String::new());
                };
            }
        }
        self.form = map;
    }

    pub fn has_query(&self, name: &str) -> bool {
        self.query(name).is_some()
    }

    pub fn query(&self, name: &str) -> Option<&str> {
        let idx = self.full_path().find('?')?;
        self.full_path()[idx + 1..]
            .split('&')
            .filter_map(|s| {
                if s.starts_with(name) && s[name.len()..].starts_with('=') {
                    Some(&s[name.len() + 1..])
                } else {
                    None
                }
            })
            .next()
    }

    pub fn cache<T, F>(&self, fun: F) -> &T
    where
        F: FnOnce(&Request) -> T,
        T: Send + Sync + 'static,
    {
        self.cache.get().unwrap_or_else(|| {
            self.cache.set(fun(&self));
            self.cache.get().unwrap()
        })
    }

    pub fn state<T: Send + Sync + 'static>(&self) -> &T {
        crate::storage::get::<T>()
    }

    #[cfg(feature = "cookies")]
    pub fn cookie(&self, name: &str) -> Option<&str> {
        self.cookies
            .iter()
            .find_map(|(k, v)| if k == name { Some(v.as_ref()) } else { None })
    }
}
