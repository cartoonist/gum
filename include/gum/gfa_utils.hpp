/**
 *    @file  gfa_utils.hpp
 *   @brief  Utility functions and data types for working with GFA files.
 *
 *  This header file is a part of the utility header files specialised for working with
 *  GFA files.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Mon Aug 12, 2019  12:11
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_GFA_UTILS_HPP__
#define  GUM_GFA_UTILS_HPP__

#include <string>
#include <istream>

#include <gfakluge.hpp>

#include "seqgraph.hpp"
#include "basic_utils.hpp"


namespace gum {
  namespace util {
    struct GFAFormat { };
    const std::string GFA_FILE_EXT = ".gfa";

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >::id_type
    add( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
         gfak::sequence_elem const& elem )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >;
      using node_type = typename graph_type::node_type;
      return graph.add_node( node_type( elem.sequence, elem.name ) );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline void
    add( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
         typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >::id_type src_id,
         typename SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >::id_type sink_id,
         gfak::edge_elem const& elem )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >;
      using link_type = typename graph_type::link_type;
      using edge_type = typename graph_type::edge_type;

      auto source_len = graph.node_length( src_id );
      if ( ( elem.type != 1 && elem.type != 2 ) ||
           elem.sink_begin != 0 || elem.source_end != source_len ||
           elem.source_end - elem.source_begin != elem.sink_end ) {
        throw std::runtime_error( "only simple dovetail overlap is supported" );
      }
      link_type link( src_id, elem.source_orientation_forward,
                      sink_id, !elem.sink_orientation_forward );
      graph.add_edge( link, edge_type( elem.sink_end ) );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              uint8_t ...TWidths >
    inline void
    extend( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
            gfak::GFAKluge& other )
    {
      using graph_type = SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >;
      using id_type = typename graph_type::id_type;

      google::sparse_hash_map< std::string, id_type > ids;
      for ( auto const& rec : other.get_name_to_seq() ) {
        ids.insert( { rec.first, add( graph, rec.second ) } );
      }
      for ( auto const& rec : other.get_seq_to_edges() ) {
        for ( auto const& elem : rec.second ) {
          add( graph, ids[ elem.source_name ], ids[ elem.sink_name ], elem );
        }
      }
      // :TODO:Tue Aug 20 16:51:\@cartoonist:
      // Add paths as O-groups
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    extend_gfa( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
                std::istream& in,
                TArgs&&... args )
    {
      gfak::GFAKluge gg;
      gg.parse_gfa_file( in );
      extend( graph, gg, std::forward< TArgs >( args )... );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    extend_gfa( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
                std::string fname,
                TArgs&&... args )
    {
      gfak::GFAKluge gg;
      gg.parse_gfa_file( fname );
      extend( graph, gg, std::forward< TArgs >( args )... );
    }

    template< template< class, uint8_t ... > class TNodeProp,
              template< class, class, uint8_t ... > class TEdgeProp,
              typename ...TArgs,
              uint8_t ...TWidths >
    inline void
    extend( SeqGraph< Dynamic, TNodeProp, TEdgeProp, TWidths... >& graph,
            std::string fname,
            GFAFormat, TArgs&&... args )
    {
      extend_gfa( graph, fname, std::forward< TArgs >( args )... );
    }
  }  /* --- end of namespace util --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_GFA_UTILS_HPP__ --- */
