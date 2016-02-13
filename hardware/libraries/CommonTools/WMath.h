/* Copyright (c) 2009 - http://ruinwesen.com/ */

#ifndef WMATH_H__
#define WMATH_H__

/**
 * \addtogroup CommonTools
 *
 * @{
 *
 * \file
 * Arduino math functions
 **/

/**
 * \addtogroup helpers_wmath Arduino math functions
 *
 * @{
 **/

#ifdef __cplusplus
long random(long);
long random(long, long);
#endif

void randomSeed(unsigned int);
long map(long, long, long, long, long);

#endif /* WMATH_H__ */
