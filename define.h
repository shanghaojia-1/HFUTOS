#pragma once
#include <malloc.h>
//�̶�һ��ҳ����4KB
const int PAGESIZE = 4096;  //ҳ���С4KB
const int BLOCKCNT = 1024 * 512;//�ڴ����
const int DISKBOCKCNT = 1024 * 512 * 2;//������
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

char* MemmoryBlock = new char[BLOCKCNT];//ģ���ڴ�飬һ����1024*1024���ڴ�飬һ���ڴ���СΪ4KB,����˵����2GB�ڴ�
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
int totalProcessCnt = 0;
