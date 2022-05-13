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

	//----�ֹ���������ǶԵ�
	//fifo->m_fifo.size = 128;
	//--����������Ҳ��ȷ

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
	//--fifo���ÿռ��Сһ����128-
	for (int i = 0; i < 127; ++i) {
		code[i] = i % 26 + 'a';
	}

	assert(0 == fifo->Write(code, 128));//����
	assert(127 == fifo->Write(code, 127));//����
	assert(0 == fifo->Write(code, 1));//����

	assert(0 == fifo->Read(rd, 126));	//�ռ䲻��=>fail
	assert(127 == fifo->Read(rd, 256));	//������
	assert(0 == strcmp(code, rd));		//�����������ݺ�д���һ��
	assert(0 == fifo->Read(rd, 1));		//��
	assert(1 == fifo->Write(code, 1));	//=>����д��

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

	//128/4=32������д��3��32��Ȼ��������Ժ���д��һ��50
	for (int i = 0; i < 200; ++i) {
		code[i] = '~';
	}

	assert(32 == fifo->Write(code, 32));
	assert(32 == fifo->Write(code, 32));
	assert(32 == fifo->Write(code, 32));
	assert(0 == fifo->Write(code, 32));//--fail.


	CTraitFiFo::Iterator itr = fifo->begin();
	++itr;//--���ǳ����޸ĵڶ������ֵ
	char* value = (char*)itr.block();
	value[1] = 'x';
	fifo->Tamper(itr.get(), value, itr.block_size());

	assert(32 == fifo->Read(rd, 256));
	memset(rd, 0x00, 256);
	assert(32 == fifo->Read(rd, 256));
	std::cout << "r " << rd << std::endl;
	assert(rd[1] == 'x');//--���������޸ĵ�ֵ

	//-----------���д��50���ֽ�--����Ȼ���� 32+18
	assert(50 == fifo->Write(code, 50));//---32 50
	CTraitFiFo::Iterator itr2 = fifo->rbegin();
	//��ʱ�����ǳ����޸����ⵥԪ��ֵ
	value = (char*)itr2.block();
	value[1] = '*';
	value[40] = '*';
	std::cout << "rx " << value << std::endl;
	fifo->Tamper(itr2.get(), value, itr2.block_size());

	//---���ǰ���������
	memset(rd, 0x00, 256);
	assert(32 == fifo->Read(rd, 256));
	memset(rd, 0x00, 256);
	assert(50 == fifo->Read(rd, 256));
	std::cout << "r " << rd << std::endl;
	assert(rd[1] == '*');
	assert(rd[40] == '*');

	//--��ʱ�������ǿյģ����ǿ����ٳ�������
	assert(127 == fifo->Write(code, 127));

	delete fifo;
}

//todo---CFifo�Ĳ���--
void case_cfifo()
{
	assert(false);
}