# Learn kingkong
- learn how to use kingkong web framework

# About:
- a simple rust framework to build rest api's

# Tutorials:

## Installation:

- installing kingkong in your project
```toml
[dependencies]
kingkong = { git = "https://github.com/kingkongofficial/kingkong" }
```


## Helloworld:
```rust
use kingkong::prelude::*;

routes! {
    GET "/" => |_| "Hello World.";
}

fn main() {
    kingkong::run!().unwrap();
}
```
