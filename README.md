# Socks proxy server

# Architecture

# memo
- https://github.com/jelford/socks5-proxy
- https://dev.classmethod.jp/articles/socks-proxy-and-http-proxy/
- https://github.com/brechtsanders/proxysocket

## kill port process
```
PORT=8080 && lsof -i TCP:${PORT} | grep LISTEN | awk '{print $2}' | xargs kill -9
```