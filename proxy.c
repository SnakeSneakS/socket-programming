/*proxyサーバの仕様(https):
ref: https://milestone-of-se.nesuke.com/nw-basic/grasp-nw/proxy/

# http
GET: /path
はプロキシサーバには
GET: /http://example.host.com/path
として届く

# https
1. Client->Proxy
2. Proxy->Client: 「HTTP/1.1 200 Connection Established」
3. Client <-> Proxy <-> Server で通信
或いはプロキシサーバ自身で動的に証明書を生成することで通信を除くことができる
*/
