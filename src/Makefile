all: debug_bld

CXXFLAGS += -c -Wall
INCS := -I$(PROJECT_HOME)/src -I$(PROJECT_HOME)/third-party/include

SOURCES := main.cpp sigar_iface.cpp metrics_data.cpp client.cpp
OBJS := $(subst .cpp,.o,$(SOURCES))

LIBS := -lboost_filesystem -lboost_regex -lsigar -ldl
STATIC_LIBS := 

export

.PHONY: release_bld
release_bld:
	$(MAKE) -C release

.PHONY: debug_bld
debug_bld:
	$(MAKE) -C debug

pack: debug_bld
	$(PROJECT_HOME)/build/pack.sh

install: pack
	rm -rf $(PROJECT_HOME)/work/lincore
	cd $(PROJECT_HOME)/work && tar xzf $(PROJECT_HOME)/pkg/lincore-0.1.tar.gz
	$(PROJECT_HOME)/work/lincore/script/configure.sh $(PROJECT_HOME)/work/lincore

run:
	$(PROJECT_HOME)/work/lincore/script/start.sh

stop:
	$(PROJECT_HOME)/work/lincore/script/stop.sh

.PHONY: test
test:
	$(MAKE) -C debug test

PHONY: clean
clean:
	rm -rf $(PROJECT_HOME)/work/lincore
	rm -rf $(PROJECT_HOME)/bin/lincore
	rm -rf $(PROJECT_HOME)/bin-dbg/lincore
	$(MAKE) -C debug clean
	$(MAKE) -C release clean

