use {
    crate::{util::percent_decode, Method, Request, Response},
    std::cmp::max,
    std::collections::HashMap,
};

pub type Pattern = Vec<String>;

pub type Action = fn(Request) -> Response;

#[derive(Default)]
pub struct Router {
    routes: HashMap<Method, Vec(Pattern, Action)>   ,
}

impl Router {
    
}