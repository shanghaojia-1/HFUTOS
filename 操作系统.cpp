#define  _CRT_SECURE_NO_WARNINGS
#include "define.h"
#include "string.h"
#include<iostream>
using namespace std;
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
						if (MemmoryBlock[k] == 0)
						{
							//如果该内存块空闲则立刻分配给程序
							s->pageAddress[j].id = j;
							s->pageAddress[j].blockID = k;
							s->pageAddress[j].inMemory = true;
							s->pageAddress[j].modify = false;
							s->pageAddress[j].recently = 0;
							s->pageAddress[j].DiskID = -1;
							MemmoryBlock[k] = 1;
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
						break;
					}
				}
			}
		}

	}
	cout << "申请完毕" << endl;
}

void segmentGrowUP(PCB* process,int segmentID,int newsize)
{

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
				MemmoryBlock[pages[j].blockID] = 0;
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

void Lack_Seg_Interrupt(PCB* pcb) {
	if (remainBlock > 0) {


	}
}

//缺页中断处理
void Lack_Page_Interrupt(SegmentRecord* s, PageRecord* pr) {
	//如果有空闲块
	if (remainBlock > 0) {

		//遍历内存块
		for (int i = 0; i < BLOCKCNT; i++) {
			if (MemmoryBlock[i] == 0) {//找到空闲内存块
				MemmoryBlock[i] = 1;
				pr->inMemory = true;
				pr->modify = false;
				pr->blockID = i;
				pr->recently = 0;
				return;
			}
		}
	}
	else {//如果没有空闲块
		int replaceIndex = LRU(s);//获取将要替换的页号
		if (s->pageAddress[replaceIndex].modify)
		{
			//如果该页被修改过
			s->pageAddress[replaceIndex].modify = false;
			s->pageAddress[replaceIndex].inMemory = false;
			s->pageAddress[replaceIndex].recently = 0;
			int blockID = s->pageAddress[replaceIndex].blockID;
			pr->blockID = blockID;
			pr->inMemory = true;
			pr->recently = 0;
		}
		else {
			s->pageAddress[replaceIndex].inMemory = false;
			s->pageAddress[replaceIndex].recently = 0;
			int blockID = s->pageAddress[replaceIndex].blockID;
			pr->blockID = blockID;
			pr->inMemory = true;
			pr->recently = 0;
		}
	}

}

int LRU(SegmentRecord* s) {

	//遍历该段
	int index = 0;
	int max =s->recently- s->pageAddress[0].recently;
	for (int i = 1; i < s->pageCnt; i++) {
		if (s->recently-s->pageAddress[i].recently > max)
		{
			index = i;
			max = s->recently-s->pageAddress[i].recently;
		}
	}
	return index;


}

//逻辑地址转换成物理地址,return:0 转换正常; 1 段越界 ; 2 页越界
int addressConvert(PCB* process, int segId, int pageId, int offset, int& paddr) {/*进程，虚拟地址(段号，页号，偏移)，物理地址*/
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

		if (segment->pageAddress[pageId].inMemory == false)
		{
			//缺页了
			Lack_Page_Interrupt(segment, &segment->pageAddress[pageId]);
		}
		segment->recently++;
		segment->pageAddress[pageId].recently++;
		PageRecord* pr = &(segment->pageAddress[pageId]);//获取页表项
		paddr = PAGESIZE * (pr->blockID) + offset;  //根据映射的物理块号和偏移量获取物理地址
		return 0;
	}
}
int main()
{
	memset(MemmoryBlock, 0, BLOCKCNT);
	memset(DiskBlock, 0, DISKBOCKCNT);
	PCB* process1 = (PCB *)malloc(sizeof(PCB));
	process1->id = 1;
	strcpy(process1->name, "进程1");
	SegmentTable segTable;
	segTable.size = 3;
	segTable.table = (SegmentRecord*)malloc(sizeof(SegmentRecord) * 3);
	segTable.table[0].id = 0;
	segTable.table[0].pageCnt = 20;
	segTable.table[1].id =1;
	segTable.table[1].pageCnt =30;
	segTable.table[2].id = 2;
	segTable.table[2].pageCnt = 40;
	process1->segment = &segTable;
	apply(process1);
	destory(process1);
}