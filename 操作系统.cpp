#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "define.h"
#include "string.h"


#include<iostream>
using namespace std;
PageRecord* LRU(SegmentRecord*& s)
{
	int max = -1;
	SegmentRecord* index = NULL;
	for (int i = 0; i < totalProcessCnt; i++)
	{
		for (int j = 0; j < Processes[i]->segment->size; j++)
		{
			if (nowTime - Processes[i]->segment->table[j].recently > max)
			{
				max = nowTime - Processes[i]->segment->table[j].recently;
				index = &Processes[i]->segment->table[j];
			}
		}
	}
	max = 0;
	PageRecord* res = NULL;
	for (int i = 0; i < index->pageCnt; i++)
	{
		if (nowTime - index->pageAddress[i].recently > max)
		{
			max = nowTime - index->pageAddress[i].recently;
			res = &index->pageAddress[i];
		}
	}
	s = index;
	return res;
}
void apply(PCB* p)
{
	for (int i = 0; i < p->segment->size; i++)
	{
		//遍历所有段
		SegmentRecord* s = &(p->segment->table[i]);
		s->pageAddress = (PageRecord*)malloc(sizeof(PageRecord) * s->pageCnt);//申请足够大的页表空间
		//free(s->pageAddress);
		if (remainBlock > 0) 
		{
			s->inMemory = true;
			for (int j = 0; j < s->pageCnt; j++)
			{
				if (remainBlock > 0)
				{
					for (int k = 0; k < BLOCKCNT; k++)
					{
						if (MemoryBlock[k] == 0)
						{
							//如果该内存块空闲则立刻分配给程序
							s->pageAddress[j].id = j;
							s->pageAddress[j].blockID = k;
							s->pageAddress[j].inMemory = true;
							s->pageAddress[j].modify = false;
							s->pageAddress[j].recently = 0;
							s->pageAddress[j].DiskID = -1;
							MemoryBlock[k] = 1;
							remainBlock--;//可用内存块-1
							break;
						}
					}
				}
				else
				{
					//无可用内存块则将页面分配到外存中
					for (int k = 0; k < BLOCKCNT; k++)
					{
						if (DiskBlock[k] == 0)
						{
							
							s->pageAddress[j].id = j;
							s->pageAddress[j].blockID = -1;
							s->pageAddress[j].inMemory = false;
							s->pageAddress[j].modify = false;
							s->pageAddress[j].recently = 0;
							s->pageAddress[j].DiskID = k;
							DiskBlock[k] = 1;
							remaindDisk--;
							break;
						}
					}

				}

			}
		}
		else
		{
			s->inMemory = false;
			for (int j = 0; j < s->pageCnt; j++)
			{
				for (int k = 0; k < BLOCKCNT; k++)
				{
					if (DiskBlock[k] == 0)
					{
					
						s->pageAddress[j].id = j;
						s->pageAddress[j].blockID = -1;
						s->pageAddress[j].inMemory = false;
						s->pageAddress[j].modify = false;
						s->pageAddress[j].recently = 0;
						s->pageAddress[j].DiskID = k;
						DiskBlock[k] = 1;
						remaindDisk--;
						break;
					}
				}
			}
		}

	}
	cout << "申请完毕" << endl;
}

void destory(PCB* p)
{

	for (int i = 0; i < p->segment->size; i++)
	{
		PageRecord* pages = p->segment->table[i].pageAddress;
		for (int j = 0; j < p->segment->table[i].pageCnt; j++)
		{
			if (pages[j].inMemory)
			{
				MemoryBlock[pages[j].blockID] = 0;
			}
			else
			{
				DiskBlock[pages[j].DiskID] = 0;
			}
		}

		free(p->segment->table[i].pageAddress);
	}
	cout << "销毁完成" << endl;
}

void Lack_Seg_Interrupt(SegmentRecord* s, PageRecord* pr)
{
	cout << "触发了缺段" << endl;
	if (remainBlock > 0) {
		//如果存在空闲块
		for (int i = 0; i < BLOCKCNT; i++)
		{
			if (MemoryBlock[i] == 0)
			{
				//如果该内存块没有被使用，则使用他
				s->inMemory = true;
				pr->blockID = i;
				pr->inMemory = true;
				pr->modify = false;
				remainBlock--;
				break;
			}
		}
	}

	else {
		SegmentRecord* replaceSeg;//报存被替换出来的页面所属于的段，用于后面判断这个段的页面被替换出去后此段是否还存在页面在内存中
		PageRecord* replacePage = LRU(replaceSeg);//通过LRU算法找出在要替换的页
		s->inMemory = true;
		replacePage->inMemory = false;
		if (replacePage->modify == true)
		{
			//如果该内存块被修改,则需要重新写回磁盘
			replacePage->modify = false;
			//copy data
		}
		//否则直接丢弃该页面即可
		DiskBlock[replacePage->DiskID] = 1;
		pr->blockID = replacePage->blockID;
		pr->inMemory = true;
		bool inMemory = false;
		for (int i = 0; i < s->pageCnt; i++)
		{
			if (replaceSeg->pageAddress[i].inMemory == true)
			{
				inMemory = true;
				break;
			}
		}
		if (inMemory == false)
		{
			replaceSeg->inMemory = false;//如果该段没有页面在内存中，则修改该段不在内存中
		}
	}
}

