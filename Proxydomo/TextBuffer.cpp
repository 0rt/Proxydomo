/**
*	@file	TextBuffer.cpp
*	@brief	�f�[�^���󂯎���ăe�L�X�g�t�B���^�[�ŏ�������RequestManager�ɕԂ��N���X
*/

#include "stdafx.h"
#include "TextBuffer.h"
#include "FilterOwner.h"

CTextBuffer::CTextBuffer(CFilterOwner& owner, IDataReceptor* output) : m_owner(owner), m_output(output)
{
}


CTextBuffer::~CTextBuffer(void)
{
}


// IDataReceptor

void CTextBuffer::DataReset()
{
}

void CTextBuffer::DataFeed(const std::string& data)
{
}

void CTextBuffer::DataDump()
{
}