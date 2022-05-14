#pragma once
#include"NcFiFo.h"
#include <assert.h>

/*
ʵ�ֶ�CNcFiFo�ķ�װ���ṩ��ԭ��RingBufferEx�������װ�������ѵ�����ϸ�����������ݽṹ��
CNcFiFo��һ��ͨ��FiFo��������Ҳ�����ṩ�������޸Ĺ��ܣ�����Ϊ�˱���FiFo�Ĵ���ԣ�������һ�·�װ��
����CNcFiFo�����������ô���R0-R3�����ݽ�����

CTraitFiFo���ڴ沼�֣�
CTraitFiFo-��������
RingBuffer-
������
���浥Ԫ
ʹ��Estimate��������ȡ�Ƽ��ߴ�

\\note!! ������ĸ��������Ϻ�2^n���룬���Դ�������ֵ�������Ѿ�����ģ���i����ܺ�Ԥ���в��
\\note!! FiFo֧��Peek��ֱ�ӵ��޸�(Tamper����)����Ȼ������--����׼�������FiFo�����ٷ�װһ�㣬���׽��ҵ���߼��������ݽṹ������
\\note!! ����ʹ�õ��������з��ʣ����ҿ��Դ��м�λ������ͷ����(from,rfrom)

author: YaoZinan 2022-03-21
*/

#ifndef _NTDDK_
#include <Windows.h>
#endif

typedef unsigned int fifopos_t;
const fifopos_t c_nIllegalFiFoIndex = 0xFFFFFFFF;
#define IS_LEGAL_FIFOPOS(idx) (idx !=c_nIllegalFiFoIndex)

class CTraitFiFo
{
public:
	/*
	nCapcity_--CTraitFiFo���ڵ��ڴ����ܴ�С(����CtraitFifo����)
	nMaxCount_--���ɴ洢����λ�����Կɱ仺�������ԣ������������ʱ��û����ǰ֪���ܴ���ٸ���ֻ����һ������ֵ
	nTraint----�ɴ�����ĵ�λ�ߴ�---
	*/
	CTraitFiFo(unsigned int nCapcity_, unsigned int nMaxCount_, unsigned int nTraint);
	static unsigned int Estimate(unsigned int nPayloadSize_, unsigned int nMaxUnitCount, unsigned int nMaxUnitSize_);
public:
	unsigned int Write(void* pIn_, unsigned int nSize_);
	unsigned int Read(void* pOut_, unsigned int nSize_);
	unsigned int GetTraitLen() { return m_nTrait; }

	//--ʹ����---
	float Vacant();

	void Clear();
	//---get free��getsizeû������---Ҫ���½���������������
	//unsigned int GetFree() { return m_fifo.GetFree(); }
	//---Ŀǰ���˶��ٸ�
	unsigned int GetSize();

	/*!�����ڵ�����peek����ʱ���е�ʱ����Ҫ�޸ķ��ص�block��ֵ����Ϊ�����fifo�������۵��ģ����Ա����ṩҪ��������ֵˢ��ȥ Yz.*/
	bool Tamper(fifopos_t nIndex_, void* pSrc_, fifopos_t nSize_);
	bool IsEmpty();

private:
	const void* PeekBlock(fifopos_t nIndex_, unsigned int* nSize_);
	unsigned int PeekBlock(fifopos_t nIndex_);

	bool IsFull();

	void Crash(const char* pSzMsg_);
	//---���ﲻ��ÿ�ζ�ȥ�ж�---Ч��̫�ͣ�����ı��뱣֤��ȷ Yz.
	bool IsValidIndex(fifopos_t nIndex_);
	unsigned int capcity() { return m_nCount; }
private:
	unsigned char* m_pCache;	/*!fifo�۵���ʱ�򻺴�*/
	unsigned int* m_pIndices;	/*!fifo������*/
	fifopos_t m_nRdIndex;	/*!ringbuffer�Ķ�дָ��*/
	fifopos_t m_nWrIndex;
	unsigned int m_nTrait;		/*!����д�����󳤶�*/
	unsigned int m_nCount;		/*!����д�����൥Ԫ*/
	CNcFiFo m_fifo;				/*!fifo����*/

public:
	struct fifopair_t {
		void* data;
		unsigned int length;
	};

