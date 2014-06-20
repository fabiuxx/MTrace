/* 
 * File:   base.h
 * Author: Acer
 *
 * Created on February 12, 2014, 12:25 PM
 */

#ifndef __BASE_H__
#define	__BASE_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>

#define null             NULL
#define cast(p, t)       ((t)(p))
#define V(p)             cast(p, void*)

typedef uint8_t          bool_t;
#define true             1
#define false            0

typedef int8_t           byte_t;
typedef uint8_t          ubyte_t;

typedef uint32_t         key_t;
typedef uint32_t         hash_t;

#define _print(s, ...)   fprintf(s, __VA_ARGS__)
#define _println(s, ...) _print(s, __VA_ARGS__); print(s, "\n")

#define print(...)       _print(stdout, __VA_ARGS__)
#define println(...)     _print(stdout, __VA_ARGS__); _print(stdout, "\n")

#define eprint(...)      _print(stderr, __VA_ARGS__)
#define eprintln(...)    _print(stderr, __VA_ARGS__); _print(stderr, "\n")

#endif	/* __BASE_H__ */