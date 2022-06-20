#pragma once

#include "main.h"
template <typename T> inline static void nativePush(T val) {
	UINT32 val32 = 0;
	if (sizeof(T) > sizeof(UINT32)) {
		throw "value size > 32 bit";
	}
	*reinterpret_cast<T*>(&val32) = val;
	nativePush32(val32);
}

template <typename T> static inline void native_push(const T& value) {
	UINT32 val32 = 0;
	*reinterpret_cast<T*>(&val32) = value;
	nativePush32(val32);
}

template <typename r, typename... args_t> inline r invoke(Hash hash, args_t... args) {
	nativeInit(hash);
	(native_push(args), ...);
	return *reinterpret_cast<r*>(nativeCall());
}

#define NATIVE_VARARGS_BEGIN(argCount)                                                                                                                         \
	va_list _nativeVarArgsList;                                                                                                                            \
	const int _nativeVarArgsCount = argCount * 2;                                                                                                          \
	va_start(_nativeVarArgsList, _nativeVarArgsCount);

#define NATIVE_VARARGS_PUSH()                                                                                                                                  \
	for (int i = 0; i < _nativeVarArgsCount; i++) {                                                                                                        \
		nativePush(va_arg(_nativeVarArgsList, Any));                                                                                                   \
	}

#define NATIVE_VARARGS_END() va_end(_nativeVarArgsList);