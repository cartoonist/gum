/**
 *    @file  vg_utils.hpp
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

#ifndef  GUM_VG_UTILS_HPP__
#define  GUM_VG_UTILS_HPP__

#include <string>
#include <istream>
#include <functional>

#include <vg/vg.pb.h>
#include <vg/io/stream.hpp>

#include "seqgraph.hpp"
#include "basic_utils.hpp"


namespace gum {
  namespace util {
    struct VGFormat { };
    const std::string VG_FILE_EXT(".vg");

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >::id_type
    add( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
         vg::Node const& node,
         bool id_as_name=false )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >;
      using node_type = typename graph_type::node_type;
      if ( node.name().empty() ) id_as_name = true;
      return graph.add_node( node_type( node.sequence(), ( id_as_name ?
                                                           std::to_string( node.id() ) :
                                                           node.name() ) ) );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline void
    add( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
         typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >::id_type src_id,
         typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >::id_type sink_id,
         vg::Edge const& edge )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >;
      using link_type = typename graph_type::link_type;
      using edge_type = typename graph_type::edge_type;
      graph.add_edge(
          link_type( src_id, !edge.from_start(), sink_id, edge.to_end() ),
          edge_type( edge.overlap() ) );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline void
    extend( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
            vg::Graph& other,
            bool id_as_name=false )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >;
      using id_type = typename graph_type::id_type;

      google::sparse_hash_map< decltype( vg::Node().id() ), id_type > ids;
      for ( const auto& node : other.node() ) {
        ids.insert( { node.id(), add( graph, node ) } );
      }
      for ( const auto& edge : other.edge() ) {
        add( graph, ids[ edge.from() ], ids[ edge.to() ], edge );
      }
      // :TODO:Tue Jul 30 18:33:\@cartoonist:
      // Add paths
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline void
    extend_vg( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
               std::istream& in )
    {
      std::function< void( vg::Graph& ) > handle_chunks =
          [&graph]( vg::Graph& other ) {
            extend( graph, other );
          };
      vg::io::for_each( in, handle_chunks );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline void
    extend_vg( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
               std::string fname )
    {
      std::ifstream ifs( fname, std::ifstream::in | std::ifstream::binary );
      if( !ifs ) {
        throw std::runtime_error( "cannot open file '" + fname + "'" );
      }
      extend_vg( graph, ifs );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline void
    extend( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
            std::string fname,
            VGFormat )
    {
      extend_vg( graph, fname );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_VG_UTILS_HPP__ --- */