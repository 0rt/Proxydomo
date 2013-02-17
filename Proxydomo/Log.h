/**
*	@file	Log.h
*	@brief	���O�N���X
*/

#pragma once

#include <string>
#include "Socket.h"

enum LogProxyEvent
{
	kLogProxyNewRequest,	// �u���E�U�\�P�b�g���J����܂���
	kLogProxyEndRequest,	// �u���E�U�\�P�b�g�������܂���
};

enum LogHttpEvent
{
	kLogHttpRecvOut,	// Browser����f�[�^����M
	kLogHttpSendOut,	// Website�փf�[�^�𑗐M
	kLogHttpPostOut,
	kLogHttpRecvIn,		// Website����f�[�^����M
	kLogHttpSendIn,		// Browser�փf�[�^�𑗐M

};


class ILogTrace
{
public:
	virtual void ProxyEvent(LogProxyEvent Event, const IPv4Address& addr) = 0;
	virtual void HttpEvent(LogHttpEvent Event, int RequestNumber, const std::string& text) = 0;
};




class CLog
{
public:
	static void	RegisterLogTrace(ILogTrace* pTrace) { s_pLogTrace = pTrace; }
	static void	RemoveLogTrace() { s_pLogTrace = nullptr; }

	static void ProxyEvent(LogProxyEvent Event, const IPv4Address& addr)
	{
		if (s_pLogTrace)
			s_pLogTrace->ProxyEvent(Event, addr);
	}
	static void HttpEvent(LogHttpEvent Event, int RequestNumber, const std::string& text)
	{
		if (s_pLogTrace)
			s_pLogTrace->HttpEvent(Event, RequestNumber, text);
	}

	static long	IncrementRequestCount() { return ::InterlockedIncrement(&s_RequestCount); }

	static long GetActiveRequestCount() { return s_ActiveRequestCount; }
	static long	IncrementActiveRequestCount() { return ::InterlockedIncrement(&s_ActiveRequestCount); }
	static long	DecrementActiveRequestCount() { return ::InterlockedDecrement(&s_ActiveRequestCount); }


private:
	// 
	static ILogTrace*	s_pLogTrace;
	static long			s_RequestCount;			/// Total number of requests received since Proximodo started
	static long			s_ActiveRequestCount;	/// Number of requests being processed
};


__declspec(selectany) ILogTrace*	CLog::s_pLogTrace = nullptr;
__declspec(selectany) long			CLog::s_RequestCount = 0;
__declspec(selectany) long			CLog::s_ActiveRequestCount = 0;
















