# c.cpp混合编译的makefile模板  

TARGET = jsonTest 
CROSS = #arm-linux-  
CC = $(CROSS)gcc  
CPP = $(CROSS)g++  
LD = $(CROSS)ld  
#这里只加入库头文件路径及库路径  
INCS = -I./include
LIBS = #-L"c:/mingw/lib"   
SUBDIRS =  
#生成依赖信息时的搜索目录，比如到下列目录中搜索一个依赖文件(比如.h文件)，例如 -I"./***/" -I"./base/"  
DEFINC = #-I./snap7/ -I./snap7/src/sys/ -I./snap7/src/lib/ -I./snap7/src/core/  
#给INCS加上依赖搜索路径，分开写可能会产生不一致情况，而且繁琐  
INCS += $(DEFINC)  
#  
#  #maintest.c tree/rbtree.c  多了子目录，那就直接添加 目录/*.c即可   所有的源文件--  .c文件列表  
CSRCS = $(wildcard ./*.c)  
CSRCS += ./utils/cJSON/cJSON.c
CPPSRCS = $(wildcard ./*.cpp ./utils/*.cpp)  
#  
#  #所有的.o文件列表  
COBJS := $(CSRCS:.c=.o)  
CPPOBJS := $(CPPSRCS:.cpp=.o)  
#  
#生成依赖信息 -MM是只生成自己的头文件信息，-M 包含了标准库头文件信息。  
#-MT 或 -MQ都可以改变生成的依赖  xxx.o:src/xxx.h 为 src/xxx.o:src/xxx.h 当然。前面的 src/xxx.o需自己指定  
#格式为 -MM 输入.c或.cpp  查找依赖路径  -MT或-MQ  生成规则，比如src/xxx.o  
#MAKEDEPEND = gcc -MM -MT  
CFLAGS += $(INCS)  
CFLAGS += -O2 -Wall -g -fPIC  
CPPFLAGS += $(INCS)  
CPPFLAGS += -O2 -Wall -g -fPIC  
#LDFLAGS  += -shared -fPIC -lpthread -lm -lrt -lstdc++  
LDFLAGS  += -lm -lstdc++  
	  
all:$(TARGET)  

#$(OBJS):%.o :%.c  先用$(OBJS)中的一项，比如foo.o: %.o : %.c  含义为:试着用%.o匹配foo.o。如果成功%就等于foo。如果不成功，  
# Make就会警告，然后。给foo.o添加依赖文件foo.c(用foo替换了%.c里的%)  
# # 也可以不要下面的这个生成规则，因为下面的 include $(DEF)  就隐含了。此处为了明了，易懂。故留着  
$(COBJS) : %.o: %.c  
	$(CC) $(CFLAGS) -c $< -o $@   
$(CPPOBJS) : %.o: %.cpp  
	$(CPP) $(CPPFLAGS) -c $< -o $@
	
#$@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。每次$< $@ 代表的值就是列表中的
   
$(TARGET) : $(COBJS) $(CPPOBJS)  
	$(CC) -o $(TARGET) $(COBJS) $(CPPOBJS) $(LDFLAGS) $(LIBS)   

#链接为最终目标  	  
#引入了.o文件对.c和.h的依赖情况。以后.h被修改也会重新生成，可看看.d文件内容即知道为何  
#引入了依赖就相当于引入了一系列的规则，因为依赖内容例如： 目录/xxx.o:目录/xxx.c 目录/xxx.h 也相当于隐含的引入了生成规则  
#故上面不能在出现如： $(OBJS) : $(DEF)之类。切记  

.PHONY:clean cleanall  
#清除所有目标文件以及生成的最终目标文件  

clean:              
	rm $(TARGET) $(COBJS) $(CPPOBJS)  
#rm *.d  

cleanall:  
	    rm $(TARGET) $(COBJS) $(CPPOBJS)  