	friend class Iterator;
	//--������----
	class Iterator {
	public:
		enum type_t {
			RD2WR,
			WR2RD,
			NOITR
		};
	public:
		Iterator(type_t nType_, fifopos_t nStart_, fifopos_t nEnd_, CTraitFiFo& fifo_);
	public:
		bool operator==(const Iterator& itr_);
		bool operator!=(const Iterator& itr_);
		Iterator& operator++();
		fifopos_t get();
		fifopos_t stop_pos();

		/*!Yz ���FiFo��װ��Ϊ��һ���洢��Ԫ�Ŀ��ն��ҿ�ʼ��ʱ���֧���޸�FiFo��tamper������
		 * ���Ҫ�� *�����ĵ��ñ������ȶ��ģ�������۵������ݣ��������޸ĵ�ʱ���ٵ��� *һ�λᵼ�����ݵĸ���
		 * ����û���������ÿ�ε�����صĺ�������ȡ��ǰblock�ĳ��Ⱥ�ָ�붼�ᵼ�����¼��㣬������ϣ���������е�ֵ���ȶ���
		 * --����֮����û���ڵ����������ʱ���ȥ��ȡ�����ȶ�������Ҫ��Ч�ʵ�ԭ���е�ʱ���������Ϊ��ȡֵ
		 */
		fifopair_t operator*();
		void* block();
		unsigned int block_size();
	private:
		fifopos_t m_nIndex;
		fifopos_t m_nStopIndex;
		type_t m_nDirection;
		CTraitFiFo& m_ff;

	private:
		void Refresh();
		void value();
	private:
		bool m_bRetrieved;
		fifopair_t m_value;
	};

	Iterator from(fifopos_t nIndex_);
	Iterator rfrom(fifopos_t nIndex_);
	Iterator begin();
	Iterator end();
	Iterator rbegin();
	Iterator rend();
	Iterator rdidx();
	Iterator wridx();
private:
	unsigned char m_pPool[1];
};

inline unsigned int up_align(unsigned int n) {
	if (n == 0)
		return n;

	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	++n;
	return n;
}

inline unsigned int down_align(unsigned int n) {
	if (n <= 1) return 0;

	return up_align(n) >> 1;
}

inline bool is_power_of_2(unsigned int n)
{
	return (n == 0 || (n & (n - 1)) == 0);
}

//#define mod(n,m) (n%m)
#define mod(n,m) (n&m-1)

inline  CTraitFiFo::CTraitFiFo(unsigned int nCapcity_, unsigned int nMaxCount_, unsigned int nTraint_)
	:m_nCount(up_align(nMaxCount_))
	, m_nRdIndex(0)
	, m_nWrIndex(0)
	, m_nTrait(nTraint_)
	, m_pCache(nullptr)
	, m_pIndices(nullptr)
	, m_fifo(m_pPool, nCapcity_ - sizeof(CTraitFiFo) - nTraint_ - up_align(nMaxCount_) * sizeof(unsigned int))
{
	if (!is_power_of_2(m_nCount))
		Crash("bad count");

	m_pCache = m_pPool + m_fifo.size();
	m_pIndices = (unsigned int*)(m_pCache + nTraint_);

	memset(m_pIndices, 0x00, sizeof(unsigned int) * m_nCount);
}

inline unsigned int CTraitFiFo::Estimate(unsigned int nPayloadSize_, unsigned int nMaxUnitCount, unsigned int nMaxUnitSize_)
{
	return sizeof(CTraitFiFo) + nPayloadSize_ + sizeof(unsigned int) * up_align(nMaxUnitCount) + nMaxUnitSize_;
}

inline unsigned int CTraitFiFo::Write(void* pIn_, unsigned int nSize_)
{
	if (IsFull() || nSize_ > m_nTrait)
		return 0;

	unsigned int writer = m_fifo.writer();
	unsigned int wr = m_fifo.Push(pIn_, nSize_);
	if (0 != wr)
	{
		if (IsEmpty()) {
			m_pIndices[m_nWrIndex] = writer;
		}

		m_pIndices[mod(m_nWrIndex + 1, m_nCount)] = m_fifo.writer();
		m_nWrIndex = mod(m_nWrIndex + 1, m_nCount);
	}

	return wr;
}

