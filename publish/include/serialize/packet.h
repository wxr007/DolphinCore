#ifndef _PACKET_H__
#define _PACKET_H__

#include "define/define.h"
#include <stdlib.h>
#include <map>

#pragma pack(push,1)
//#pragma warning(disable : 4200)
//#pragma warning(disable:4201)
//#pragma warning(disable:4815)

template <typename _DT,unsigned int _ARRAYINITSIZE = 0,typename _TSIZE=int>
struct	stSizeArray{
public:
	_TSIZE size;
private:
	_DT thearray[_ARRAYINITSIZE];
public:
	stSizeArray():size(0){};
	_DT& operator []  (_TSIZE  index){
		return thearray[index];
	}
	const _DT& operator[] (_TSIZE index) const {
		return thearray[index];
	}
	__inline _DT* getptr(){
		return (_DT*)(&thearray);
	}
	__inline unsigned int getarraysize(){
		return sizeof(_DT)*size;
	}
	__inline unsigned int getallsize(){
		return sizeof(_DT)*size+sizeof(size);
	}
	__inline void push_back(_DT &t){
		if (size<(int)_ARRAYINITSIZE){
			thearray[size]=t;
			size++;
		}
	}
	__inline void push_back(const char* pin,int ninlen){
		int nlen=safe_min(ninlen,_ARRAYINITSIZE-size);
		memcpy(&thearray[size],pin,nlen);
		size+=nlen;
	}
	__inline void clear(){size=0;}
	__inline void push_str(const char* str){
		int nlen=safe_min(strlen(str),_ARRAYINITSIZE-size-1);
		memcpy(&thearray[size],str,nlen);
		size+=nlen;
		*(char*)(&thearray[size])=0;
	}
};

template <typename _DT,typename _TSIZE=int>
struct	stZeroArray{
public:
	_TSIZE size;
private:
	_DT thearray[0];
public:
	stZeroArray():size(0){};
	_DT& operator []  (_TSIZE  index){
		return thearray[index];
	}
	const _DT& operator[] (_TSIZE index) const {
		return thearray[index];
	}
	__inline _DT* getptr(){
		return (_DT*)(&thearray);
	}
	__inline unsigned int getarraysize(){
		return sizeof(_DT)*size;
	}
	__inline unsigned int getallsize(){
		return sizeof(_DT)*size+sizeof(size);
	}
	__inline void push_back(_DT &t){
		thearray[size]=t;
		size++;
	}
	__inline void push_back(const char* pin,int ninlen){
		memcpy(&thearray[size],pin,ninlen);
		size+=ninlen;
	}
	__inline void clear(){size=0;}

	__inline void push_str(const char* str){
		int nlen=strlen(str);
		memcpy(&thearray[size],str,nlen);
		size+=nlen;
		*(char*)(&thearray[size])=0;
	}
};

typedef uint8_t CMD_TYPE;
typedef uint8_t SUBCMD_TYPE;
typedef unsigned short CMD_SIZE_TYPE;

#define MAKECMDVALUE(cmd,subcmd)		(CMD_SIZE_TYPE)( (subcmd<<(sizeof(CMD_TYPE)*8)) | cmd )

template <CMD_TYPE cmdvalue=0, SUBCMD_TYPE subcmdvalue=0 >
struct stCmdBase{
	enum{
		_cmd=cmdvalue,
	};
	enum{
		_subcmd=subcmdvalue,
	};
	enum{
		_value=	MAKECMDVALUE(cmdvalue,subcmdvalue),
	};
	union{
		struct{
			CMD_TYPE cmd;
			SUBCMD_TYPE subcmd;
		};
		CMD_SIZE_TYPE	value;
	};
	stCmdBase(){
		cmd=cmdvalue;
		subcmd=subcmdvalue;
	}
	__inline static CMD_SIZE_TYPE makevalue(CMD_TYPE cmd,SUBCMD_TYPE subcmd){
		return MAKECMDVALUE(cmd,subcmd);
	}
};
typedef		stCmdBase<>		stBaseCmd;

#define ZEROPCMD(p)			ZeroMemory(((char*)(p))+sizeof(stBaseCmd),sizeof(*(p))-sizeof(stBaseCmd));
#define ZEROCMD				ZEROPCMD(this)

