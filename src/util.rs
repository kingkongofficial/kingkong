const HTTP_DATE_FMT: &str = "%a, %d %b %Y %H:%M:%S";

pub fn file_size(path: &str) -> usize {
    std::fs::File::open(path)
        .map(|f| f.metadata().map(|m| m.len()).unwrap_or(0))
        .unwrap_or(0) as usize
}

pub fn decode_form_value(post: &str) -> String {
    let cleaned = post.replace('+', " ").replace('\r', "");
    percent_decode(&cleaned).unwrap_or_else(|| "".into())
}

pub fn http_current_date() -> String {
    let now = libc_strftime::epoch();
    libc_strftime::strftime_gmt(HTTP_DATE_FMT, now) + " GMT"
}

pub fn percent_decode(mut inp: &str) -> Option<String> {
    let mut out = Vec::new();
    loop {
        let next_pct = match inp.find('%') {
            Some(l) if l < inp.len() - 2 => l,
            Some(_) => return None,
            None => break,
        };
        let (push, pct_rest) = inp.split_at(next_pct);
        out.extend_from_slice(push.as_bytes());
        let (pct, rest) = pct_rest.split_at(3);
        inp = rest;
        let val = u8::from_str_radix(&pct[1..], 16).ok()?;
        out.push(val);
    }
    out.extend_from_slice(inp.as_bytes());
    String::from_utf8(out).ok()
}

pub fn content_type(path: &str) -> &'static str {

}