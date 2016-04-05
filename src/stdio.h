#pragma once

#ifndef __STDIO_H__
#define __STDIO_H__

#include <stddef.h>
#include <stdarg.h>

int putchar(int c);
int puts(const char *str);
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list args);
int snprintf(char *buf, size_t size, const char *fmt, ...);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

static inline void local_irq_disable(void)
{ __asm__ volatile ("cli" : : : "cc"); }

static inline void local_irq_enable(void)
{ __asm__ volatile ("sti" : : : "cc"); }

#define DBG_ASSERT(cond)				\
do {							\
	if (!(cond)) {					\
		local_irq_disable();			\
		printf("Condition %s failed", #cond);	\
		while (1);				\
	}						\
} while (0)

#endif /*__STDIO_H__*/
