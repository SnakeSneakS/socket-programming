# USAGE: make help

.DEFAULT_GOAL := help


.PHONY: clean
clean: ## clean ## make clean
	rm out/go.a 
	rm out/go.h

.PHONY: build
build: ## build ## make build
	go build -buildmode=c-archive -o out/go.a


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