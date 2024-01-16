VPATH    =
LIB_DIR  = 
INCLUDES =
CFLAGS   = -Wall -g
LIBS     = -lpthread

OBJS    =

TARGET  = tl_tcp_client tl_tcp_server tl_udp_client tl_udp_server

all:$(TARGET)

tl_tcp_client:tl_tcp_client.o $(OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

tl_tcp_server:tl_tcp_server.o $(OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

tl_udp_client:tl_udp_client.o $(OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

tl_udp_server:tl_udp_server.o $(OBJS)
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

%.o:%.c
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

%.d:%.c
	@set -e; $(CC) -MM $< $(INCLUDES) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o:%.cpp
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

%.d:%.cpp
	@set -e; $(CC) -MM $< $(INCLUDES) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
			
sinclude $(OBJS:.o=.d)

.PHONY:
clean:
	rm -f $(TARGET) *.o *.d *.d.* 
install:$(TARGET)
	cp $(TARGET) $(INSTALL_DIR)