#define  DEFINEUSERCMD(cmdtype,p1,p2)	\
struct cmdtype: public stCmdBase<p1,p2>{};	\

#define  DEFINEUSERCMD1(cmdtype,p1)	\
template <SUBCMD_TYPE subcmdvalue=0 >	\
struct cmdtype: public stCmdBase<p1,subcmdvalue>{};	\

#define DEFINEDATACMD1_0(stname,basetype)	\
	template <SUBCMD_TYPE subcmdvalue=0>		\
struct	stname:public basetype<subcmdvalue>{};	\

#define DEFINEDATACMD1_0_N		DEFINEDATACMD1_0

#define DEFINEDATACMD1_1_N(stname,dname1,basetype)	\
	template <typename _DT1,SUBCMD_TYPE subcmdvalue=0>		\
struct	stname:public basetype<subcmdvalue>		\
{		\
	\
	_DT1 dname1;		\
};
#define DEFINEDATACMD1_2_N(stname,dname1,dname2,basetype)	\
	template <typename _DT1,typename _DT2,SUBCMD_TYPE subcmdvalue=0>		\
struct	stname:public basetype<subcmdvalue>		\
{		\
	\
	_DT1 dname1;		\
	_DT2 dname2;		\
};
#define DEFINEDATACMD1_3_N(stname,dname1,dname2,dname3,basetype)	\
	template <typename _DT1,typename _DT2,typename _DT3,SUBCMD_TYPE subcmdvalue=0>		\
struct	stname:public basetype<subcmdvalue>		\
{		\
	\
	_DT1 dname1;		\
	_DT2 dname2;		\
	_DT3 dname3;		\
};
#define DEFINEDATACMD1_4_N(stname,dname1,dname2,dname3,dname4,basetype)	\
	template <typename _DT1,typename _DT2,typename _DT3,typename _DT4,SUBCMD_TYPE subcmdvalue=0>		\
struct	stname:public basetype<subcmdvalue>		\
{		\
	\
	_DT1 dname1;		\
	_DT2 dname2;		\
	_DT3 dname3;		\
	_DT4 dname4;		\
};
#define DEFINEDATACMD1_5_N(stname,dname1,dname2,dname3,dname4,dname5,basetype)	\
	template <typename _DT1,typename _DT2,typename _DT3,typename _DT4,typename _DT5,SUBCMD_TYPE subcmdvalue=0>		\
struct	stname:public basetype<subcmdvalue>		\
{		\
	\
	_DT1 dname1;		\
	_DT2 dname2;		\
	_DT3 dname3;		\
	_DT4 dname4;		\
	_DT5 dname5;		\
};



