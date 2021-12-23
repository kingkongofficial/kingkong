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

impl From<io::Error> for Error {
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
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "{}",
            match self {
                Error::UnknownHTTPMethod(reason) => &reason,
                Error::ConnectionClosed => "Connection Closed By Client",
                Error::ParseVersion => "Error Parsing HTTP Version",
                Error::ExpectedCRLF => "Expected CRLF in HTTP Request",
                Error::ParseHeaderName => "Error Parsing HTTP Header name",
                Error::ParseHeaderValue => "Error Parsing HTTP Header value",
                Error::ParseError => "Error Parsing HTTP Request",
                Error::AssetNotFound(..) => "Can't Find Asset",
                Error::IO(..) => "io::Error While Parsing HTTP Request",
                Error::Other(reason) => &reason,
            }
        )
    }
}

impl PartialEq for Error {
    fn eq(&self, other: &Self) -> bool {
        use Error::*;
        match self {
            IO(_) => false,
            AssetNotFound(s) => match other {
                AssetNotFound(o) => s == o,
                _ => false,
            },
            UnknownHTTPMethod(s) => match other {
                UnknownHTTPMethod(o) => s == o,
                _ => false,
            },
            Other(s) => match other {
                Other(o) => s == o,
                _ => false,
            },
            ConnectionClosed => matches!(other, ConnectionClosed),
            ParseVersion => matches!(other, ParseVersion),
            ExpectedCRLF => matches!(other, ExpectedCRLF),
            ParseHeaderName => matches!(other, ParseHeaderName),
            ParseHeaderValue => matches!(other, ParseHeaderValue),
            ParseError => matches!(other, ParseError),
        }
    }
}