inline unsigned int CTraitFiFo::Read(void* pOut_, unsigned int nSize_)
{
	if (IsEmpty())
		return 0;

	unsigned length = PeekBlock(m_nRdIndex);
	if (nSize_ < length)
		return 0;

	unsigned int rd = m_fifo.Pull(pOut_, length);

	if (length == rd)
	{
		m_nRdIndex = mod(m_nRdIndex + 1, m_nCount);
		return rd;
	}

	return 0;
}

inline void CTraitFiFo::Clear()
{
	m_nRdIndex = m_nWrIndex;
	m_fifo.Clear();
}

//----������---
inline float CTraitFiFo::Vacant()
{
	return (float)min(1 - GetSize() * 1.0 / capcity(), m_fifo.GetFree() * 1.0 / m_fifo.size());
}

inline unsigned int CTraitFiFo::GetSize()
{
	return mod(m_nWrIndex - m_nRdIndex + m_nCount, m_nCount);
}

//----�û�����޸������Ƿ��ظ�����ָ�룬��Ҫ�������������������
inline bool CTraitFiFo::Tamper(fifopos_t nWrindex_, void* pSrc_, unsigned int nSize_)
{
	//---Ҫ���������д�ص���ԭ���ĵط�
	if (pSrc_ == m_pCache)
		return nSize_ == m_fifo.Tamper(m_pIndices[nWrindex_], m_pCache, nSize_);

	return true;
}

inline bool CTraitFiFo::IsFull()
{
	return mod(m_nRdIndex - m_nWrIndex + m_nCount, m_nCount) == 1;
}

inline void CTraitFiFo::Crash(const char* pSzMsg_)
{
	return;
#ifdef _NTDDK_
	FatalError();
#else
	RaiseException(0, 0, 0, 0);
#endif
}

inline bool CTraitFiFo::IsEmpty()
{
	return m_nRdIndex == m_nWrIndex;
}

//Yz. todo.
inline bool CTraitFiFo::IsValidIndex(fifopos_t nIndex_)
{
	return false;
}

inline const void* CTraitFiFo::PeekBlock(fifopos_t nIndex_, unsigned int* nSize_)
{
	unsigned begin = m_pIndices[nIndex_];
	unsigned next = m_pIndices[mod(nIndex_ + 1, m_nCount)];

	void* block = m_pCache;
	unsigned int length = PeekBlock(nIndex_);

	if (begin < next)
		block = (char*)m_fifo.buffer() + begin;
	else
		m_fifo.Peek(begin, m_pCache, length);

	(nSize_) ? (*nSize_ = length) : (0);

	return block;
}

inline unsigned int CTraitFiFo::PeekBlock(fifopos_t nIndex_)
{
	unsigned begin = m_pIndices[nIndex_];
	unsigned next = m_pIndices[mod(nIndex_ + 1, m_nCount)];

	return begin < next ? next - begin : m_fifo.size() - begin + next;
}

inline CTraitFiFo::Iterator CTraitFiFo::from(fifopos_t nIndex_)
{
	if (IsEmpty() || nIndex_ == c_nIllegalFiFoIndex)
		return end();

	//---�Ƿ��ж�û��---todo Yz.
	return CTraitFiFo::Iterator(
		CTraitFiFo::Iterator::RD2WR,
		nIndex_,
		m_nWrIndex,
		*this);
}

inline CTraitFiFo::Iterator CTraitFiFo::rfrom(fifopos_t nIndex_)
{
	if (IsEmpty() || nIndex_ == c_nIllegalFiFoIndex)
		return rend();

	return CTraitFiFo::Iterator(
		CTraitFiFo::Iterator::WR2RD,
		nIndex_,
		m_nRdIndex,
		*this);
}

inline CTraitFiFo::Iterator CTraitFiFo::begin()
{
	if (IsEmpty())
		return end();

	return CTraitFiFo::Iterator(
		CTraitFiFo::Iterator::RD2WR,
		m_nRdIndex,
		m_nWrIndex,
		*this);
}

inline CTraitFiFo::Iterator CTraitFiFo::end()
{
	return CTraitFiFo::Iterator(
		CTraitFiFo::Iterator::RD2WR,
		c_nIllegalFiFoIndex,
		c_nIllegalFiFoIndex,
		*this);
}