#define DEFINEDATACMD1(stname,basetype)		\
	DEFINEDATACMD1_0(##stname##Ex0,basetype)		\
	DEFINEDATACMD1_1_N(##stname##Ex1,_p1,basetype)		\
    DEFINEDATACMD1_2_N(##stname##Ex2,_p1,_p2,basetype)		\
    DEFINEDATACMD1_3_N(##stname##Ex3,_p1,_p2,_p3,basetype)		\
    DEFINEDATACMD1_4_N(##stname##Ex4,_p1,_p2,_p3,_p4,basetype)		\
    DEFINEDATACMD1_5_N(##stname##Ex5,_p1,_p2,_p3,_p4,_p5,basetype)

typedef uint32_t SIZE_TYPE;
typedef uint8_t TYPE_TYPE;
typedef uint8_t FLAG_TYPE;

const unsigned int PACKET_ZIP		=		MAKEBIT(7);
const unsigned int PACKET_SPLIT		=		MAKEBIT(6);
const unsigned int PACKET_ENC		=		MAKEBIT(5);

template < typename _CMDTYPE=stBaseCmd >
struct stPacketBase{
#define _PACK_HDR_SIZE_		(sizeof(SIZE_TYPE)+sizeof(TYPE_TYPE)+sizeof(FLAG_TYPE))

	typedef _CMDTYPE _CMD;
	enum{
		MIN_PACKET_SIZE=ROUNDNUM2(_PACK_HDR_SIZE_,8),
		MAX_PACKET_SIZE=((64-1) * 1024)-ROUNDNUM2(_PACK_HDR_SIZE_,8)-1024,
		MAX_SEND_PACKET_SIZE= 0xfffff-ROUNDNUM2(_PACK_HDR_SIZE_,8)-1024,		//1000*1024
		//重新定义数据包大小需求
		PACKET_SIZE_1K  = 1000,  
		PACKET_SIZE_5K  = 5000,  
		PACKET_SIZE_10K = 10000,  
		PACKET_SIZE_20K = 20000,  
		PACKET_SIZE_30K = 30000,  
	};

	union{
		struct{
			SIZE_TYPE	size; //未压缩的时候是 发送原始数据的大小   压缩的时候是压缩长度
			TYPE_TYPE type;   //保留位
			FLAG_TYPE flag;   //第1位是否压缩 第2位是否分割
		};
		char phd[_PACK_HDR_SIZE_];
	};
private:

	SIZE_TYPE	_cmdlen;
	//没有压缩的时候 是存放数据的首地址
	//压缩的时候是原始数据的长度


public:
	__inline SIZE_TYPE getcmdsize(){
		if (!iscompress()){
			return this->size;
		}else{
			return 	_cmdlen;
		}
	}
	__inline void setcmdsize(SIZE_TYPE rawsize){
		if (iscompress()){
			_cmdlen=rawsize;
		}else{
			this->size=rawsize;
		}
	}
	__inline _CMDTYPE* cmd(){//包的数据的首地址
		if (!iscompress()){
			return ((_CMDTYPE*)&_cmdlen);
		}else{
			return 	(_CMDTYPE*)(((uint8_t*)&_cmdlen)+sizeof(_cmdlen));
		}
	}
	__inline void init(bool iszlib,bool issplit){
		size=0;type=0;flag=0;_cmdlen=0;
		if (iszlib){ flag=(flag | PACKET_ZIP); }
		if (issplit){ flag=(flag | PACKET_SPLIT); }
	}
	__inline bool iscompress(){
		return ((flag & PACKET_ZIP)!=0);
	}
	__inline bool isencode(){
		return ((flag & PACKET_ENC)!=0);
	}
	__inline bool issplit(){
		return ((flag & PACKET_SPLIT)!=0);
	}
	__inline unsigned int getpacketsize(){
		if (!iscompress()){
			return (size + _PACK_HDR_SIZE_ );
		}else{
			return (size + _PACK_HDR_SIZE_ + sizeof(_cmdlen) );
		}
	}
	__inline bool isfullpacket(unsigned int nlen){
		return (getpacketsize()<=nlen);
	}
};

typedef		stPacketBase<>		stBasePacket;

const unsigned int MINZLIBPACKSIZE	=		(64-sizeof(stBasePacket));
#define  _MAX_PACKETBUF_SIZE_				stBasePacket::MAX_PACKET_SIZE
#define	 _MIN_PACKETBUF_SIZE_				stBasePacket::MIN_PACKET_SIZE
#define  _MAX_SEND_PACKET_SIZE_				stBasePacket::MAX_SEND_PACKET_SIZE

// struct stBigPacketInfoBase{
// 	static long static_uniquelyid;
// 	long uniquelyid;
//
// 	uint16_t maxnum;
// 	uint16_t curnum;
// 	uint32_t packetsize;
// 	uint32_t cursize;
// 	uint32_t offset;
// };

#define BUFFER_CMD(cmd,cmdname,len) char buffer##cmdname[(len)];cmd *cmdname=(cmd *)buffer##cmdname;constructInPlace(cmdname);
#define PTR_CMD(cmd,cmdname,ptr) cmd *cmdname=((cmd *)(ptr));constructInPlace(cmdname);
typedef bool (*pstc_msgfunc)(void* pobj,stBaseCmd* pcmd,unsigned int ncmdlen,void* param);
struct stCmfFuncInfo{
	pstc_msgfunc pfunc;
	uint16_t	wcmdsize;//uint16_t;
};

typedef std::map<unsigned int,stCmfFuncInfo> msgfuncmap;


#define _DEC_MAX_MSGID_						MAKECMDVALUE(0xff,0xff)

#define DEC_MSG_MAP(name)					friend struct st##name##_cld_basecmd_funcsmap_pre_init;static stCmfFuncInfo m_cld_basecmd_funcsmap[_DEC_MAX_MSGID_];/*static msgfuncmap	m_cld_basecmd_funcsmap;*/static void cld_basecmd_funcsmap_pre_init();static void cld_basecmd_funcsmap_init()
#define DEC_MSG_FUNC(objtype,cmd,paramtype)	static bool On##cmd(objtype* pobj,cmd* pcmd,unsigned int ncmdlen,paramtype param)



#define IMP_DEC_MSG_FUNC(name,objtype,cmd,paramtype)	bool name::On##cmd(objtype* pobj,cmd* pcmd,unsigned int ncmdlen,paramtype param)

#define MSG_MAP_INIT(name) stCmfFuncInfo name::m_cld_basecmd_funcsmap[_DEC_MAX_MSGID_]; 	\
_STACK_AUTOEXEC_(st##name##_cld_basecmd_funcsmap_pre_init,name::cld_basecmd_funcsmap_pre_init(),__noop);	\
st##name##_cld_basecmd_funcsmap_pre_init	name##_cld_basecmd_funcsmap_pre_init;			\
void name::cld_basecmd_funcsmap_pre_init()	{ ZeroMemory(m_cld_basecmd_funcsmap,sizeof(m_cld_basecmd_funcsmap));  cld_basecmd_funcsmap_init(); }	\
void name::cld_basecmd_funcsmap_init()	\





#define MSG2FUNC(cmd)						do{ stCmfFuncInfo* pcf=&m_cld_basecmd_funcsmap[cmd::_value];pcf->pfunc=((pstc_msgfunc)On##cmd);pcf->wcmdsize=sizeof(cmd); }while(false);
#define MSG2FUNCEX(cmd,func)				do{ stCmfFuncInfo* pcf=&m_cld_basecmd_funcsmap[cmd::_value];pcf->pfunc=((pstc_msgfunc)On##func);pcf->wcmdsize=sizeof(cmd); }while(false);
#define CMDVALUE2FUNC(cmd,subcmd,func,cmdsize)		do{ stCmfFuncInfo* pcf=&m_cld_basecmd_funcsmap[stBaseCmd::makevalue(cmd,subcmd)];pcf->pfunc=((pstc_msgfunc)func));pcf->wcmdsize=cmdsize; }while(false);


#define MSG_MAP_PROCESS(name,ret,obj,cmd,len,param)		if(obj && cmd && len>=sizeof(stBaseCmd)){	\
stCmfFuncInfo* name##pcf=&name::m_cld_basecmd_funcsmap[cmd->value];		\
pstc_msgfunc name##_msg_func=name##pcf->pfunc;	\
if( name##_msg_func && len>=name##pcf->wcmdsize ){ ret=name##_msg_func((void*)obj,(stBaseCmd*)cmd,len,(void*)param); } }


#define MSG_MAP_PROCESSEX(name,ret,findfunc,obj,cmd,len,param)		findfunc=false;if(obj && cmd && len>=sizeof(stBaseCmd)){	\
stCmfFuncInfo* name##pcf=&name::m_cld_basecmd_funcsmap[cmd->value];		\
pstc_msgfunc name##_msg_func=name##pcf->pfunc;	\
if( name##_msg_func && len>=name##pcf->wcmdsize ){ findfunc=true;ret=name##_msg_func((void*)obj,(stBaseCmd*)cmd,len,(void*)param); } }


//dy packet
#define CMD_CLIENTCOMMAND		2
DEFINEUSERCMD1(stClientCommand,CMD_CLIENTCOMMAND);



#define SUBCMD_SENDPLATTO_MANAGEALL 64
struct stSendPlatManaAll: stClientCommand<SUBCMD_SENDPLATTO_MANAGEALL>//发送 该服务的平台类型
{
	int nType;	//平台类型
	int nZoneid;//2012 12 4 当前zoneid
	stSendPlatManaAll()
	{
		ZEROCMD;
	}
};

#define SUBCMD_IP_INFO_	67
struct stIpInfo: stClientCommand<SUBCMD_IP_INFO_>//manageall 发送过来的ip信息
{
	char szip[256];
	stIpInfo()
	{
		ZEROCMD;
	}
};

#pragma pack(pop)

#endif //_PACKET_H__
