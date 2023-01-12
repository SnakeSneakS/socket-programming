# USAGE: make help

.DEFAULT_GOAL := help

IP="127.0.0.1"
PORT=8080
TEXT="This is an test message kdlxCNKLXZCNXZLCJNJZXKCJ85NX7ZIC5J5K6K3DN3S45X3Z2CB15I5678U9D96SLKJCNWEDSIVKJLCJDSWREJ2PVIBOWJDNBSFCVUQEHIPFHY21f0jce"

.PHONY: build-server
build-server: ## build server ## make build-server
	cd go && make build && cd ../

.PHONY: run-server
run-server: ## run server ## make run-server
	mkdir -p ./out
	gcc \
	-framework CoreFoundation -framework Security \
	-o ./out/server \
	go/out/go.a http.c proxy.c handler.c server.c 
	./out/server ${PORT}

.PHONY: run-client
run-client: ## run client ## make run-client IP=127.0.0.1 PORT=8080 TEXT="test message"
	mkdir -p ./out
	gcc -o ./out/client client.c
	./out/client ${IP} ${PORT} ${TEXT}


# https://ktrysmt.github.io/blog/write-useful-help-command-by-shell/
.PHONY: help
help: ## show help ## make help ## a
	@echo "--- Makefile Help ---"
	@echo ""
	@echo "Usage: make SUB_COMMAND argument_name=argument_value"
	@echo ""
	@echo "Command list:"
	@printf "\033[36m%-30s\033[0m %-80s %s\n" "[Sub command]" "[Description]" "[Example]"
	@grep -E '^[/a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | perl -pe 's%^([/a-zA-Z_-]+):.*?(##)%$$1 $$2%' | awk -F " *?## *?" '{printf "\033[36m%-30s\033[0m %-80s %-30s\n", $$1, $$2, $$3}'