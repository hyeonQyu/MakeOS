#include "Page.h"

/**
 *	IA-32e ��� Ŀ���� ���� ������ ���̺� ����
 */
void kInitializePageTables( void )
{
	PML4TENTRY* pstPML4TEntry;
	PDPTENTRY* pstPDPTEntry;
	PDENTRY* pstPDEntry;
	PTENTRY* pstPTEntry;
	DWORD dwMappingAddress;
	int i;

	// PML4 ���̺� ����
	// ù ��° ��Ʈ�� �ܿ� �������� ��� 0���� �ʱ�ȭ
	pstPML4TEntry = ( PML4TENTRY* ) 0x100000;
	kSetPageEntryData( &( pstPML4TEntry[ 0 ] ), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0 );
	for( i = 1 ; i < PAGE_MAXENTRYCOUNT ; i++ )
	{
		kSetPageEntryData( &( pstPML4TEntry[ i ] ), 0, 0, 0, 0 );
	}
	
	// ������ ���͸� ������ ���̺� ����
	// �ϳ��� PDPT�� 512GByte���� ���� �����ϹǷ� �ϳ��� �����
	// 64���� ��Ʈ���� �����Ͽ� 64GByte���� ������
	pstPDPTEntry = ( PDPTENTRY* ) 0x101000;
	for( i = 0 ; i < 64 ; i++ )
	{
		kSetPageEntryData( &( pstPDPTEntry[ i ] ), 0, 0x102000 + ( i * PAGE_TABLESIZE ), PAGE_FLAGS_DEFAULT, 0 );
	}
	for( i = 64 ; i < PAGE_MAXENTRYCOUNT ; i++ )
	{
		kSetPageEntryData( &( pstPDPTEntry[ i ] ), 0, 0, 0, 0 );
	}
	
	// ������ ���͸� ���̺� ����
	// �ϳ��� ������ ���͸��� 1GByte���� ���� ���� 
	// �����ְ� 64���� ������ ���͸��� �����Ͽ� �� 64GB���� ����
	pstPDEntry = ( PDENTRY* ) 0x102000;
	dwMappingAddress = 0;
	for( i = 0 ; i < PAGE_MAXENTRYCOUNT * 64 ; i++ )
	{
		// 0x1ff000�� pstPDEntry[0]�� ��ġ
		// ������ ũ�⸦ 4KB�� �ϱ� ���� PS�ʵ��� ��Ʈ�� 0���� ��
		if(i == 0){
			kSetPageEntryData(&(pstPDEntry[i]), 0, 0x142000, PAGE_FLAGS_DEFAULT, 0);
			dwMappingAddress += PAGE_DEFAULTSIZE;
			continue;
		}
		//0xAB8000 ���� �޸� ����
		//0xAB8000�� pstPDEntry[5]�� ��ġ, offset�� 0xB8000�� ����
		if(i == 5){
			kSetPageEntryData( &( pstPDEntry[ i ] ), 0, 0, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0 );
			dwMappingAddress += PAGE_DEFAULTSIZE;
			continue;
		}
		// 32��Ʈ�δ� ���� ��巹���� ǥ���� �� �����Ƿ�, Mbyte ������ ����� ����
		// ���� ����� �ٽ� 4Kbyte�� ������ 32��Ʈ �̻��� ��巹���� �����
		kSetPageEntryData( &( pstPDEntry[ i ] ), 
				( i * ( PAGE_DEFAULTSIZE >> 20 ) ) >> 12, dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0 );
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}	

	pstPTEntry = (PTENTRY*) 0x142000;
	dwMappingAddress = 0;
	for(i = 0; i < PAGE_MAXENTRYCOUNT; i++){
		if(dwMappingAddress == 0x1FF000){
			kSetPageEntryData(&(pstPTEntry[i]), 0, dwMappingAddress, 0, 0);
			dwMappingAddress += 0x1000;		// 4KB
			continue;
		}
		kSetPageEntryData(&(pstPTEntry[i]), 0, dwMappingAddress, PAGE_FLAGS_DEFAULT, 0);
		dwMappingAddress += 0x1000;		// 4KB
	}

}

/**
 *	������ ��Ʈ���� ���� �ּҿ� �Ӽ� �÷��׸� ����
 */
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
		DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags )
{
	pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB = ( dwUpperBaseAddress & 0xFF ) | 
		dwUpperFlags;
}