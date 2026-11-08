CC = cc
CFLAGS = -Wall -Wextra -Wpedantic
OBJS = message.o mmeAttach.o

message: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(OBJS:.o=.d)
