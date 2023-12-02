#pragma once

/*-------------------------------------------
			       CRASH

	   인위적인 크래시를 발생시키는 매크로
-------------------------------------------*/
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}									

/*-------------------------------------------
		       ASSERT_CRASH

    조건이 거짓이면 크래시를 발생시키는 매크로
-------------------------------------------*/
#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}