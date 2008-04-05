/*====================================================================
* Project: Board Support Package (BSP)
* Developed using:
* Function: ARM9 Memory Management common definitions
*
* Copyright HighTec EDV-Systeme GmbH 1982-2006
*====================================================================*/

#ifndef __ARM9_MM_H__
#define __ARM9_MM_H__


/* -------------------------------------------------------------------------
	MMU initialization:

	These structures are laid down in memory to define the translation table.
 */

/*
 * ARM Translation Table Base Bit Masks
 */
#define ARM_TRANSLATION_TABLE_MASK		0xFFFFC000

/*
 * ARM Domain Access Control Bit Masks
 */
#define ARM_ACCESS_TYPE_NO_ACCESS(domain_num)	(0x0 << (domain_num)*2)
#define ARM_ACCESS_TYPE_CLIENT(domain_num)		(0x1 << (domain_num)*2)
#define ARM_ACCESS_TYPE_MANAGER(domain_num)		(0x3 << (domain_num)*2)

struct ARM_MMU_FIRST_LEVEL_FAULT
{
	unsigned int id : 2;
	unsigned int sbz : 30;
};
#define ARM_MMU_FIRST_LEVEL_FAULT_ID		0x0

struct ARM_MMU_FIRST_LEVEL_PAGE_TABLE
{
	unsigned int id : 2;
	unsigned int imp : 2;
	unsigned int domain : 4;
	unsigned int sbz : 1;
	unsigned int base_address : 23;
};
#define ARM_MMU_FIRST_LEVEL_PAGE_TABLE_ID	0x1

struct ARM_MMU_FIRST_LEVEL_SECTION
{
	unsigned int id : 2;
	unsigned int b : 1;
	unsigned int c : 1;
	unsigned int imp : 1;
	unsigned int domain : 4;
	unsigned int sbz0 : 1;
	unsigned int ap : 2;
	unsigned int sbz1 : 8;
	unsigned int base_address : 12;
};
#define ARM_MMU_FIRST_LEVEL_SECTION_ID		0x2

struct ARM_MMU_FIRST_LEVEL_FINE_PAGE_TABLE
{
	unsigned int id : 2;
	unsigned int sbz0 : 2;
	unsigned int imp : 1;
	unsigned int domain : 4;
	unsigned int sbz1 : 3;
	unsigned int base_address : 20;
};
#define ARM_MMU_FIRST_LEVEL_FINE_PAGE_ID	0x3

#define ARM_MMU_FIRST_LEVEL_DESCRIPTOR_ADDRESS(ttb_base, table_index) \
	(unsigned int *)((unsigned int)(ttb_base) + ((table_index) << 2))

#define ARM_FIRST_LEVEL_PAGE_TABLE_SIZE		0x4000


#define ARM_MMU_SECTION(ttb_base, actual_base, virtual_base,				\
						cacheable, bufferable, perm)						\
	do {																	\
		register union ARM_MMU_FIRST_LEVEL_DESCRIPTOR desc;					\
																			\
		desc.word = 0;														\
		desc.section.id = ARM_MMU_FIRST_LEVEL_SECTION_ID;					\
		desc.section.imp = 1;												\
		desc.section.domain = 0;											\
		desc.section.c = (cacheable);										\
		desc.section.b = (bufferable);										\
		desc.section.ap = (perm);											\
		desc.section.base_address = (actual_base);							\
		*ARM_MMU_FIRST_LEVEL_DESCRIPTOR_ADDRESS(ttb_base, (virtual_base))	\
							= desc.word;									\
	} while (0)

#define X_ARM_MMU_SECTION(abase,vbase,size,cache,buff,access)		\
	{																\
		int i; int j = abase; int k = vbase;						\
		for (i = size; i > 0 ; i--,j++,k++)							\
		{															\
			ARM_MMU_SECTION(ttb_base, j, k, cache, buff, access);	\
		}															\
	}

union ARM_MMU_FIRST_LEVEL_DESCRIPTOR
{
	unsigned int								word;
	struct ARM_MMU_FIRST_LEVEL_FAULT			fault;
	struct ARM_MMU_FIRST_LEVEL_PAGE_TABLE		page_table;
	struct ARM_MMU_FIRST_LEVEL_SECTION			section;
	struct ARM_MMU_FIRST_LEVEL_FINE_PAGE_TABLE	fine_page_table;
};

#define ARM_UNCACHEABLE				0
#define ARM_CACHEABLE				1
#define ARM_UNBUFFERABLE			0
#define ARM_BUFFERABLE				1

#define ARM_ACCESS_PERM_NONE_NONE	0
#define ARM_ACCESS_PERM_RO_NONE		0
#define ARM_ACCESS_PERM_RO_RO		0
#define ARM_ACCESS_PERM_RW_NONE		1
#define ARM_ACCESS_PERM_RW_RO		2
#define ARM_ACCESS_PERM_RW_RW		3

/*
 * Initialization for the Domain Access Control Register
 */
#define ARM_ACCESS_DACR_DEFAULT		(	\
		ARM_ACCESS_TYPE_MANAGER(0)    |	\
		ARM_ACCESS_TYPE_NO_ACCESS(1)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(2)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(3)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(4)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(5)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(6)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(7)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(8)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(9)  |	\
		ARM_ACCESS_TYPE_NO_ACCESS(10) |	\
		ARM_ACCESS_TYPE_NO_ACCESS(11) |	\
		ARM_ACCESS_TYPE_NO_ACCESS(12) |	\
		ARM_ACCESS_TYPE_NO_ACCESS(13) |	\
		ARM_ACCESS_TYPE_NO_ACCESS(14) |	\
		ARM_ACCESS_TYPE_NO_ACCESS(15)  )


#ifdef __cplusplus
  extern "C" {
#endif /* __cplusplus */

/* Initialization of platform's MMU */
void hal_mmu_init(void);

#ifdef __cplusplus
  }
#endif /* __cplusplus */

#endif /* __ARM9_MM_H__ */
