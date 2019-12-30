// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#ifndef STDAFX_H
#define STDAFX_H

#ifndef WIN32
typedef   unsigned   int		UINT;
typedef   unsigned   char		UCHAR;
typedef   unsigned   short		USHORT;
#else
#define  snprintf _snprintf

#pragma warning(disable:4786)
#pragma warning (disable:4819)
#endif

typedef		unsigned long		ULONG;
typedef		long long			Int64;
//以下定义空格不得去除、添加、换成table符号，否则会影响出包
#define SDK_VERSION "1.3.0-5.1"

#undef	SAFE_DELETE
#define SAFE_DELETE(ptr)		{ if(ptr){ delete ptr;ptr = 0; } }
// TODO: 在此处引用程序需要的其他头文件
#endif
