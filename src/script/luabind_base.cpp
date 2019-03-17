#include "script/luabind_base.h"
#include "platform/platform.h"

#include "logging/logging/zLogger.h"
#define  GLuaLog_WriteLog	g_logger.error

const luaL_reg lualibs[] =
{
	{"base", luaopen_base},

	{"io", luaopen_io},
#ifdef _DEBUG
	{"debug", luaopen_debug},
#endif
	{"string", luaopen_string},

	{"math", luaopen_math},
	{"table",luaopen_table},


#if (LUA_VERSION_NUM <501)
	{"loadlib", luaopen_loadlib},
#endif
	{NULL, NULL}
};

void lua_log_debug(const char* log){
	g_logger.debug("lua log: %s",log);
}
void lua_log_error(const char* log){
	g_logger.error("lua log: %s",log);
}
void lua_log_info(const char* log){
	g_logger.info("lua log: %s",log);
}
void lua_log_force(const char* log){
	g_logger.forceLog(zLogger::zINFO,"lua log: %s",log);
}
// template <class T1,class T2>
// T1 lua_mod(T1 sf,T2 num){
// 	return (sf % num);
// }

const char* lua_timetostr(int time1,const char* sformat){
	if (sformat==NULL || sformat[0]==0){
		return timetostr((time_t)time1,NULL,0,"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d");
	}else{
		return timetostr((time_t)time1);
	}
}

// const char* lua_timetostr(time_t time1,const char* sformat){
// 	if (sformat==NULL || sformat[0]==0){
// 		return timetostr(time1,NULL,0,"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d");
// 	}else{
// 		return timetostr(time1);
// 	}
// }

time_t lua_strtotime(const char * szTime,const char* sformat){
	if (sformat==NULL || sformat[0]==0){
		return strtotime_q(szTime,"%4d-%2d-%2d %2d:%2d:%2d");
	}else{
		return strtotime_q(szTime,sformat);
	}
}

int lua_nowtime(){
	return ((int)time(NULL));
}

void init_cld_lib_func(lua_State* L){
	using namespace luabind;
	module (L)
		[
			def("glog_d",&lua_log_debug),
			def("glog_e",&lua_log_error),
			def("glog_i",&lua_log_info),
			def("glog_f",&lua_log_force),


			//def("mod",(int(*)(int,int))&lua_mod),
			def("nowtime",&lua_nowtime),


			def("timetostr",&lua_timetostr),
			def("strtotime",&lua_strtotime)
		];


}

uint32_t CLuaVM::dwThreadsId=0;

CLuaVM::CLuaVM(bool bOpenStdLib)
: m_ErrFn(0)
, m_nParseStatus(-1){

	m_luaState = lua_open();
//    if(bOpenStdLib) OpenStdLib();//WARN lua open version fuck! 因为lua的版本 更新成5.15 所以原来的lua代码这里无法运行 http://lists.luaforge.net/pipermail/iup-users/2007-December/000183.html
  	if(bOpenStdLib) luaL_openlibs(m_luaState);
	luabind::open(m_luaState);
	init_cld_lib_func(m_luaState);
}

CLuaVM::~CLuaVM(){

	lua_close(m_luaState);
}

void CLuaVM::OpenStdLib(){
	assert(m_luaState);
//	extern const luaL_reg lualibs[];
	const luaL_reg *lib = lualibs;
	for (; lib->func; lib++){
		lib->func(m_luaState);
		lua_settop(m_luaState, 0);
	}
}

bool CLuaVM::Do(){
	if (m_nParseStatus == 0) {
		m_nParseStatus = lua_pcall(m_luaState, 0, LUA_MULTRET, 0);
	}
	if (m_nParseStatus != 0) {
		lua_getglobal(m_luaState, "_ALERT");
		if (lua_isfunction(m_luaState, -1)) {
			lua_insert(m_luaState, -2);
			lua_call(m_luaState, 1, 0);
		}else {
			const char* msg;
			msg = lua_tostring(m_luaState, -2);
			if (msg == NULL) msg = "(error with no message)";
			g_logger.error("CATCHED Lua EXCEPTION -> err: %s",msg);
			lua_pop(m_luaState, 2);
		}
	}
	return m_nParseStatus == 0;
}

bool CLuaVM::DoFile(const char* filename){
	if ( (m_nParseStatus = luaL_loadfile(m_luaState, filename)) == 0 ){
		return Do();
	}else{
		const char* errmsg=(const char*)lua_tostring(m_luaState, -1);
		if(errmsg==NULL){errmsg="";};
		lua_pop(m_luaState, 1);
		g_logger.error("CATCHED Lua EXCEPTION -> err: %s",errmsg);
	}
	return false;
}
bool CLuaVM::DoString(const char* buffer){
	if ( (m_nParseStatus = luaL_loadbuffer(m_luaState,buffer,strlen(buffer),"LuaWrap")) == 0 ){
		return Do();
	}else{
		const char* errmsg=(const char*)lua_tostring(m_luaState, -1);
		if(errmsg==NULL){errmsg="";};
		lua_pop(m_luaState, 1);
		g_logger.error("CATCHED Lua EXCEPTION -> err: %s",errmsg);
	}
	return false;
}
bool CLuaVM::DoBuffer(const char* buffer, size_t size){
	if ( (m_nParseStatus = luaL_loadbuffer(m_luaState,buffer,size,"LuaWrap")) == 0 ){
		return Do();
	}else{
		const char* errmsg=(const char*)lua_tostring(m_luaState, -1);
		if(errmsg==NULL){errmsg="";};
		lua_pop(m_luaState, 1);
		g_logger.error("CATCHED Lua EXCEPTION -> err: %s",errmsg);
	}
	return false;
}

bool CLuaVM::VCall_LuaStr(const char* callstr){
	if(callstr[0]=='+'){
		return DoString(&callstr[1]);
	}else{
		char* pchar_param[10];
		char szfuncname[256]={0};
		int param_count=0;
		_BUILDLUASTR_(callstr,INT32(count_of(pchar_param)),szfuncname,pchar_param,param_count);
		if (param_count>=0){
			_VCALL_BUILDLUASTR_(szfuncname,param_count,pchar_param[0],pchar_param[1],pchar_param[2],pchar_param[3],pchar_param[4],
				pchar_param[5],pchar_param[6],pchar_param[7],pchar_param[8],pchar_param[9])
		}else{
			g_logger.error("VCall_LuaStr(%s) !",callstr);
		}
		return false;
	}
}

bool CLuaVM::LoadFileToBuffer(lua_State *L, const char *filename,char* szbuffer,int &maxlen,bool& loadlocal){
	return false;
}































