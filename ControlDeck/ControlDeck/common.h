#pragma once
#include <stdio.h>

#define DBG_ASSERT(condition, msg, ...) do {if (!(condition)) {fprintf(stderr, "(%s:%d) %s : " msg "\n", __FILE__, __LINE__, #condition, ##__VA_ARGS__ ); fflush(stderr); __debugbreak();}} while (0)
#define DBG_CRASH(msg, ...) DBG_ASSERT(0, msg, __VA_ARGS__)

#define arrSizeof(arr) (sizeof(arr) / sizeof(arr[0]))