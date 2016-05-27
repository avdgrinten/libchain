
CXXFLAGS += -std=c++14
CXXFLAGS += -Iinclude

define runtest =
@echo Running test $1; \
echo ----------------------------------------; \
$1; status=$$?; \
echo ----------------------------------------; \
if [ $$status -eq 0 ]; then echo Success!; else echo Failure! Exit status: $$status; fi; \
echo; exit $$status
endef

.PHONY: tests
tests: bin/test-await bin/test-then bin/test-contextify \
		bin/test-compose
	@echo
	$(call runtest,./bin/test-await)
	$(call runtest,./bin/test-then)
	$(call runtest,./bin/test-contextify)
	$(call runtest,./bin/test-compose)

.PHONY: clean
clean:
	rm -rf obj/ bin/

obj bin:
	mkdir $@

obj/%.o: src/%.cpp | obj
	$(CXX) -c -o $@ $(CXXFLAGS) -MD $<

bin/test-%: obj/test-%.o | bin
	$(CXX) -o $@ $<

-include obj/test-*.d

