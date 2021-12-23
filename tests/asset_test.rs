use kingkong::{self, asset};

/**
 * entry point
 */
#[test]
fn asset_tests() {
    asset_exists();
}

fn asset_exists() {
    kingkong::asset_dir!("./tests/assets/");

    assert!(asset::exists("kingkong.jpeg"));
}