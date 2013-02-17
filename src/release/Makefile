all: lincore 

CXXFLAGS += -O3
STATIC_LIBS += $(PROJECT_HOME)/lib/libcdbutils.a

%.o : ../%.cpp
	$(CXX) $(CXXFLAGS) $(INCS) $< -o $@ -MMD

lincore: $(OBJS) $(STATIC_LIBS)
	$(CXX) $(CXXLFLAGS) -o $@ $^ -L$(PROJECT_HOME)/third-party/lib $(LIBS) && cp $@ $(PROJECT_HOME)/bin/.

test: lincore
	./lincore

clean:
	rm -rf *.o
	rm -rf *.d
	rm -rf lincore

-include $(subst .cpp,.d,$(SOURCES))

