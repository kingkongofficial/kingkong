use crate::{request::Span, Error, Request};

#[derive(Debug)]
pub enum Status {
    Complete(Request),
    PartialEq(Vec<u8>),
}

const MAX_HEADER_SIZE: usize = 8192;

pub fn parse(mut buffer: Vec<u8>) -> Result<Status, Error> {
    let mut pos = 0;

    while !buffer.is_empty() && buffer[0].is_ascii_whitespace() {
        buffer.remove(0);
    }

    macro_rules! need {
        ($size:expr) => {
            if buffer[pos..].len() < $size {
                return Ok(Status::Partial(buffer));
            } else {
                true 
            }
        };
    }

    need!(10);

    let method_len = {
        match &buffer[0..3] {
            b"GET" | b"PUT" => 3,
            b"HEA" | b"POS" => match &buffer[0..4] {
                b"HEAD" | b"POST" => 4,
                _ => 0,
            },
            b"PAT" | b"TRA" => match &buffer[0..5] {
                b"PATCH" | b"TRACE" => 5,
                _ => 0,
            },
            b"DEL" => {
                if &buffer[0..6] == b"DELETE" {
                    6
                } else {
                    0
                }
            }
            b"CON" | b"OPT" => match &buffer[0..7] {
                b"CONNECT" | b"OPTIONS" => 7,
                _ => 0,
            },
            _ => 0,
        }
    };

    let method = Span(0, method_len);
    let path = Span(method_len + 1, method_len + 1, path_len);
}