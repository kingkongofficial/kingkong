#[macro_export]
macro_rules! run {
    () => {
        kingkong::run!("0.0.0.0:7667")
    };
    ($addr:expr) => {{
        kingkong::run!($addr, self)
    }};
    ($($module:ident),+) => {{
        kingkong::run!("0.0.0.0:7667", $($module),+)
    }};
    ($addr:expr, $($module:ident),+) => {{
        kingkong::setup!();
        let mut router = ::kingkong::Router::new();
        $($module::kingkong_add_to_router(&mut router);)+
        kingkong::run($addr, router, None)
    }};
}