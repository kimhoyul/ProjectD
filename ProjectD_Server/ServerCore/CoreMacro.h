#pragma once

#define OUT

/*-------------------------------------------
				    LOCK

	   �������� ũ���ø� �߻���Ű�� ��ũ��
-------------------------------------------*/
#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1);
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0);
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0);

/*-------------------------------------------
				   Memory

  BaseAllocator �� ����Ͽ� �Ҵ� �� ���� �ҋ�
 ��ü�� �����ڿ� �Ҹ��ڸ� ��������� ȣ��� Ŭ����
-------------------------------------------*/
#ifdef _DEBUG
#define Xalloc(size) PoolAllocator::Alloc(size)
#define Xrelease(ptr) PoolAllocator::Release(ptr)
#else
#define Xalloc(size) BaseAllocator::Alloc(size)
#define Xrelease(ptr) BaseAllocator::Release(ptr)
#endif


/*-------------------------------------------
			       CRASH

	   �������� ũ���ø� �߻���Ű�� ��ũ��
-------------------------------------------*/
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}									

/*-------------------------------------------
		       ASSERT_CRASH

    ������ �����̸� ũ���ø� �߻���Ű�� ��ũ��
-------------------------------------------*/
#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}