/**
 *    @file  base.hpp
 *   @brief  Basic data types and macros
 *
 *  This header file defines basic data types and macros.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Thu Feb 21, 2019  17:22
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_BASE_HPP__
#define  GUM_BASE_HPP__

#include <string>
#include <iostream>

#define BINARY_NAME "gum"
#define ASSERT(expr)							\
  ((expr)								\
   ? __ASSERT_VOID_CAST (0)						\
   : assert_fail (#expr, BINARY_NAME, __FILE__, __LINE__, __PRETTY_FUNCTION__))


/**
 *  @brief  Assert fail function
 *
 *  @param  expr The assert expression.
 *  @param  outfile The name of output file.
 *  @param  file The source file that assertion failed.
 *  @param  line The line number where assertion failed.
 *  @param  func The function signiture where assertion failed.
 *
 *  Print the message and exit with error code 134.
 */
  inline void
assert_fail( std::string const& expr, std::string const& outfile,
    std::string const& file, int line, std::string const& func )
{
  std::cout << outfile << ": " << file << ":" << line << ": " << func
    << ": Assertion `" << expr << "' failed." << "\n"
    << "Aborted." << std::endl;
  std::exit( 134 );
}

#endif  /* ----- #ifndef GUM_BASE_HPP__  ----- */
