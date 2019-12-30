target=libgdmsdk
obj=cjson.cpp \
   	clientdata.cpp \
   	cmsg.cpp \
   	common.cpp \
   	event_mgr.cpp  \
	ipcontainer.cpp  \
	Lock.cpp  \
	MD5.cpp \
   	mts_lib.cpp \
   	room.cpp \
   	socket.cpp \
   	stdafx.cpp \
   	thread.cpp

objs=$(obj:.cpp=.o)
ALL: $(objs) library
%.o : %.cpp
	g++ -Wall -g -c $< -o $@ $(CXXFLAGS)

library: 
	g++ -Wall -g $(obj) --shared -fPIC -o $(target).so -levent -lpthread -levent_pthreads
	ar crs $(target).a $(objs)
clean:
	rm -f *.a *.so *.o
