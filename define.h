#pragma once
#include <malloc.h>
//�̶�һ��ҳ����4KB
const int PAGESIZE = 4;  //ҳ���С4KB
const int BLOCKCNT = 512 * 4;//�ڴ����
const int DISKBOCKCNT = 512*2*4;//������
struct PageRecord
{
	int id;//ҳ��
	int blockID;//������
	bool inMemory;//�Ƿ����ڴ���
	int recently;//�Ƿ񱻷���
	bool modify;//�Ƿ��޸�
	int DiskID; //����ַ
};

struct SegmentRecord 
{
	int id; //�κ�
	int pageCnt;//ҳ���������δ�С
	PageRecord* pageAddress;//ҳ����ʼ��ַ
	int recently;//���������
	bool modify;//�Ƿ��޸�
	bool inMemory;//�Ƿ����ڴ���
	int DiskID; //����ַ
};
struct SegmentTable//�α�
{
	int size;//���ж��ٶ�
	SegmentRecord* table;
};

char* MemoryBlock = new char[BLOCKCNT];//ģ���ڴ�飬һ����1024*1024���ڴ�飬һ���ڴ���СΪ4KB,����˵����2GB�ڴ�
char* DiskBlock = new char[DISKBOCKCNT]; //ģ������
int remainBlock=BLOCKCNT;
int remaindDisk = DISKBOCKCNT;
struct PCB 
{
	int id;//����id
	int totalSize;
	char name[10];//��������
	SegmentTable* segment;
};
PCB* Processes[100];
int totalProcessCnt = 0;
int nowTime = 0;