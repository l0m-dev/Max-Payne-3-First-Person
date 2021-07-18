#pragma once

#include "main.h"

template <typename T>
inline static void nativePush(T val)
{
	UINT32 val32 = 0;
	if (sizeof(T) > sizeof(UINT32))
	{
		throw "value size > 32 bit";
	}
	*reinterpret_cast<T*>(&val32) = val;
	nativePush32(val32);
}

template <typename R>
inline static R invoke(Hash hash)
{
	nativeInit(hash);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1>
inline static R invoke(Hash hash, T1 P1)
{
	nativeInit(hash);

	nativePush(P1);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2>
inline static R invoke(Hash hash, T1 P1, T2 P2)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14, T15 P15)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);
	nativePush(P15);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14, T15 P15, T16 P16)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);
	nativePush(P15);
	nativePush(P16);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14, T15 P15, T16 P16, T17 P17)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);
	nativePush(P15);
	nativePush(P16);
	nativePush(P17);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14, T15 P15, T16 P16, T17 P17, T18 P18)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);
	nativePush(P15);
	nativePush(P16);
	nativePush(P17);
	nativePush(P18);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14, T15 P15, T16 P16, T17 P17, T18 P18, T19 P19)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);
	nativePush(P15);
	nativePush(P16);
	nativePush(P17);
	nativePush(P18);
	nativePush(P19);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14, T15 P15, T16 P16, T17 P17, T18 P18, T19 P19, T20 P20)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);
	nativePush(P15);
	nativePush(P16);
	nativePush(P17);
	nativePush(P18);
	nativePush(P19);
	nativePush(P20);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20, typename T21>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14, T15 P15, T16 P16, T17 P17, T18 P18, T19 P19, T20 P20, T21 P21)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);
	nativePush(P15);
	nativePush(P16);
	nativePush(P17);
	nativePush(P18);
	nativePush(P19);
	nativePush(P20);
	nativePush(P21);

	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20, typename T21, typename T22>
inline static R invoke(Hash hash, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6, T7 P7, T8 P8, T9 P9, T10 P10, T11 P11, T12 P12, T13 P13, T14 P14, T15 P15, T16 P16, T17 P17, T18 P18, T19 P19, T20 P20, T21 P21, T22 P22)
{
	nativeInit(hash);

	nativePush(P1);
	nativePush(P2);
	nativePush(P3);
	nativePush(P4);
	nativePush(P5);
	nativePush(P6);
	nativePush(P7);
	nativePush(P8);
	nativePush(P9);
	nativePush(P10);
	nativePush(P11);
	nativePush(P12);
	nativePush(P13);
	nativePush(P14);
	nativePush(P15);
	nativePush(P16);
	nativePush(P17);
	nativePush(P18);
	nativePush(P19);
	nativePush(P20);
	nativePush(P21);
	nativePush(P22);

	return *reinterpret_cast<R*>(nativeCall());
}

#define NATIVE_VARARGS_BEGIN(argCount) \
va_list _nativeVarArgsList; \
const int _nativeVarArgsCount = argCount * 2; \
va_start(_nativeVarArgsList, _nativeVarArgsCount); 

#define NATIVE_VARARGS_PUSH() \
for (int i = 0; i < _nativeVarArgsCount; i++) \
{ \
	nativePush(va_arg(_nativeVarArgsList, Any)); \
}

#define NATIVE_VARARGS_END() va_end(_nativeVarArgsList);