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
    pub fn new() -> Router {
        Router {
            routes: HashMap::new(),
        }
    }

    pub fn action_for(&self, req: &mut Request) -> Option<&Action> {

    }

    fn pattern_to_vec(pattern: &str) -> Pattern {
        pattern
            .trim_matches('/')
            .split('/')
            .flat_map(|s| {
                if let Some(idx) = s.find('.') {
                    vec![s[..idx].to_string(), s[idx..].to_string()]
                } else {
                    vec![s.to_string()]
                }
            })
            .collect::<Vec<_>>()
    }

}