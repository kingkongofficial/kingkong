use crate::Response;

pub trait Responder {
    fn to_response(self) -> Response;
}

impl Responsed for Response {
    fn to_response(self) -> Response {
        self
    }
}

impl Responder for &str {
    fn to_response(self) -> Response {
        Response::from(self)
    }
}