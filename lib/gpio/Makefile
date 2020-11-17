TEST_APP = gpio

CC = gcc
LDFLAGS :=

include ../build/common.include

src := $(TEST_APP).c ../common/new.c ../mm/mm.c ../list/list.c
misc_src := ../common/new.c ../mm/mm.c ../list/list.c
include = $(TEST_APP).h

include_dirs := ./  \
    ../common       \
    ../config       \
    ../list         \
    ../mm

CFLAGS += $(patsubst %,-I%,$(include_dirs))

objects := $(patsubst %.c,%.o,$(wildcard $(misc_src)))

all: $(include) $(objects) $(TEST_APP).o
	@echo $(objects)
	@echo "Building $(TEST_APP) test app"
	$(CC) $(LDFLAGS) $(objects) $(TEST_APP).o -o $(TEST_APP)
	@echo "Running $(TEST_APP) test app"
	./$(TEST_APP)

clean:
	-rm -f $(TEST_APP) $(objects) *.o

$(TEST_APP).o: $(TEST_APP).c
	$(CC) -c $(CFLAGS) -DUNIT_TEST $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

