CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif
#LIBS=-I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -lpthread 	#链接库
server: main.cpp  ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp  webserver.cpp config.cpp
	$(CXX) -o server  $^ $(CXXFLAGS) -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -lpthread

clean:
	rm  -r server
