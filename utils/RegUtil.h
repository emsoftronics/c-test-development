/*
 * =====================================================================================
 *
 *       Filename:  RegUtil.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Monday 08 January 2018 08:07:09  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */


#ifndef __REGUTIL_h__
#define __REGUTIL_h__


typedef struct {
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
    unsigned char b6:1;
    unsigned char b7:1;
} b_byte_t;

typedef struct {
    unsigned char lower:4;
    unsigned char higher:4;
} n_byte_t;

typedef unsigned char reg8_t;

typedef union {
    b_byte_t bits;
    n_byte_t nibbles;
    reg8_t reg8;
} byte_t;

typedef struct {
  byte_t byte0;
  byte_t byte1;
  byte_t byte2;
  byte_t byte3;
} b_dword_t;

typedef struct {
  byte_t byte0;
  byte_t byte1;
} b_word_t;

typedef unsigned short reg16_t;
typedef unsigned int reg32_t;

typedef union {
    b_word_t dword;
    reg16_t reg16;
} word_t;


typedef union {
    b_dword_t dword;
    reg32_t reg32;
} dword_t;


#endif /* __REGUTIL_h__ */
