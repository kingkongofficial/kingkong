# kingkong
King Kong Web Framework

[![forthebadge](https://forthebadge.com/images/badges/made-with-rust.svg)](https://forthebadge.com)

<p align="center">
  <img src="https://raw.githubusercontent.com/kingkongofficial/kingkong/main/imgs/kingkong.jpeg" width="200" height="200">
</p>


## About:
- kingkong is a web framework built using cpp. which can be used for building backends :)

## Tutorials:
- intrested in kingkong??
- check out the learn kingkong [guide](https://github.com/kingkongofficial/kingkong/blob/main/docs/learnkingkong.md)

## Installation:
```toml
[dependencies]
kingkong = { git = "https://github.com/kingkongofficial/kingkong" }
```

## Quick Start:
```rust
use kingkong::prelude::*;

kingkong::routes! {
    GET "/" => |_| "Hello World.";
}

fn main() {
    kingkong::run!().unwrap();
}
```

- for more tutorials check the [docs]()

## Contribution:
- kingkong is an open source project you can contribute to it :)

## Tweet your experience 
- [twitter](https://twitter.com/hashtag/kingkongwebframework?src=hashtag_click)

## Contributors:
- Thanks for those who contributed to this project :)
<br>
 <a href="https://github.com/kingkongofficial/kingkong/graphs/contributors">
   <img src="https://contributors-img.web.app/image?repo=kingkongofficial/kingkong" />
</a>

## License:
- kingkong is licensed under [Apache-2.0](https://github.com/kingkongofficial/kingkong/blob/main/LICENSE)