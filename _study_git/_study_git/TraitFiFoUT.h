#pragma once
#include "TraitFiFo.h"
#include <assert.h>
#include <iostream>

static char* g_buffer = nullptr;

CTraitFiFo* factory_create(unsigned unit = 1000, unsigned fifo = 128, unsigned trait = 512)
{
	unsigned total = CTraitFiFo::Estimate(128, 1000, 512);
	delete[] g_buffer;

	char* buffer = new char[total]();

	CTraitFiFo* f = new(buffer) CTraitFiFo(total, 1000, 512);

	return f;
}

void case_birth()
{
	auto fifo = factory_create();

	assert(0 == fifo->GetSize());

	//----手工检查结果都是对的
	//fifo->m_fifo.size = 128;
	//--缓冲区划分也正确

	char c[300];

	assert(0 == fifo->Read(c, 300));
	assert(fifo->begin() == fifo->end());
	assert(fifo->rbegin() == fifo->rend());
	assert(fifo->rfrom(0) == fifo->rend());
	assert(fifo->from(0) == fifo->end());

	assert(fifo->Tamper(0, 0, 0));

	delete fifo;
}

void case_basic_rdwr()
{
	auto fifo = factory_create();
	char code[256];
	char rd[256];
	memset(rd, 0x00, 256);
	memset(code, 0x00, 256);
	//--fifo可用空间大小一共是128-
	for (int i = 0; i < 127; ++i) {
		code[i] = i % 26 + 'a';
	}

	assert(0 == fifo->Write(code, 128));//超出
	assert(127 == fifo->Write(code, 127));//正好
	assert(0 == fifo->Write(code, 1));//超出

	assert(0 == fifo->Read(rd, 126));	//空间不够=>fail
	assert(127 == fifo->Read(rd, 256));	//正常读
	assert(0 == strcmp(code, rd));		//读出来的数据和写入的一样
	assert(0 == fifo->Read(rd, 1));		//空
	assert(1 == fifo->Write(code, 1));	//=>可用写入

	delete fifo;
}

void case_fullfil_and_pour()
{
	auto fifo = factory_create();
	char code[256];
	char rd[256];
	memset(rd, 0x00, 256);
	memset(code, 0x00, 256);

	unsigned int count = 0;
	for (int i = 0; i < 200; ++i) {
		code[i] = i + 'a';

		if (0 == fifo->Write(code, (unsigned)strlen(code)))
			break;
		else
			++count;
	}

	std::cout << "write count ==> " << count << std::endl;

	std::cout << "1-------read---------------------->\n";
	while (count--) {
		memset(rd, 0x00, 256);
		int r = fifo->Read(rd, 256);
		std::cout << "r " << rd << std::endl;

		if (0 == r) break;
	}

	std::cout << "1-------finish---------------------->\n";


	assert(0 == fifo->Read(rd, 256));
	//-----------------------
	memset(code, 0x00, 256);
	for (int i = 0; i < 200; ++i) {
		code[i] = i % 26 + 'a';

		if (0 == fifo->Write(code, (unsigned)strlen(code)))
			break;
	}

	std::cout << "2-------read(begin-end)---------------------->\n";
	unsigned index = 0;
	count = 0;
	for (CTraitFiFo::Iterator itr = fifo->begin(); itr != fifo->end(); ++itr) {
		memset(rd, 0x00, 256);

		memcpy(rd, itr.block(), itr.block_size());
		std::cout << rd << std::endl;

		if (++count == 8)
			index = itr.get();
	}
	std::cout << "2-------finish---------------------->\n";

	std::cout << "3-------read(rbegin-rend)---------------------->\n";
	for (CTraitFiFo::Iterator itr = fifo->rbegin(); itr != fifo->rend(); ++itr) {
		memset(rd, 0x00, 256);

		memcpy(rd, itr.block(), itr.block_size());
		std::cout << rd << std::endl;
	}
	std::cout << "3-------finish---------------------->\n";

	std::cout << "4-------read(rbegin-rend)---------------------->\n";
	std::cout << "from :" << index << std::endl;

	for (CTraitFiFo::Iterator itr = fifo->rfrom(index); itr != fifo->rend(); ++itr) {
		memset(rd, 0x00, 256);
		memcpy(rd, itr.block(), itr.block_size());
		std::cout << rd << std::endl;
	}
	std::cout << "4-------finish---------------------->\n";

	std::cout << "5-------read(begin-end)---------------------->\n";
	std::cout << "from :" << index << std::endl;
	for (CTraitFiFo::Iterator itr = fifo->from(index); itr != fifo->end(); ++itr) {
		memset(rd, 0x00, 256);
		memcpy(rd, itr.block(), itr.block_size());
		std::cout << rd << std::endl;
	}
	std::cout << "5-------finish---------------------->\n";

	delete fifo;
}

void case_tamper()
{
	auto fifo = factory_create();
	char code[256];
	char rd[256];
	memset(rd, 0x00, 256);
	memset(code, 0x00, 256);

	//128/4=32我们先写入3个32，然后读出来以后再写入一个50
	for (int i = 0; i < 200; ++i) {
		code[i] = '~';
	}

	assert(32 == fifo->Write(code, 32));
	assert(32 == fifo->Write(code, 32));
	assert(32 == fifo->Write(code, 32));
	assert(0 == fifo->Write(code, 32));//--fail.


	CTraitFiFo::Iterator itr = fifo->begin();
	++itr;//--我们尝试修改第二个块的值
	char* value = (char*)itr.block();
	value[1] = 'x';
	fifo->Tamper(itr.get(), value, itr.block_size());

	assert(32 == fifo->Read(rd, 256));
	memset(rd, 0x00, 256);
	assert(32 == fifo->Read(rd, 256));
	std::cout << "r " << rd << std::endl;
	assert(rd[1] == 'x');//--读出我们修改的值

	//-----------这次写入50个字节--，必然回绕 32+18
	assert(50 == fifo->Write(code, 50));//---32 50
	CTraitFiFo::Iterator itr2 = fifo->rbegin();
	//这时候我们尝试修改特殊单元的值
	value = (char*)itr2.block();
	value[1] = '*';
	value[40] = '*';
	std::cout << "rx " << value << std::endl;
	fifo->Tamper(itr2.get(), value, itr2.block_size());

	//---我们把它读出来
	memset(rd, 0x00, 256);
	assert(32 == fifo->Read(rd, 256));
	memset(rd, 0x00, 256);
	assert(50 == fifo->Read(rd, 256));
	std::cout << "r " << rd << std::endl;
	assert(rd[1] == '*');
	assert(rd[40] == '*');

	//--此时缓冲区是空的，我们可以再尝试下入
	assert(127 == fifo->Write(code, 127));

	delete fifo;
}

//todo---CFifo的测试--
void case_cfifo()
{
	assert(false);
}