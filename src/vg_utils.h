/**
 *    @file  vg_utils.h
 *   @brief  vg utility functions and data types.
 *
 *  This header file is a part of the utility header files specialised for vg data type.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Jul 30, 2019  17:02
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */


#ifndef  GUM_VG_UTILS_H__
#define  GUM_VG_UTILS_H__

#include <string>

#include "vg.pb.h"
#include "stream/stream.hpp"

#include "seqgraph.h"
#include "basic_utils.h"

#define VG_FILE_EXT ".vg"

namespace gum {
  namespace utils {
    template< uint8_t ...TWidths >
        inline void
      add( SeqGraph< Dynamic, TWidths... >& graph, vg::Node const& node ) {
        using graph_type = SeqGraph< Dynamic, TWidths... >;
        using node_type = typename graph_type::node_type;
        graph.add_node( node.id(), node_type( node.sequence(), node.name() ) )
      }

    template< uint8_t ...TWidths >
        inline void
      add( SeqGraph< Dynamic, TWidths... >& graph, vg::Edge const& edge ) {
        using graph_type = SeqGraph< Dynamic, TWidths... >;
        using link_type = typename graph_type::link_type;
        using edge_type = typename graph_type::edge_type;
        graph.add_edge( link_type( edge.from, edge.from_start, edge.to, edge.to_end ),
            edge_type( edge.overlap ) )
      }

    template< uint8_t ...TWidths >
        inline void
      extend( SeqGraph< Dynamic, TWidths... >& graph, vg::Graph const& other ) {
        for ( std:size_t i = 0; i < other.node_size(); ++i ) {
          add( graph, other.node( i ) );
        }
        for ( std::size_t i = 0; i < other.edge_size(); ++i ) {
          add( graph, other.edge( i ) );
        }
        // :TODO:Tue Jul 30 18:33:\@cartoonist:
        // Add paths
      }  /* -----  end of template function extend  ----- */

    template< uint8_t ...TWidths >
        inline void
      extend_vg( SeqGraph< Dynamic, TWidths... >& graph, std::istream& in ) {
        auto handle_chunks = [&graph]( vg::Graph const& other ) {
          extend( graph, other );
        };
        stream::for_each( in, handle_chunks );
      }  /* -----  end of template function extend_vg  ----- */

    template< uint8_t ...TWidths >
        inline void
      extend( SeqGraph< Dynamic, TWidths... >& graph, std::string const& fname ) {
        std::ifstream ifs( fname, std::ifstream::in | std::ifstream::binary );
        if( !ifs ) {
          throw std::runtime_error( "cannot open file '" + fname + "'" );
        }

        if ( utils::start_with( fname, VG_FILE_EXT ) ) extend_vg( graph, ifs );
      }  /* -----  end of template function extend  ----- */
  }  /* -----  end of namespace utils  ----- */
}  /* -----  end of namespace gum  ----- */

#endif  /* ----- #ifndef GUM_VG_UTILS_H__  ----- */
