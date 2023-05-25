SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

CC=g++
TARGET=mp3_20200901

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ -std=c++11
$(OBJS): $(SRCS)
	$(CC) -c $^ -std=c++11
clean:
	rm -f $(OBJS) $(TARGET)
