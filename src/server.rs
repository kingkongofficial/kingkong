use {
    crate::{asset, Request, Response, Result, Router},
    std::{
        io::Write,
        net::{TcpListener, TcpStream, ToSocketAddrs},
        sync::{Arc, Mutex},
    },
    threadpool::ThreadPool,
};

const MAX_CONNECTIONS: usize = 10;

#[doc(hidden)]
pub fn run<T: ToSocketAddrs>(addr: T, router: Router, banner: Option<&str>) -> Result<()> {
    let pool = ThreadPool::new(MAX_CONNECTIONS);
    let listener = TcpListener::bind(&addr)?;
    let addr = listener.local_addr()?;
    let server = Arc::new(Server::new(router));

    #[cfg(feature = "state")]
    eprintln!("! vial feature `state` is now built-in. You can safely remove it.");

    if let Some(banner) = banner {
        if !banner.is_empty() {
            println!("{}", banner.replace("{}", &format!("http://{}", addr)));
        }
    } else {
        println!("~ kingkong running at http://{}", addr);
    }

    for stream in listener.incoming() {
        let server = server.clone();
        let stream = stream?;
        pool.execute(move|| {
            if let Err(e) = server.handle_request(stream) {
                eprintln!("!! {}", e);
            }
        });
    }
    Ok(());
}

struct Server {
    router: Router,
}

impl Server {

}