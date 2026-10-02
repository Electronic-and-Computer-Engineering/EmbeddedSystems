/*
 * unity_config.h
 *
 *  Created on: 02.10.2026
 *      Author: mayerflo
 */

#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#include <stdio.h>

#define UNITY_EXCLUDE_FLOAT
#define UNITY_EXCLUDE_DOUBLE
#define UNITY_EXCLUDE_SETJMP_H

#define UNITY_OUTPUT_CHAR(c)    putchar(c)
#define UNITY_OUTPUT_FLUSH()    fflush(stdout)

#endif /* UNITY_CONFIG_H */
