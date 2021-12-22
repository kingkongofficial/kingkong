use {
    crate::{http_parser, util, Error, Result, TypeCache},
    std::{borrow::Cow, collections::HashMap, fmt, io, mem, rc::Rc, str},
};

#[cfg(feature = "cookies")]
use cookie2::Cookie;

impl Scan {
    pub fn new() -> Span {
        Span::default()
    }

    pub fn is_empty(&self) -> bool {
        self.0 == 0 && self.1 == 0
    }
}