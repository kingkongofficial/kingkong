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

#[macro_export]
macro_rules! run_with_banner {
    ($banner:expr) => {
        kingkong::run_with_banner!($banner, "0.0.0.0:7667")
    };
    ($banner:expr, $addr:expr) => {{
        kingkong::run_with_banner!($banner, $addr, self)
    }};
    ($banner:expr, $($module:ident),+) => {{
        kingkong::run_with_banner!($banner, "0.0.0.0:7667", $($module),+)
    }};
    ($banner:expr, $addr:expr, $($module:ident),+) => {{
        kingkong::setup!();
        let mut router = ::kingkong::Router::new();
        $($module::kingkong_add_to_router(&mut router);)+
        kingkong::run($addr, router, Some($banner))
    }};
}

#[macro_export]
macro_rules! use_state {
    ($state:expr) => {
        kingkong::storage::init();
        kingkong::storage::set($state);
    };
}

#[doc(hidden)]
#[macro_export]
macro_rules! setup {
    () => {
        #[cfg(bundle_assets)]
        #[macro_export]
        macro_rules! kingkong_bundled_assets {
            () => { include!(concat!(env!("OUT_DIR"), "/bundle.rs")) };
        }
        #[cfg(bundle_assets)]
        kingkong::include_assets!();
        kingkong::asset_dir!(@option option_env!("ASSET_DIR"));
    };
}

#[doc(hidden)]
#[macro_export]
macro_rules! include_assets {
    () => {
        unsafe {
            kingkong::BUNDLED_ASSETS = Some(kingkong_bundled_assets!());
        }
    };
}

#[macro_export]
macro_rules! asset_dir {
    (@option $opt:expr) => {
        if let Some(dir) = $opt {
            ::kingkong::asset_dir!(dir);
        }
    };
    ($dir:expr) => {
        unsafe {
            ::kingkong::ASSET_DIR = Some($dir.into());
        }
    };
}

#[macro_export]
macro_rules! bundle_assets {
    ($dir:expr) => {
        ::kingkong::bundle_assets($dir)
    };
}

#[macro_export]
macro_rules! routes {
    (
        $(#![filter($($all_filter:ident),+)])*

        $(
            $(#[filter($($action_filter:ident),+)])*
            $method:ident $path:expr => $body:expr;)*
        ) => {
        fn kingkong_check_method() {
            #![allow(non_snake_case)]
            fn GET() {}
            fn POST() {}
            fn PUT() {}
            fn DELETE() {}
            fn UPDATE() {}
            fn PATCH() {}
            $($method();)*
        }

        fn kingkong_filter(req: &mut ::kingkong::Request) -> Option<::kingkong::Response> {
            $($({
                if let Some(res) = $all_filter(req) {
                    return Some(res);
                }
            })+)*

            None
        }

        pub fn kingkong_add_to_router(router: &mut ::kingkong::Router) {
            $( router.insert(::kingkong::Method::$method, $path, |mut req| {
                use ::kingkong::{Request, Response, Responder};

                let b: fn(::kingkong::Request) -> _ = $body;
                let mut res = kingkong_filter(&mut req);

                $($({
                    if res.is_none() {
                        res = $action_filter(&mut req);
                    }
                })+)*

                res.unwrap_or_else(|| b(req).to_response())
            }); )*
        }
    };
}