//缺页中断处理
void Lack_Page_Interrupt(SegmentRecord* s, PageRecord* pr) {
	cout << "触发了缺页" << endl;
	//如果有空闲块
	if (remainBlock > 0) {

		//遍历内存块
		for (int i = 0; i < BLOCKCNT; i++) {
			if (MemoryBlock[i] == 0) {//找到空闲内存块
				MemoryBlock[i] = 1;
				pr->inMemory = true;
				pr->modify = false;
				pr->blockID = i;
				remainBlock--;
				return;
			}
		}
	}
	else {//如果没有空闲块
		SegmentRecord* replaceSeg;//报存被替换出来的页面所属于的段，用于后面判断这个段的页面被替换出去后此段是否还存在页面在内存中
		PageRecord* replacePage = LRU(replaceSeg);//通过LRU算法找出在要替换的页
		replacePage->inMemory = false;
		if (replacePage->modify == true)
		{
			//如果该内存块被修改,则需要重新写回磁盘
			replacePage->modify=false;		
			//copy data

		}
		//否则直接丢弃该页面即可
		DiskBlock[replacePage->DiskID] = 1;
		pr->blockID = replacePage->blockID;
		pr->inMemory = true;
		bool inMemory = false;
		for (int i = 0; i < s->pageCnt; i++)
		{
			if (s->pageAddress[i].inMemory == true)
			{
				inMemory = true;
				break;
			}
		}
		if (inMemory == false)
		{
			s->inMemory = false;//如果该段没有页面在内存中，则修改该段不在内存中
		}
	}

}



//逻辑地址转换成物理地址,return:0 转换正常; 1 段越界 ; 2 页越界
int addressConvert(PCB* process, int segId, int pageId, int offset, int& paddr) {/*进程，虚拟地址(段号，页号，偏移)，物理地址*/
	nowTime++;
	SegmentTable* segTable = process->segment;   //获取段表
	//根据段表找到相应的页框
	if (segId < 0 || segId >= segTable->size) {//判断段号是否越界
		return 1;
	}
	else {
		SegmentRecord* segment = &segTable->table[segId];
		if (pageId < 0 || pageId >= segment->pageCnt) {//判断页号是否越界
			return 2;
		}
		if (segment->inMemory == false)
		{
			//缺段
			Lack_Seg_Interrupt(segment, &segment->pageAddress[pageId]);
		}
		if (segment->pageAddress[pageId].inMemory == false)
		{
			//缺页了
			Lack_Page_Interrupt(segment, &segment->pageAddress[pageId]);
		}
		segment->recently= nowTime;
		segment->pageAddress[pageId].recently= nowTime;
		
		PageRecord* pr = &(segment->pageAddress[pageId]);//获取页表项
		paddr = PAGESIZE * (pr->blockID) + offset;  //根据映射的物理块号和偏移量获取物理地址
		return 0;
	}
}

void shouPCBMemory(PCB* process)
{
	cout << "进程信息" << endl;
	cout << process->name << endl;
	for (int i = 0; i < process->segment->size; i++)
	{
		cout << "段" << i << endl;
		for (int j = 0; j < process->segment->table[i].pageCnt; j++)
		{
			if (process->segment->table[i].pageAddress[j].inMemory==true)
			{
				cout << process->segment->table[i].pageAddress[j].blockID << "  ";
			}
			else
			{
				cout << process->segment->table[i].pageAddress[j].DiskID << "(外存)" << "  ";
			}
		}
		cout << endl;
	}
}


bool readBlock(int diskID, int blockID)
{
	if (fseek(PageFile, diskID * PAGESIZE, 0) == 0)
	{
		int size=fread(&Memories[blockID * PAGESIZE], PAGESIZE, 1, PageFile);
		if (size == 1)
		{
			return true;
		}
		return false;
	}
	return false;
}

bool saveBlock(int diskID, int blockID)
{
	if (fseek(PageFile, diskID * PAGESIZE, 0) == 0)
	{
		int size = fwrite(&Memories[blockID * PAGESIZE], PAGESIZE, 1, PageFile);
		if (size == 1)
		{
			return true;
		}
		return false;
	}
	return false;
}


int main()
{
	PageFile = fopen("PageFile","w+");
	char buffer[DISKBOCKCNT * PAGESIZE] = { 0 };
	fwrite(buffer, 1, DISKBOCKCNT * PAGESIZE, PageFile);
	fclose(PageFile);
	memset(MemoryBlock, 0, BLOCKCNT);
	memset(DiskBlock, 0, DISKBOCKCNT);
//	memset(MemoryBlock,1, BLOCKCNT-10);
	//remainBlock = 10;
	PCB* process1 = (PCB *)malloc(sizeof(PCB));
	PCB* process2 = (PCB*)malloc(sizeof(PCB));
	process1->id = 0;
	strcpy(process1->name, "进程1");
	SegmentTable segTable1;
	segTable1.size = 1;
	segTable1.table = (SegmentRecord*)malloc(sizeof(SegmentRecord));
	segTable1.table[0].id = 0;
	segTable1.table[0].pageCnt = BLOCKCNT - 10;
	process1->segment = &segTable1;
	SegmentTable segTable2;
	segTable2.size = 2;
	segTable2.table = (SegmentRecord*)malloc(sizeof(SegmentRecord)*2);
	segTable2.table[0].id = 0;
	segTable2.table[0].pageCnt =15;
	segTable2.table[1].id = 1;
	segTable2.table[1].pageCnt = 15;
	process2->id = 1;
	process2->segment = &segTable2;
	strcpy(process2->name, "进程2");
	totalProcessCnt = 2;
	apply(process1);
	apply(process2);
	Processes[0] = process1;
	Processes[1] = process2;
	shouPCBMemory(process2);
	int address;
	addressConvert(process2, 1, 11, 11, address);
	shouPCBMemory(process2);
	cout << address << endl;;
	destory(process1);
}