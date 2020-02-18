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
              template< class, class, uint8_t ... > class TGraphProp,
              uint8_t ...TWidths >
    inline typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >::id_type
    add( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
         vg::Node const& node,
         bool id_as_name=false )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >;
      using node_type = typename graph_type::node_type;

      if ( node.name().empty() ) id_as_name = true;
      return graph.add_node( node_type( node.sequence(), ( id_as_name ?
                                                           std::to_string( node.id() ) :
                                                           node.name() ) ) );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              uint8_t ...TWidths >
    inline void
    add( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
         typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >::id_type src_id,
         typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >::id_type sink_id,
         vg::Edge const& edge )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >;
      using link_type = typename graph_type::link_type;
      using edge_type = typename graph_type::edge_type;

      graph.add_edge(
          link_type( src_id, !edge.from_start(), sink_id, edge.to_end() ),
          edge_type( edge.overlap() ) );
    }

    template< typename TFunc,
              template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              uint8_t ...TWidths >
    inline void
    add( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
         TFunc to_id,
         vg::Path const& path )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >;
      using id_type = typename graph_type::id_type;
      using mappings_type = std::decay_t< decltype( path.mapping() ) >;
      using nodes_type = RandomAccessProxyContainer< mappings_type, id_type >;
      using orientations_type = RandomAccessProxyContainer< mappings_type, bool >;

      auto const& mappings = path.mapping();

      auto get_id =
          [&to_id]( auto const& m ) {
            return to_id( m.position().node_id() );
          };
      auto get_orient =
          []( auto const& m ) {
            return m.position().is_reverse();
          };
      nodes_type nodes( &mappings, get_id );
      orientations_type orients( &mappings, get_orient );
      graph.add_path( nodes.begin(), nodes.end(),
                      orients.begin(), orients.end(),
                      path.name() );
    }


    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              uint8_t ...TWidths >
    inline void
    extend( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
            vg::Graph& other,
            bool id_as_name=false )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >;
      using id_type = typename graph_type::id_type;

      google::sparse_hash_map< decltype( vg::Node().id() ), id_type > ids;
      for ( auto const& node : other.node() ) {
        ids.insert( { node.id(), add( graph, node, id_as_name ) } );
      }
      for ( auto const& edge : other.edge() ) {
        add( graph, ids[ edge.from() ], ids[ edge.to() ], edge );
      }
      auto to_id = [&ids]( auto const& vgid ) { return ids[ vgid ]; };
      for ( auto const& path : other.path() ) {
        add( graph, to_id, path );
      }
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    extend_vg( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
               std::istream& in,
               TArgs&&... args )
    {
      std::function< void( vg::Graph& ) > handle_chunks =
          [&]( vg::Graph& other ) {
            extend( graph, other, std::forward< TArgs >( args )... );
          };
      vg::io::for_each( in, handle_chunks );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    extend_vg( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
               std::string fname,
               TArgs&&... args )
    {
      std::ifstream ifs( fname, std::ifstream::in | std::ifstream::binary );
      if( !ifs ) {
        throw std::runtime_error( "cannot open file '" + fname + "'" );
      }
      extend_vg( graph, ifs, std::forward< TArgs >( args )... );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              template< class, class, uint8_t ... > class TGraphProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    extend( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TGraphProp, TWidths... >& graph,
            std::string fname,
            VGFormat, TArgs&&... args )
    {
      extend_vg( graph, fname, std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_VG_UTILS_HPP__ --- */
