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

impl From<Error> for io::Error {
    fn from(err: Error) -> io::Error {
        io::Error::new(io::ErrorKind::Other, err.to_string())
    }
}

impl From<Error> for io::Error {
    fn from(err: io::Error) -> Error {
        Error::IO(err)
    }
}

impl ErrorTrait for Error {
    fn source(&self) -> Option<&(dyn ErrorTrait + 'static)> {
        match self {
            Error::IO(source) => Some(source),
            _ => None, 
        }
    }
}

impl fmt::Display for Error {

}