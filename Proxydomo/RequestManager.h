/**
*	@file	RequestManager.h
*	@brief	�u���E�U�̃v���N�V�̃T�[�o�[�Ԃ̏������󂯎���
*/

#pragma once

#include <string>
#include <vector>
#include "Socket.h"
#include "DataReceptor.h"
#include "TextBuffer.h"
#include "FilterOwner.h"
#include "proximodo\url.h"
#include "proximodo\zlibbuffer.h"

class CRequestManager : public IDataReceptor
{
public:
	CRequestManager(std::unique_ptr<CSocket>&& psockBrowser);
	~CRequestManager();

	void	Manage();

	bool	IsValid() const { return m_valid; }
	void	SwitchToInvalid() { m_valid = false; }
	bool	IsDoInvalidPossible();

private:

	/// �u���E�U �� Proxy(this) �� �T�C�g
	bool	_ReceiveOut();
	std::string	m_recvOutBuf;
	void	_ProcessOut();
	std::string m_sendOutBuf;	/// �T�C�g�ɑ���f�[�^
	bool	_SendOut();			/// Send outgoing data to website

	void	_ConnectWebsite();

	/// �T�C�g �� Proxy(this) �� �u���E�U
	bool	_ReceiveIn();
	std::string m_recvInBuf;

	void	_ProcessIn();
	bool	_SendIn();
	std::string m_sendInBuf;


	bool	_VerifyContentType(std::string& ctype);

	// IDataReceptor
	void	DataReset() { m_dumped = false; }
	void	DataFeed(const std::string& data);
	void	DataDump();

	void	_EndFeeding();

	void	_FakeResponse(const std::string& code);
	
	// Constants
	enum { kReadBuffSize = 2048 };

    // Processing steps (incoming or outgoing)
    enum STEP { 
		STEP_START,     // marks the beginning of a request/response
        STEP_FIRSTLINE, // read GET/POST line, or status line
        STEP_HEADERS,   // read headers
        STEP_DECODE,    // check and filter headers
        STEP_CHUNK,     // read chunk size / trailers
        STEP_RAW,       // read raw message body
        STEP_TUNNELING, // read data until disconnection
        STEP_FINISH,	// marks the end of a request/response
	};

	// Data members

	// Filter instances
	CTextBuffer	m_textFilterChain;

	// Sockets
	std::unique_ptr<CSocket>	m_psockBrowser;
	std::unique_ptr<CSocket>	m_psockWebsite;
	std::string	m_previousHost;

	bool	m_valid;
	bool	m_dumped;

	IPv4Address	m_ipFromAddress;
	// for recv events only
	std::string m_logRequest;
	std::string m_logResponse;
	std::string	m_logPostData;

	// for correct incoming body processing
    bool	m_recvConnectionClose; // should the connection be closed when finished
    bool	m_sendConnectionClose;
    int		m_recvContentCoding;   // 0: plain/compress, 1: gzip, 2:deflate (zlib)
    int		m_sendContentCoding;
	std::unique_ptr<CZlibBuffer>	m_decompressor;
	std::unique_ptr<CZlibBuffer>	m_compressor;

    // Variables and functions for outgoing processing
    STEP	m_outStep;	/// �u���E�U �� Proxy(this) �� �T�C�g �Ԃ̏����̏�Ԃ�����
	int		m_outSize;
	bool	m_outChunked;
	struct { 
		std::string method, url, ver; 
	} m_requestLine;

	// Variables and functions for incoming processing
	STEP	m_inStep;	/// �T�C�g �� Proxy(this) �� �u���E�U �Ԃ̏����̏�Ԃ�����
	int		m_inSize;
	bool	m_inChunked;
	struct { 
		std::string ver, code, msg;
	} m_responseLine;

	CFilterOwner	m_filterOwner;

};

