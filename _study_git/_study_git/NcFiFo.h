#pragma once
#include <string.h>
#include <assert.h>
/*
 *ͨ��FiFo��ʵ��
 *Ϊ��֧��ҵ���߼������ݵ�ֱ���޸ģ��ṩ��һ����̫�õĺ���Tamper�����ڿ���ȥ��

 * push/pull ʵ���Ƚ��ȳ�
 * Write/Read ֱ�Ӹ�FiFoָ���ĵط��򲹶�
 * 
 author:YaoZinan 2022-03-22
 */

#ifdef _NTDDK_
	#ifndef min
	#define min(a,b) (a>b?b:a)
	#endif
#else
	#include <algorithm>
	//#undef min
	//#define min (std::min)
#endif

class CNcFiFo {
public:
	/*!Yz.ͨ������һ���������ͳ�������ʼ��--����б�Ҫ���Դ���һ��allocator*/
	CNcFiFo(void* pBase_, unsigned int nSize_);
public:
	unsigned int Push(void* pIn_, unsigned int nSize_);
	unsigned int Pull(void* pOut_, unsigned int nSize_);

public:
	unsigned int Peek(unsigned int nPosition_, void* pIn_, unsigned int nSize_);
	unsigned int Tamper(unsigned int nPosition_, void* pIn_, unsigned int nSize_);

public:
	unsigned int size() { return m_nCapcity; }
	unsigned int reader() const { return m_nOut; }
	unsigned int writer() const { return m_nIn; }
	void* buffer() const { return m_pBuffer; }

public:
	bool CapcityOf(unsigned int nSze_);
	unsigned int GetFree();
	bool IsEmpty() { return m_nIn == m_nOut; }
	void Clear() { m_nOut = m_nIn; }

private:
	unsigned int Write(unsigned int nPosition_, void* pIn_, unsigned int nSize_);
	unsigned int Read(unsigned int nPosition_, void* pOut_, unsigned int nSize_);

private:
	char* m_pBuffer;
	unsigned int m_nIn;
	unsigned int m_nOut;
	unsigned int m_nCapcity;
};

inline CNcFiFo::CNcFiFo(void* pBase_, unsigned int nSize_)
	:m_pBuffer((char*)pBase_)
	, m_nCapcity(nSize_)
	, m_nIn(0)
	, m_nOut(0) {
}

inline unsigned int CNcFiFo::Write(unsigned int nPosition_, void* pIn_, unsigned int nSize_)
{
	if (nSize_ >= size())//?-->=Yz
		return 0;

	unsigned len = (std::min)(m_nCapcity - nPosition_, nSize_);
	memcpy(m_pBuffer + nPosition_, pIn_, len);
	memcpy(m_pBuffer, (char*)pIn_ + len, nSize_ - len);

	return nSize_;
}

inline unsigned int CNcFiFo::Push(void* pIn_, unsigned int nSize_)
{
	if (!CapcityOf(nSize_))
		return 0;

	unsigned wr = Write(m_nIn, pIn_, nSize_);

	if (wr == nSize_)
		m_nIn = (m_nIn + nSize_) % m_nCapcity;

	assert(wr == nSize_ || wr == 0);
	return wr;
}

inline unsigned int CNcFiFo::Pull(void* pOut_, unsigned int nSize_)
{
	unsigned rd = Read(m_nOut, pOut_, nSize_);

	if (rd == nSize_)
		m_nOut = (m_nOut + rd) % m_nCapcity;

	assert(rd == nSize_ || rd == 0);
	return rd;
}

inline unsigned int CNcFiFo::Peek(unsigned int nPosition_, void* pIn_, unsigned int nSize_)
{
	return Read(nPosition_, pIn_, nSize_);
}

inline unsigned int CNcFiFo::Tamper(unsigned int nPosition_, void* pIn_, unsigned int nSize_)
{
	return Write(nPosition_, pIn_, nSize_);
}

inline unsigned int CNcFiFo::Read(unsigned int nPosition_, void* pOut_, unsigned int nSize_)
{
	//---ʵ������û��empty�ĸ���
	//if (IsEmpty())
	//	return 0;
	//--�����̫��Ҳ�ǲ��е�---
	//---���費��Խ��
	//nSize_ = min(nSize_, size() - GetFree() - 1);
	assert(nSize_ < size() - GetFree());

	unsigned len = (std::min)(m_nCapcity - nPosition_, nSize_);
	memcpy(pOut_, m_pBuffer + nPosition_, len);
	memcpy((char*)pOut_ + len, m_pBuffer, nSize_ - len);

	return nSize_;
}

inline unsigned int CNcFiFo::GetFree()
{
	return (m_nCapcity + m_nOut - m_nIn - 1) % m_nCapcity;
}

inline bool CNcFiFo::CapcityOf(unsigned int nSze_)
{
	return GetFree() >= nSze_;
}