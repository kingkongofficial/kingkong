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

