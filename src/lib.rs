#![warn(missing_docs)]
#![allow(clippy::needless_doctest_main)]
#![allow(clippy::large_enum_variant)]

#[macro_use]
mod macros;
pub mod asset;

mod cache;
mod error;
mod method;
pub mod prelude;
mod request;
mod responder;
mod response;
mod router;
mod server;

#[doc(hidden)]
pub mod bundler;
#[doc(hidden)]
pub mod http_parser;
#[doc(hidden)]
pub mod util;

#[doc(hidden)]
pub mod storage;

#[cfg(feature = "horror")]
#[doc(hidden)]
pub mod horrorshow;

pub use {
    bundler::bundle_assets, cache::TypeCache, error::Error, method::Method, request::Request,
    responder::Responder, response::Response, router::Router, server::run,
};

pub type Result<T> = std::result::Result<T, Error>;

pub static mut ASSET_DIR: Option<String> = None;

pub static mut BUNDLED_ASSETS: Option<std::collections::HashMap<String, &'static [u8]>> = None;

pub const BUILD_DATE: &str = env!("BUILD_DATE");

pub const VERSION: &str = env!("CARGO_PKG_VERSION");
