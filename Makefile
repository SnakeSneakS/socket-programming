# USAGE: make help

.DEFAULT_GOAL := help

IP="127.0.0.1"
PORT=8080
TEXT="This is an test message kdlxCNKLXZCNXZLCJNJZXKCJ85NX7ZIC5J5K6K3DN3S45X3Z2CB15I5678U9D96SLKJCNWEDSIVKJLCJDSWREJ2PVIBOWJDNBSFCVUQEHIPFHY21f0jce"


.PHONY: build-server-module
build-server-module: ## build server modules ## make build-server-module
	cd go && make build && cd ../

.PHONY: build-server
build-server: ## build server ## make build-server
	mkdir -p ./out
	gcc \
	-framework CoreFoundation -framework Security \
	-o ./out/server \
	go/out/go.a http.c proxy.c handler.c server.c 

.PHONY: run-web-server
run-web-server: build-server ## run web server ## make run-web-server PORT={port}
	./out/server ${PORT} 0

.PHONY: run-proxy-server
run-proxy-server: build-server ## run proxy server ## make run-proxy-server PORT={port}
	./out/server ${PORT} 1

.PHONY: run-client
run-client: ## run client ## make run-client IP=127.0.0.1 PORT=8080 TEXT="test message"
	mkdir -p ./out
	gcc -o ./out/client client.c
	./out/client ${IP} ${PORT} ${TEXT}

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