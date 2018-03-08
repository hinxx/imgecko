#TOPDIR  := $(shell cd ..; pwd)
#include $(TOPDIR)/Rules.make

DEPENDENCIES := -lpthread
UNAME := $(shell uname)
# Assume target is Mac OS if build host is Mac OS; any other host targets Linux
ifeq ($(UNAME), Darwin)
	DEPENDENCIES += -lobjc -framework IOKit -framework CoreFoundation
else
	DEPENDENCIES += -lrt
endif

CFLAGS = -Wall -Wextra
CFLAGS += -ggdb3 -O0
CFLAGS += -Ilibftd2xx

DYNAMIC_LINK_OPTIONS := -L ./libftd2xx/build

APP = simple
STATIC_APP = $(APP)-static
DYNAMIC_APP = $(APP)-dynamic

#all: $(APP)-static $(APP)-dynamic 
all: $(APP)-static

imgecko.o: imgecko.cpp
	g++ -c $< -o $@ $(CFLAGS) $(DEPENDENCIES)

main.o: main.cpp
	g++ -c $< -o $@ $(CFLAGS) $(DEPENDENCIES)

$(STATIC_APP): imgecko.o main.o
	g++ imgecko.o main.o -o $(STATIC_APP) ./libftd2xx/build/libftd2xx.a $(CFLAGS) $(DEPENDENCIES)

#$(DYNAMIC_APP): main.c	
#	$(CC) main.c -o $(DYNAMIC_APP) $(CFLAGS) -lftd2xx $(DEPENDENCIES) $(DYNAMIC_LINK_OPTIONS)
	
clean:
	-rm -f *.o ; rm -f $(STATIC_APP); rm -f $(DYNAMIC_APP)
