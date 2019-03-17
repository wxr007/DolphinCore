
#参数
#ifdef STDCXX
#	CFLAG= -c -g -std=c++11 -D_USE_CXX11_	
#else
	CFLAG= -c -g -Wno-deprecated -D_TEST_SELF_ #-D_USE_MPOOL_ #-D_USE_JEMALLOC_ #-D_USE_TCMALLOC_
	BOOSTLIB=-lboost_thread-mt
#endif

MALLOC_INC=#-I/usr/local/include
#编译器.
CXX=clang++
AR = ar
LD=$(CXX)
#环境设置.
LDFLAG=-Wl,-Bstatic -luv  -Wl,-Bdynamic -Wl,--as-needed -lpthread $(BOOSTLIB)
LIB_DIR=-L/usr/local/lib/
#WXF_DIR=
WORKLIB=./publish/libs

SVR_SRC=./src
#
MYSQL_DIR=/usr/include/mysql/
COMMON_DIR=./publish/include/
DEPS_DIR_ROOT=./publish
DEPS_DIR=$(DEPS_DIR_ROOT)/deps
UV_DIR=$(DEPS_DIR)/libuv
JSON_DIR=$(DEPS_DIR)/jsoncpp
LUA_DIR=$(DEPS_DIR)/lua
LUABIND_DIR=$(DEPS_DIR)/luabind
TINYXML_DIR=$(DEPS_DIR)/tinyxml
ZLIB_DIR=$(DEPS_DIR)/zlib
REDIS_DIR=$(DEPS_DIR)/hiredis
ICONV_DIR=$(DEPS_DIR)/iconv
INC=-I$(COMMON_DIR) -I$(UV_DIR) -I$(MYSQL_DIR) $(MALLOC_INC) -I$(JSON_DIR) -I$(LUA_DIR) -I$(LUABIND_DIR) -I$(TINYXML_DIR) -I$(ZLIB_DIR) -I$(REDIS_DIR) -I$(DEPS_DIR) -I$(ICONV_DIR)

LOCAL_LIB_DIR = -L./publish/libs/
LOCAL_LIB_LINK = -luv -liconv -ltinyxml -lz -llua -lluabind64 -ljsoncpp -lhiredis -lmysqlclient

TLIB= ./publish/libs/libuvcommon.a
#----------------------------------LIBUV SET-------------------------
OBJ_DIR=./src/.obj/uvcommon
LIB_OBJ= $(OBJ_DIR)/qthread.o $(OBJ_DIR)/tcp_client.o $(OBJ_DIR)/tcp_link.o $(OBJ_DIR)/qmemory.o $(OBJ_DIR)/uv_timer.o\
			$(OBJ_DIR)/tcp_server.o $(OBJ_DIR)/uv_ex.o $(OBJ_DIR)/tcp_manager.o \
			$(OBJ_DIR)/com_func.o $(OBJ_DIR)/initGlobal.o $(OBJ_DIR)/output.o $(OBJ_DIR)/msgpack.o $(OBJ_DIR)/stringex.o \
			$(OBJ_DIR)/winFileio.o $(OBJ_DIR)/zLogger.o \
			$(OBJ_DIR)/compress.o $(OBJ_DIR)/condSigal.o $(OBJ_DIR)/timeex.o $(OBJ_DIR)/TimeManage.o\
			$(OBJ_DIR)/convEncode.o $(OBJ_DIR)/base64.o $(OBJ_DIR)/crc32.o $(OBJ_DIR)/encdec.o $(OBJ_DIR)/des.o \
			$(OBJ_DIR)/fileter.o $(OBJ_DIR)/luabind_base.o $(OBJ_DIR)/md5.o \
			$(OBJ_DIR)/md5ex.o $(OBJ_DIR)/loopBuf.o $(OBJ_DIR)/p_linux.o \
			$(OBJ_DIR)/class.o $(OBJ_DIR)/reflect.o $(OBJ_DIR)/type.o \
			$(OBJ_DIR)/connPool.o $(OBJ_DIR)/rttiSqlHelper.o $(OBJ_DIR)/mysqlDBConnPool.o $(OBJ_DIR)/sqlHelper.o $(OBJ_DIR)/sqltool.o \
			$(OBJ_DIR)/frameallocator.o $(OBJ_DIR)/lookaside_alloc.o $(OBJ_DIR)/streamqueue.o $(OBJ_DIR)/synclist.o \
			$(OBJ_DIR)/easyStrParse.o $(OBJ_DIR)/xmlParse.o $(OBJ_DIR)/qsyslog.o \
			$(OBJ_DIR)/simpleIniManage.o $(OBJ_DIR)/convertUTF.o \
			$(OBJ_DIR)/logic_base.o $(OBJ_DIR)/logic_obj.o $(OBJ_DIR)/msg_transform.o $(OBJ_DIR)/server_status.o $(OBJ_DIR)/service.o \
			$(OBJ_DIR)/db_cache.o $(OBJ_DIR)/nosql_define.o $(OBJ_DIR)/redis_client.o \
			$(OBJ_DIR)/DataSrcBaseObj.o $(OBJ_DIR)/ModBaseObj.o $(OBJ_DIR)/ModManage.o $(OBJ_DIR)/DataSrcManage.o \
			$(OBJ_DIR)/json_config.o 

