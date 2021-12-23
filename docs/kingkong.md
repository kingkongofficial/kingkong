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

## Serving static file:
```rust
use vial::prelude::*;

routes! {
    // kingkong supports serving .jpeg, .jpg, png, .gif
    GET "/" => |_| "<img src='/yourimage.jpeg' />";
    // serving html files also works
    GET "/index" => |_| Response::from_asset("index.html");
}

fn main() {
    // specify the dir where the image is available :)
    asset_dir!("./assets");
    // run the wapp
    run!().unwrap();
}
```
