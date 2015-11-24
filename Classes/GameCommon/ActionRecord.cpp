#include "ActionRecord.h"


SINGLETON_DEFINE( userAction );

char* RecordingMsgId[RECODING_MSG_NUM] = 
{
	(char*)"1001",						// 飞机升级		
	(char*)"1002",						// 飞机解锁		
	(char*)"1003",						// 飞机出战次数	
	(char*)"1004",						// 无尽关进入次数
	(char*)"1005",						// 普通关进度
};


userAction::userAction()
{
#ifdef WIN32
	setDevicesId("54351551");
#endif
}

userAction::~userAction()
{
}

void userAction::setRecordHeadMsg(RecordingMsgType type )																							
{				
	// 地址 + 设备ID^渠道号^版本号^行为ID^data1^data2^data3^data4^data5
	_Recordingdata.clear();																															
	_Recordingdata.append( ActionRecord_Url );								
	_Recordingdata.append( getDevicesId().c_str() );
	_Recordingdata.append( "^" );	
	_Recordingdata.append( Define_Channel );
	_Recordingdata.append( "^" );
	_Recordingdata.append( Define_Version );
	_Recordingdata.append("^");
	_Recordingdata.append( RecordingMsgId[type] );
	_Recordingdata.append( "^" );
}

void userAction::setRecordMsgAppend( const char* string )
{
	_Recordingdata.append( "^" );
	if(string)
		_Recordingdata.append( string );
}

void userAction::sendRecordingMsg( RecordingMsgType type, ccHttpRequestCallback	pSelector, const char* string1/*=NULL*/,const char *string2/*=NULL*/, const char *string3/*=NULL*/,const char *string4/*=NULL*/,const char *string5/*=NULL*/ )
{
	if(type <0 || type>=RECODING_MSG_NUM)
		return ;

	setRecordHeadMsg(type);
	setRecordMsgAppend(string1);
	setRecordMsgAppend(string2);
	setRecordMsgAppend(string3);
	setRecordMsgAppend(string4);
	setRecordMsgAppend(string5);
	
	CCLOG("send message data[%s]", _Recordingdata.c_str() );
	sendMsg( _Recordingdata.c_str() , pSelector );
}

void userAction::sendMsg(const char *url, ccHttpRequestCallback	pSelector )
{
	HttpRequest* request = new (std::nothrow) HttpRequest();
	request->setUrl( url );
	request->setRequestType( HttpRequest::Type::GET );
	request->setResponseCallback( pSelector);
// 	HttpClient::getInstance()->setTimeoutForConnect(30);
// 	HttpClient::getInstance()->setTimeoutForRead(30);
	HttpClient::getInstance()->sendImmediate(request);
	HttpClient::getInstance()->send(request);
	request->release();
}

