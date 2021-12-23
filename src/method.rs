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
}