fn main() {
    println!(
        "cargo:rustc-env=BUILD_DATE={}",
        sh("date +%Y-%m-%d_%H:%M:%S%p")
    );
}


fn sh(args: &str) -> String {
    use std::process::Command;
    
    let args: Vec<_> = args.split('')
}