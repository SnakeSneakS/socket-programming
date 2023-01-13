# memo
- https://github.com/jelford/socks5-proxy
- https://dev.classmethod.jp/articles/socks-proxy-and-http-proxy/
- https://github.com/brechtsanders/proxysocket
- https://www.geekpage.jp/programming/linux-network/http-server.php
- https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/CONNECT
- https://developer.mozilla.org/en-US/docs/Web/HTTP/Proxy_servers_and_tunneling

## kill port process
```
PORT=8080 && lsof -i TCP:${PORT} | grep LISTEN | awk '{print $2}' | xargs kill -9
```
