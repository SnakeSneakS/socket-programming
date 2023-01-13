# USAGE: make help

.DEFAULT_GOAL := help

HOST="127.0.0.1"
PORT="8080"
TEXT="GET / HTTP/1.1\r\n\
Host: localhost:8080\r\n\r\n\
" #環境変数は改行を読み込んでくれないっぽいね

#GET http://abehiroshi.la.coocan.jp/image/abehiroshi.jpg HTTP/1.1
#Host: abehiroshi.la.coocan.jp
#Proxy-Connection: keep-alive
#User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36
#Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
#Referer: http://abehiroshi.la.coocan.jp/top.htm
#Accept-Encoding: gzip, deflate
#Accept-Language: ja,en;q=0.9
#Range: bytes=4-3103
#If-Range: "c20-4a8f71b8608c0"



.PHONY: build-server-module
build-server-module: ## build server modules ## make build-server-module
	cd go && make build && cd ../

.PHONY: build-server
build-server: ## build server ## make build-server
	mkdir -p ./out
	gcc \
	-framework CoreFoundation -framework Security \
	-o ./out/server \
	go/out/go.a http.c proxy.c socket.c handler.c server.c 

.PHONY: run-web-server
run-web-server: build-server ## run web server ## make run-web-server PORT={port}
	./out/server ${PORT} 0

.PHONY: run-proxy-server
run-proxy-server: build-server ## run proxy server ## make run-proxy-server PORT={port}
	./out/server ${PORT} 1


.PHONY: build-client
build-client: ## build client ## make build-client
	mkdir -p ./out
	gcc \
	-framework CoreFoundation -framework Security \
	-o ./out/client \
	go/out/go.a http.c socket.c client.c
	
.PHONY: run-client
run-client: build-client ## run client ## make run-client HOST=127.0.0.1 PORT=8080 TEXT="test message"
	./out/client ${HOST} ${PORT} ${TEXT}

.PHONY: clean
clean: ## clean built file ## make clean
	cd go && make clean && cd ../
	rm -f out/server && rm -f out/client

.PHONY: help
help: ## show help ## make help ## 
	@echo "--- Makefile Help ---"
	@echo ""
	@echo "Usage: "
	@echo "1. build and run web server:    make build-server-module && make build-server && make run-web-server"
	@echo "2. build and run proxy server:  make build-server-module && make build-server && make run-proxy-server"
	@echo ""
	@echo "Command list:"
	@printf "\033[10m%-30s\033[0m %-30s %s\n" "[Sub command]" "[Description]" "[Example]"
	@grep -E '^[/a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | perl -pe 's%^([/a-zA-Z_-]+):.*?(##)%$$1 $$2%' | awk -F " *?## *?" '{printf "\033[36m%-30s\033[0m %-30s %-30s\n", $$1, $$2, $$3}'