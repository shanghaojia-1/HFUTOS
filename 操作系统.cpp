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
		SegmentRecord s = p->segment->table[i];
		s.pageAddress = (PageRecord*)malloc(sizeof(PageRecord) * s.pageCnt);//申请足够大的页表空间
		if (remainBlock > 0) 
		{
			s.inMemory = true;
			for (int j = 0; j < s.pageCnt; j++)
			{
				if (remainBlock > 0)
				{
					for (int k = 0; k < BLOCKCNT; k++)
					{
						if (MemmoryBlock[k] == 0)
						{
							//如果该内存块空闲则立刻分配给程序
							s.pageAddress[j].id = j;
							s.pageAddress[j].blockID = k;
							s.pageAddress[j].inMemory = true;
							s.pageAddress[j].modify = false;
							s.pageAddress[j].recently = 0;
							s.pageAddress[j].DiskID = -1;
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
							
							s.pageAddress[j].id = j;
							s.pageAddress[j].blockID = -1;
							s.pageAddress[j].inMemory = false;
							s.pageAddress[j].modify = false;
							s.pageAddress[j].recently = 0;
							s.pageAddress[j].DiskID = k;
							DiskBlock[k] = 1;
							break;
						}
					}

				}

			}
		}
		else
		{
			s.inMemory = false;
			for (int j = 0; j < s.pageCnt; j++)
			{
				for (int k = 0; k < BLOCKCNT; k++)
				{
					if (DiskBlock[k] == 0)
					{
					
						s.pageAddress[j].id = j;
						s.pageAddress[j].blockID = -1;
						s.pageAddress[j].inMemory = false;
						s.pageAddress[j].modify = false;
						s.pageAddress[j].recently = 0;
						s.pageAddress[j].DiskID = k;
						DiskBlock[k] = 1;
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

}
int main()
{
	memset(MemmoryBlock, 0, BLOCKCNT);
	memset(DiskBlock, 0, DISKBOCKCNT);
	PCB* process1 = new PCB();
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

}