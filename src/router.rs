use {
    crate::{util::percent_decode, Method, Request, Response},
    std::cmp::max,
    std::collections::HashMap,
};

pub type Pattern = Vec<String>;

pub type Action = fn(Request) -> Response;

#[derive(Default)]
pub struct Router {
    routes: HashMap<Method, Vec<(Pattern, Action)>>,
}

impl Router {
    pub fn new() -> Router {
        Router {
            routes: HashMap::new(),
        }
    }

    pub fn action_for(&self, req: &mut Request) -> Option<&Action> {
        if let Some(routes) = self.routes.get(&req.method().into()) {
            let req_parts = Self::pattern_to_vec(req.path());

            'outer: for (pattern, action) in routes {
                for i in 0..max(req_parts.len(), pattern.len()) {
                    if i >= pattern.len() {
                        continue 'outer;
                    }
                    let pattern_part = &pattern[i];
                    if i >= req_parts.len() {
                        continue 'outer;
                    }
                    let req_part = &req_parts[i];
                    if pattern_part.starts_with(':') && !req_part.is_empty() {
                        if let Some(decoded) = percent_decode(req_part) {
                            req.set_arg(pattern_part.trim_start_matches(':').into(), decoded);
                        }
                        continue;
                    } else if pattern_part.starts_with('*') && !req_part.is_empty() {
                        if let Some(idx) = req.path().find(&req_parts[i]) {
                            if let Some(decoded) = percent_decode(&req.path()[idx..]) {
                                req.set_arg(pattern_part.trim_start_matches('*').into(), decoded);
                            }
                        }
                        return Some(action);
                    } else if req_part == pattern_part {
                        continue;
                    } else {
                        continue 'outer;
                    }
                }
                return Some(action);
            }
        }
        None
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

    pub fn insert<T: Into<Method>>(&mut self, method: T, pattern: &'static str, action: Action) {
        let method = method.into();
        let pattern_parts = Self::pattern_to_vec(pattern);

        if let Some(map) = self.routes.get_mut(&method) {
            map.push((pattern_parts, action));
        } else {
            self.routes.insert(method, vec![(pattern_parts, action)]);
        }
    }
}
