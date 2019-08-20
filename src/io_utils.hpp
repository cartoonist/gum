/**
 *    @file  io_utils.hpp
 *   @brief  Utility functions and data types for IO.
 *
 *  This header file is a part of the utility header files specialised for working with
 *  files and input streams of any file format.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Aug 20, 2019  15:44
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_IO_UTILS_H__
#define  GUM_IO_UTILS_H__

#include "vg_utils.hpp"
#include "gfa_utils.hpp"

namespace gum {
  namespace util {
    template< typename ...TArgs, uint8_t ...TWidths >
        inline void
      extend( SeqGraph< Dynamic, TWidths... >& graph, std::string fname,
          TArgs&&... args )
      {
        if ( util::ends_with( fname, VG_FILE_EXT ) ) {
          extend_vg( graph, fname, std::forward< TArgs >( args )... );
        }
        else if ( util::ends_with( fname, GFA_FILE_EXT ) ) {
          extend_gfa( graph, fname, std::forward< TArgs >( args )... );
        }
        else throw std::runtime_error( "unsupported input file format" );
      }  /* -----  end of template function extend  ----- */
  }  /* -----  end of namespace util  ----- */
}  /* -----  end of namespace gum  ----- */

#endif  /* ----- #ifndef GUM_IO_UTILS_H__  ----- */
