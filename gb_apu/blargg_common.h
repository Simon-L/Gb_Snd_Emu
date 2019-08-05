
// Sets up common environment for Shay Green's libraries.
//
// Don't modify this file directly; #define HAVE_CONFIG_H and put your
// configuration into "config.h".

// Copyright (C) 2004-2005 Shay Green.

#pragma once

// Source files use #include BLARGG_ENABLE_OPTIMIZER before performance-critical code
#ifndef BLARGG_ENABLE_OPTIMIZER
	#define BLARGG_ENABLE_OPTIMIZER "blargg_common.h"
#endif

// Source files have #include BLARGG_SOURCE_BEGIN at the beginning
#ifndef BLARGG_SOURCE_BEGIN
	#define BLARGG_SOURCE_BEGIN "blargg_source.h"
#endif

// uint8_t, int16_t, etc.
#include <cstdint>

// Common standard headers
#include <cstddef>
#include <cassert>
#include <limits>

// blargg_err_t (NULL on success, otherwise error string)
typedef const char* blargg_err_t;
const blargg_err_t blargg_success = 0;