lib: createdir TLIB

all: createdir TLIB

createdir:
	test -d $(TATGETLIB) || mkdir -p $(TATGETLIB)
	test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)

clean:
	rm -f $(TLIB)
	rm -f $(OBJ_DIR)/*
#-----------------------------------Compile LIB_OBJ----------------------
#compress/	
$(OBJ_DIR)/compress.o:$(SVR_SRC)/compress/zlib/compress.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/compress/zlib/compress.cpp -o $(OBJ_DIR)/compress.o

#define
$(OBJ_DIR)/initGlobal.o:$(SVR_SRC)/define/initGlobal.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/define/initGlobal.cpp -o $(OBJ_DIR)/initGlobal.o	
	
#encrypt	
$(OBJ_DIR)/base64.o:$(SVR_SRC)/encrypt/base64/base64.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/encrypt/base64/base64.cpp -o $(OBJ_DIR)/base64.o
	
$(OBJ_DIR)/crc32.o:$(SVR_SRC)/encrypt/crc/crc32.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/encrypt/crc/crc32.cpp -o $(OBJ_DIR)/crc32.o
	
$(OBJ_DIR)/encdec.o:$(SVR_SRC)/encrypt/des/encdec.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/encrypt/des/encdec.cpp -o $(OBJ_DIR)/encdec.o
	
$(OBJ_DIR)/des.o:$(SVR_SRC)/encrypt/des/des.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/encrypt/des/des.cpp -o $(OBJ_DIR)/des.o

$(OBJ_DIR)/md5.o:$(SVR_SRC)/encrypt/md5/md5.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/encrypt/md5/md5.cpp -o $(OBJ_DIR)/md5.o

$(OBJ_DIR)/md5ex.o:$(SVR_SRC)/encrypt/md5/md5ex.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/encrypt/md5/md5ex.cpp -o $(OBJ_DIR)/md5ex.o

#logging
$(OBJ_DIR)/output.o:$(SVR_SRC)/logging/filelog/output.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/logging/filelog/output.cpp -o $(OBJ_DIR)/output.o

$(OBJ_DIR)/winFileio.o:$(SVR_SRC)/logging/filelog/winFileio.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/logging/filelog/winFileio.cpp -o $(OBJ_DIR)/winFileio.o
	
$(OBJ_DIR)/zLogger.o:$(SVR_SRC)/logging/logging/zLogger.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/logging/logging/zLogger.cpp -o $(OBJ_DIR)/zLogger.o

$(OBJ_DIR)/qsyslog.o:$(SVR_SRC)/logging/syslog/qsyslog.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/logging/syslog/qsyslog.cpp -o $(OBJ_DIR)/qsyslog.o

#memory
$(OBJ_DIR)/frameallocator.o:$(SVR_SRC)/mem/frameallocator.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/mem/frameallocator.cpp -o $(OBJ_DIR)/frameallocator.o

$(OBJ_DIR)/lookaside_alloc.o:$(SVR_SRC)/mem/lookaside_alloc.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/mem/lookaside_alloc.cpp -o $(OBJ_DIR)/lookaside_alloc.o

$(OBJ_DIR)/streamqueue.o:$(SVR_SRC)/mem/streamqueue.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/mem/streamqueue.cpp -o $(OBJ_DIR)/streamqueue.o

$(OBJ_DIR)/synclist.o:$(SVR_SRC)/mem/synclist.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/mem/synclist.cpp -o $(OBJ_DIR)/synclist.o

$(OBJ_DIR)/loopBuf.o:$(SVR_SRC)/mem/loopBuf.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/mem/loopBuf.cpp -o $(OBJ_DIR)/loopBuf.o

$(OBJ_DIR)/qmemory.o:$(SVR_SRC)/mem/qmemory.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/mem/qmemory.cpp -o $(OBJ_DIR)/qmemory.o
	
#net
$(OBJ_DIR)/tcp_client.o:$(SVR_SRC)/net/tcp/tcp_client.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/net/tcp/tcp_client.cpp -o $(OBJ_DIR)/tcp_client.o

$(OBJ_DIR)/tcp_link.o:$(SVR_SRC)/net/tcp/tcp_link.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/net/tcp/tcp_link.cpp -o $(OBJ_DIR)/tcp_link.o

$(OBJ_DIR)/tcp_server.o:$(SVR_SRC)/net/tcp/tcp_server.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/net/tcp/tcp_server.cpp -o $(OBJ_DIR)/tcp_server.o
#
$(OBJ_DIR)/uv_ex.o:$(SVR_SRC)/net/tcp/uv_ex.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/net/tcp/uv_ex.cpp -o $(OBJ_DIR)/uv_ex.o

$(OBJ_DIR)/uv_timer.o:$(SVR_SRC)/net/tcp/uv_timer.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/net/tcp/uv_timer.cpp -o $(OBJ_DIR)/uv_timer.o
	
$(OBJ_DIR)/tcp_manager.o:$(SVR_SRC)/net/tcp/tcp_manager.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/net/tcp/tcp_manager.cpp -o $(OBJ_DIR)/tcp_manager.o

#platform
$(OBJ_DIR)/com_func.o:$(SVR_SRC)/platform/com_func.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/platform/com_func.cpp -o $(OBJ_DIR)/com_func.o

$(OBJ_DIR)/p_linux.o:$(SVR_SRC)/platform/p_linux.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/platform/p_linux.cpp -o $(OBJ_DIR)/p_linux.o

#script
$(OBJ_DIR)/luabind_base.o:$(SVR_SRC)/script/luabind_base.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/script/luabind_base.cpp -o $(OBJ_DIR)/luabind_base.o

#serialize
$(OBJ_DIR)/easyStrParse.o:$(SVR_SRC)/serialize/easyStrParse.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/serialize/easyStrParse.cpp -o $(OBJ_DIR)/easyStrParse.o

$(OBJ_DIR)/xmlParse.o:$(SVR_SRC)/serialize/xmlParse.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/serialize/xmlParse.cpp -o $(OBJ_DIR)/xmlParse.o

$(OBJ_DIR)/msgpack.o:$(SVR_SRC)/serialize/msgpack.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/serialize/msgpack.cpp -o $(OBJ_DIR)/msgpack.o

#sql
$(OBJ_DIR)/connPool.o:$(SVR_SRC)/sql/connPool.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/sql/connPool.cpp -o $(OBJ_DIR)/connPool.o

$(OBJ_DIR)/rttiSqlHelper.o:$(SVR_SRC)/sql/rttiSqlHelper.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/sql/rttiSqlHelper.cpp -o $(OBJ_DIR)/rttiSqlHelper.o

$(OBJ_DIR)/mysqlDBConnPool.o:$(SVR_SRC)/sql/mysqlDBConnPool.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/sql/mysqlDBConnPool.cpp -o $(OBJ_DIR)/mysqlDBConnPool.o

$(OBJ_DIR)/sqlHelper.o:$(SVR_SRC)/sql/sqlHelper.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/sql/sqlHelper.cpp -o $(OBJ_DIR)/sqlHelper.o

$(OBJ_DIR)/sqltool.o:$(SVR_SRC)/sql/sqltool.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/sql/sqltool.cpp -o $(OBJ_DIR)/sqltool.o

#thread
$(OBJ_DIR)/condSigal.o:$(SVR_SRC)/thread/condSigal.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/thread/condSigal.cpp -o $(OBJ_DIR)/condSigal.o

$(OBJ_DIR)/qthread.o:$(SVR_SRC)/thread/qthread.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/thread/qthread.cpp -o $(OBJ_DIR)/qthread.o

#utils/
$(OBJ_DIR)/convEncode.o:$(SVR_SRC)/utils/convert_code/convEncode.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/convert_code/convEncode.cpp -o $(OBJ_DIR)/convEncode.o

$(OBJ_DIR)/fileter.o:$(SVR_SRC)/utils/filter/fileter.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/filter/fileter.cpp -o $(OBJ_DIR)/fileter.o

$(OBJ_DIR)/convertUTF.o:$(SVR_SRC)/utils/ini/convertUTF.c
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/ini/convertUTF.c -o $(OBJ_DIR)/convertUTF.o
	
$(OBJ_DIR)/simpleIniManage.o:$(SVR_SRC)/utils/ini/simpleIniManage.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/ini/simpleIniManage.cpp -o $(OBJ_DIR)/simpleIniManage.o

$(OBJ_DIR)/class.o:$(SVR_SRC)/utils/rtti/class.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/rtti/class.cpp -o $(OBJ_DIR)/class.o
	
$(OBJ_DIR)/reflect.o:$(SVR_SRC)/utils/rtti/reflect.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/rtti/reflect.cpp -o $(OBJ_DIR)/reflect.o

$(OBJ_DIR)/type.o:$(SVR_SRC)/utils/rtti/type.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/rtti/type.cpp -o $(OBJ_DIR)/type.o

$(OBJ_DIR)/stringex.o:$(SVR_SRC)/utils/string/stringex.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/string/stringex.cpp -o $(OBJ_DIR)/stringex.o

$(OBJ_DIR)/timeex.o:$(SVR_SRC)/utils/time/timeex.cpp
	$(CXX) $(CFLAG) $(INC) $(SVR_SRC)/utils/time/timeex.cpp -o $(OBJ_DIR)/timeex.o
	
$(OBJ_DIR)/TimeManage.o : $(SVR_SRC)/utils/time/TimeManage.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/utils/time/TimeManage.cpp -o $(OBJ_DIR)/TimeManage.o

#service/	
$(OBJ_DIR)/logic_base.o : $(SVR_SRC)/service/logic_base.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/service/logic_base.cpp -o $(OBJ_DIR)/logic_base.o

$(OBJ_DIR)/logic_obj.o : $(SVR_SRC)/service/logic_obj.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/service/logic_obj.cpp -o $(OBJ_DIR)/logic_obj.o
	
$(OBJ_DIR)/msg_transform.o : $(SVR_SRC)/service/msg_transform.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/service/msg_transform.cpp -o $(OBJ_DIR)/msg_transform.o

$(OBJ_DIR)/server_status.o : $(SVR_SRC)/service/server_status.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/service/server_status.cpp -o $(OBJ_DIR)/server_status.o
	
$(OBJ_DIR)/service.o : $(SVR_SRC)/service/service.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/service/service.cpp -o $(OBJ_DIR)/service.o

#db_cache
$(OBJ_DIR)/db_cache.o : $(SVR_SRC)/dbcache/db_cache.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/dbcache/db_cache.cpp -o $(OBJ_DIR)/db_cache.o
	
$(OBJ_DIR)/nosql_define.o : $(SVR_SRC)/dbcache/nosql_define.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/dbcache/nosql_define.cpp -o $(OBJ_DIR)/nosql_define.o
	
$(OBJ_DIR)/redis_client.o : $(SVR_SRC)/dbcache/redis_client.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/dbcache/redis_client.cpp -o $(OBJ_DIR)/redis_client.o
	
$(OBJ_DIR)/json_config.o : $(SVR_SRC)/serialize/json_config.cpp
	$(CXX) $(CFLAG) $(INC) -c $(SVR_SRC)/serialize/json_config.cpp -o $(OBJ_DIR)/json_config.o
		
#-----------------------------------------------------------------------------------	
TLIB:$(LIB_OBJ)
	$(AR) rcs $(TLIB) $(LIB_OBJ) 















