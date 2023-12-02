#pragma once

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