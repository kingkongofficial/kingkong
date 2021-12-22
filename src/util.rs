const HTTP_DATE_FMT: &str = "%a, %d %b %Y %H:%M:%S";

pub fn file_size(path: &str) -> usize {
    std::fs::File::open(path)
        .map(|f| f.metadata().map(|m| m.len()).unwrap_or(0))
        .unwrap_or(0) as usize
}