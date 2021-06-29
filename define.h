#pragma once
#include <malloc.h>
//固定一个页面有4KB
#define PAGESIZE  4096;  //页面大小4KB
#define BLOCKCNT  1024*512 //内存块数
#define DISKBOCKCNT 1024*512*2 //外存块数
struct PageRecord
{
	int id;//页号
	int blockID;//物理块号
	bool inMemory;//是否在内存中
	int recently;//是否被访问
	bool modify;//是否被修改
	int DiskID; //外存地址
};

struct SegmentRecord 
{
	int id; //段号
	int pageCnt;//页面数量，段大小
	PageRecord* pageAddress;//页面起始地址
	int recently;//最近被访问
	bool modify;//是否被修改
	bool inMemory;//是否在内存中
	int DiskID; //外存地址
};
struct SegmentTable//段表
{
	int size;//共有多少段
	SegmentRecord* table;
};

char* MemmoryBlock = new char[BLOCKCNT];//模拟内存块，一共有1024*1024个内存块，一个内存块大小为4KB,所以说共有2GB内存
char* DiskBlock = new char[DISKBOCKCNT]; //模拟外存块
int remainBlock=BLOCKCNT;
struct PCB 
{
	int id;//进程id
	int totalSize;
	char name[10];//进程名字
	SegmentTable* segment;
};
int totalProcessCnt = 0;
