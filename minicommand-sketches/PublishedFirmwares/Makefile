DIRS := $(shell find . -type d -depth 1)
FIRMWARES = $(foreach dir,$(DIRS),$(dir)/$(dir).hex)

# $(info $(DIRS))
# $(info $(FIRMWARES))

all: $(FIRMWARES)

%.hex: $(wildcard $(`dirname \"$@\"`)/*.pde)
	make -C "`dirname \"$@\"`" -f ../../../makefiles/Sketches.mk

clean:
	for i in $(DIRS); do make -C "$$i" -f ../../../makefiles/Sketches.mk _$@; done

