#pragma once
#include"NcFiFo.h"
#include <assert.h>

/*
实现对CNcFiFo的封装，提供对原先RingBufferEx的整体封装，包括把迭代器细节隐藏在数据结构中
CNcFiFo是一个通用FiFo，他本身也可以提供迭代和修改功能，但是为了保持FiFo的存粹性，这里做一下封装。
后期CNcFiFo还有其他的用处（R0-R3的数据交换）

CTraitFiFo的内存布局：
CTraitFiFo-对象自身
RingBuffer-
索引表
缓存单元
使用Estimate函数来获取推荐尺寸

\\note!! 索引表的个数会向上和2^n对齐，所以传进来的值尽量是已经对齐的，否i则可能和预期有差距
\\note!! FiFo支持Peek和直接的修改(Tamper函数)，虽然不建议--后续准备把这个FiFo外面再封装一层，彻底解决业务逻辑侵入数据结构的问题
\\note!! 可以使用迭代器进行访问，并且可以从中间位置向两头搜索(from,rfrom)

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
	nCapcity_--CTraitFiFo所在的内存块的总大小(包括CtraitFifo自身)
	nMaxCount_--最大可存储“单位”，对可变缓冲区而言，索引在外面的时候没法提前知道能存多少个，只能是一个经验值
	nTraint----可存的最大的单位尺寸---
	*/
	CTraitFiFo(unsigned int nCapcity_, unsigned int nMaxCount_, unsigned int nTraint);
	static unsigned int Estimate(unsigned int nPayloadSize_, unsigned int nMaxUnitCount, unsigned int nMaxUnitSize_);
public:
	unsigned int Write(void* pIn_, unsigned int nSize_);
	unsigned int Read(void* pOut_, unsigned int nSize_);
	unsigned int GetTraitLen() { return m_nTrait; }

	//--使用率---
	float Vacant();

	void Clear();
	//---get free和getsize没意义了---要重新解释他的商务意义
	//unsigned int GetFree() { return m_fifo.GetFree(); }
	//---目前存了多少个
	unsigned int GetSize();

	/*!我们在迭代和peek数据时候有的时候需要修改返回的block的值，因为下面的fifo可能是折叠的，所以必须提供要给函数把值刷回去 Yz.*/
	bool Tamper(fifopos_t nIndex_, void* pSrc_, fifopos_t nSize_);
	bool IsEmpty();

private:
	const void* PeekBlock(fifopos_t nIndex_, unsigned int* nSize_);
	unsigned int PeekBlock(fifopos_t nIndex_);

	bool IsFull();

	void Crash(const char* pSzMsg_);
	//---这里不想每次都去判断---效率太低，传入的必须保证正确 Yz.
	bool IsValidIndex(fifopos_t nIndex_);
	unsigned int capcity() { return m_nCount; }
private:
	unsigned char* m_pCache;	/*!fifo折叠的时候缓存*/
	unsigned int* m_pIndices;	/*!fifo索引表*/
	fifopos_t m_nRdIndex;	/*!ringbuffer的读写指针*/
	fifopos_t m_nWrIndex;
	unsigned int m_nTrait;		/*!可以写入的最大长度*/
	unsigned int m_nCount;		/*!可以写入的最多单元*/
	CNcFiFo m_fifo;				/*!fifo管理*/

public:
	struct fifopair_t {
		void* data;
		unsigned int length;
	};

	friend class Iterator;
	//--迭代器----
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

		/*!Yz 这个FiFo封装因为有一个存储单元的快照而且开始的时候会支持修改FiFo（tamper函数）
		 * 这就要求 *函数的调用必须是稳定的，否则对折叠的数据，当尝试修改的时候再调用 *一次会导致数据的覆盖
		 * 即便没有这个需求，每次调用相关的函数来获取当前block的长度和指针都会导致重新计算，所以我希望迭代器中的值是稳定的
		 * --另外之所以没有在迭代器构造的时候就去获取（更稳定），主要是效率的原因，有的时候迭代不是为了取值
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

//----空置率---
inline float CTraitFiFo::Vacant()
{
	return (float)min(1 - GetSize() * 1.0 / capcity(), m_fifo.GetFree() * 1.0 / m_fifo.size());
}

inline unsigned int CTraitFiFo::GetSize()
{
	return mod(m_nWrIndex - m_nRdIndex + m_nCount, m_nCount);
}

//----用户如果修改了我们返回给他的指针，需要调用这个函数进行修正
inline bool CTraitFiFo::Tamper(fifopos_t nWrindex_, void* pSrc_, unsigned int nSize_)
{
	//---要把这个数据写回到他原来的地方
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

	//---非法判断没做---todo Yz.
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
