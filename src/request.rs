use {
    crate::{http_parser, util, Error, Result, TypeCache},
    std::{borrow::Cow, collections::HashMap, fmt, io, mem, rc::Rc, str},
};

#[cfg(feature = "cookies")]
use cookie2::Cookie;

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