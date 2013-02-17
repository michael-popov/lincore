all: lincore 

CXXFLAGS += -g
STATIC_LIBS += $(PROJECT_HOME)/lib-dbg/libcdbutils.a

%.o : ../%.cpp
	$(CXX) $(CXXFLAGS) $(INCS) $< -o $@ -MMD

lincore: $(OBJS) $(STATIC_LIBS)
	$(CXX) $(CXXLFLAGS) -o $@ $^ -L$(PROJECT_HOME)/third-party/lib $(LIBS) && cp $@ $(PROJECT_HOME)/bin-dbg/.

test: lincore
	./lincore --test=1 --dataspace=TOR2345 --collection=system --nets=eth0 --fs=/ --filter=*

clean:
	rm -rf *.o
	rm -rf *.d
	rm -rf core*
	rm -rf lincore

-include $(subst .cpp,.d,$(SOURCES))

