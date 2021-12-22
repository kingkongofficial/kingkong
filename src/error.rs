use std::{error::Error as ErrorTrait, fmt, io};

#[derive(Debug)]
pub enum Error {
    AssetNotFound(String),
    ConnectionClosed,
    UnknownHTTPMethod(String),
    ParseVersion,
    ExpectedCRLF,
    ParseHeaderName,
    ParseHeaderValue,
    ParseError,
    IO(io::Error),
    Other(String),
}