inline CTraitFiFo::Iterator CTraitFiFo::rbegin()
{
	if (IsEmpty())
		return rend();

	return CTraitFiFo::Iterator(
		CTraitFiFo::Iterator::WR2RD,
		mod(m_nWrIndex - 1 + m_nCount, m_nCount),
		m_nRdIndex,
		*this);
}

inline CTraitFiFo::Iterator CTraitFiFo::rend()
{
	return CTraitFiFo::Iterator(
		CTraitFiFo::Iterator::WR2RD,
		c_nIllegalFiFoIndex,
		c_nIllegalFiFoIndex,
		*this);
}

inline CTraitFiFo::Iterator CTraitFiFo::rdidx()
{
	return CTraitFiFo::Iterator(
		CTraitFiFo::Iterator::NOITR,
		m_nRdIndex,
		m_nRdIndex,
		*this);
}

inline CTraitFiFo::Iterator CTraitFiFo::wridx()
{
	return CTraitFiFo::Iterator(
		CTraitFiFo::Iterator::NOITR,
		m_nWrIndex,
		m_nWrIndex,
		*this);
}

inline CTraitFiFo::Iterator::Iterator(type_t nType_, fifopos_t nStart_, fifopos_t nEnd_, CTraitFiFo& fifo_)
	:m_nDirection(nType_)
	, m_nIndex(nStart_)
	, m_nStopIndex(nEnd_)
	, m_ff(fifo_)
	, m_bRetrieved(false) {
	memset(&m_value, 0x00, sizeof(fifopair_t));
}

inline bool CTraitFiFo::Iterator::operator==(const Iterator& itr_)
{
	return (m_nDirection == itr_.m_nDirection && m_nIndex == itr_.m_nIndex && m_nStopIndex == itr_.m_nStopIndex);
}

inline bool CTraitFiFo::Iterator::operator!=(const Iterator& itr_)
{
	assert(m_nDirection == itr_.m_nDirection);
	return !(m_nDirection == itr_.m_nDirection && m_nIndex == itr_.m_nIndex && m_nStopIndex == itr_.m_nStopIndex);
}

inline CTraitFiFo::Iterator& CTraitFiFo::Iterator::operator++()
{
	m_bRetrieved = false;

	switch (m_nDirection)
	{
	case CTraitFiFo::Iterator::RD2WR:
		m_nIndex = mod(m_nIndex + 1, m_ff.capcity());
		if (m_nIndex == m_nStopIndex)
		{
			m_nIndex = m_nStopIndex = c_nIllegalFiFoIndex;
		}
		break;

	case CTraitFiFo::Iterator::WR2RD:
		if (m_nIndex == m_nStopIndex)
		{
			m_nIndex = m_nStopIndex = c_nIllegalFiFoIndex;
			return *this;
		}

		m_nIndex = mod(m_nIndex - 1, m_ff.capcity());

		break;

	default:
		break;
	}

	return *this;
}

inline fifopos_t CTraitFiFo::Iterator::get()
{
	return m_nIndex;
}

inline fifopos_t CTraitFiFo::Iterator::stop_pos()
{
	return m_nStopIndex;
}

inline void CTraitFiFo::Iterator::value()
{
	memset(&m_value, 0x00, sizeof(fifopair_t));

	if (m_nDirection != CTraitFiFo::Iterator::NOITR)
		m_value.data = const_cast<void*>(m_ff.PeekBlock(get(), &m_value.length));
}

inline CTraitFiFo::fifopair_t CTraitFiFo::Iterator::operator*()
{
	Refresh();
	return m_value;
}

inline void* CTraitFiFo::Iterator::block()
{
	Refresh();
	return m_value.data;
}

inline unsigned int CTraitFiFo::Iterator::block_size()
{
	Refresh();
	return m_value.length;
}

inline void CTraitFiFo::Iterator::Refresh()
{
	if (m_bRetrieved) {
		return;
	}

	m_bRetrieved = true;
	value();
}

#if 0
inline unsigned int CTraitFiFo::Iterator::peek()
{
	return m_ff.PeekBlock(m_nIndex);
}
#endif
