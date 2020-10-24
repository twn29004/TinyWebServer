CC=g++	#编译器
CFLAGS=-g	#可以使用gdb调试
BIN=MicroChatServer	#生成的可执行目标文件名
OBJS=connect.o	#.c/.cpp文件对应的.o目标文件
LIBS=-I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -ljsoncpp -lpthread 	#链接库

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean	#清理所有目标文件和可执行文件
clean:
	rm -f *.o $(BIN)