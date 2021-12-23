use crate::Error;

#[derive(PartialEq, Eq, Hash, Debug)]
pub enum Method {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    PATCH,
    OPTIONS,
    TRACE,
}

impl std::str::FromStr for Method {
    type Err = Error;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        Ok(match s {
            "GET" => Method::GET,
            "HEAD" => Method::HEAD,
            "POST" => Method::POST,
            "PUT" => Method::PUT,
            "DELETE" => Method::DELETE,
            "PATCH" => Method::PATCH,
            "OPTIONS" => Method::OPTIONS,
            "TRACE" => Method::TRACE,
            _ => return Err(Error::UnknownHTTPMethod(s.into())),
        })
    }
}

impl From<&str> for Method {
    fn from(s: &str) -> Method {
        s.parse().unwrap_or(Method::GET)
    }
}
