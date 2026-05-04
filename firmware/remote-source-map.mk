ifneq (,$(filter template,$(MAKECMDGOALS)))
REMOTE_URL?=$(shell (git remote get-url upstream 2>/dev/null || git remote get-url origin 2>/dev/null) | sed 's/\.git$$//')
REMOTE_REF?=$(shell git rev-parse HEAD)
PREFIX_MAP_FLAG=-ffile-prefix-map=$(abspath $(ROOT))=$(REMOTE_URL)/blob/$(REMOTE_REF)
ASMFLAGS+=$(PREFIX_MAP_FLAG)
CFLAGS+=$(PREFIX_MAP_FLAG)
CXXFLAGS+=$(PREFIX_MAP_FLAG)
LDFLAGS+=$(PREFIX_MAP_FLAG)

ifneq ($(wildcard .d),)
  $(error Error: `make template` must be run on a clean slate (Run `make clean` first))
endif
endif
