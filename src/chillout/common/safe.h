/*
  Copyright (c) 2012, Samsung R&D Institute Russia
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/// @brief This namespace contains some basic supplements
/// of the needed libc functions which potentially use heap.
namespace Safe {
  /// @brief Converts an integer to a preallocated string.
  /// @pre base must be less than or equal to 16.
  INLINE char *itoa(int val, char* memory, int base = 10) {
    char* res = memory;
    if (val == 0) {
      res[0] = '0';
      res[1] = '\0';
      return res;
    }
    const int res_max_length = 32;
    int i;
    bool negative = val < 0;
    res[res_max_length - 1] = 0;
    for (i = res_max_length - 2; val != 0 && i != 0; i--, val /= base) {
      res[i] = "0123456789ABCDEF"[val % base];
    }
    if (negative) {
      res[i--] = '-';
    }
    return &res[i + 1];
  }

  /// @brief Converts an unsigned integer to a preallocated string.
  /// @pre base must be less than or equal to 16.
  INLINE char *utoa(uint64_t val, char* memory, int base = 10) {
    char* res = memory;
    if (val == 0) {
      res[0] = '0';
      res[1] = '\0';
      return res;
    }
    const int res_max_length = 32;
    int i;
    res[res_max_length - 1] = 0;
    for (i = res_max_length - 2; val != 0 && i != 0; i--, val /= base) {
      res[i] = "0123456789abcdef"[val % base];
    }
    return &res[i + 1];
  }

  /// @brief Converts a pointer to a preallocated string.
  INLINE char *ptoa(const void *val, char* memory) {
    char* buf = utoa(reinterpret_cast<uint64_t>(val), memory + 32, 16);
    char* result = memory;  // 32
    strcpy(result + 2, buf);  // NOLINT(runtime/printf
    result[0] = '0';
    result[1] = 'x';
    return result;
  }

  ssize_t write2stderr(const char* msg, size_t len) {
    return write(STDERR_FILENO, msg, len);
  }
} // namespace Safe
