/**
 *    @file  seqgraph_base.hpp
 *   @brief  Sequence graph base definitions
 *
 *  This is a base header file for sequence graph class definition.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Thu Feb 21, 2019  17:51
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_SEQGRAPH_BASE_HPP__
#define  GUM_SEQGRAPH_BASE_HPP__

#include <string>
#include <vector>
#include <utility>
#include <tuple>

#include <sparsehash/dense_hash_map>

#include "basic_types.hpp"


namespace gum {
  /* Graph dynamic specialization tag. */
  struct Dynamic;
  /* Graph succinct specialization tag. */
  struct Succinct;
  /* Graph directed specialization tag. */
  struct Directed;
  /* Graph bidirected specialization tag. */
  struct Bidirected;

  template< typename TSpec, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class GraphTrait;

  template< uint8_t TIdWidth, uint8_t TOffsetWidth >
    class GraphTrait< Dynamic, TIdWidth, TOffsetWidth > {
      public:
        using id_type = integer_t< TIdWidth >;
        using offset_type = integer_t< TOffsetWidth >;
        using rank_type = id_type;
        using nodes_type = std::vector< id_type >;
        using rank_map_type = google::dense_hash_map< id_type, rank_type >;
    };  /* ----------  end of template class GraphTrait  ---------- */

  template< typename TSpec, typename TDir, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class DirectedGraphTrait;

  template< uint8_t ...TWidths >
    class DirectedGraphTrait< Dynamic, Bidirected, TWidths... >
    : public GraphTrait< Dynamic, TWidths... > {
      private:
        using base_type = GraphTrait< Dynamic, TWidths... >;
      public:
        using typename base_type::id_type;
        using typename base_type::offset_type;
        using typename base_type::rank_type;
        using typename base_type::nodes_type;
        using typename base_type::rank_map_type;
        using side_type = std::pair< id_type, bool >;
        using link_type = std::tuple< id_type, bool, id_type, bool >;
        using adjs_type = std::vector< side_type >;
        using adj_map_type = google::dense_hash_map< side_type, adjs_type >;

          inline side_type
        from_side( link_type sides ) {
          return side_type( std::get<0>( sides ), std::get<1>( sides ) );
        }

          inline side_type
        to_side( link_type sides ) {
          return side_type( std::get<2>( sides ), std::get<3>( sides ) );
        }

          inline link_type
        merge_sides( side_type from, side_type to ) {
          return link_type( from.first, from.second, to.first, to.second );
        }
    };  /* ----------  end of template class DirectedGraphTrait  ---------- */

  template< uint8_t ...TWidths >
    class DirectedGraphTrait< Dynamic, Directed, TWidths... >
    : public GraphTrait< Dynamic, TWidths... > {
      private:
        using base_type = GraphTrait< Dynamic, TWidths... >;
      public:
        using typename base_type::id_type;
        using typename base_type::offset_type;
        using typename base_type::rank_type;
        using typename base_type::nodes_type;
        using typename base_type::rank_map_type;
        using side_type = id_type;
        using link_type = std::pair< id_type, id_type >;
        using adjs_type = nodes_type;
        using adj_map_type = google::dense_hash_map< side_type, adjs_type >;

          inline side_type
        from_side( link_type sides ) {
          return sides.first;
        }

          inline side_type
        to_side( link_type sides ) {
          return sides.second;
        }

          inline link_type
        merge_sides( side_type from, side_type to ) {
          return link_type( from, to );
        }
    };  /* ----------  end of template class DirectedGraphTrait  ---------- */

  template< typename TSpec, typename TDir, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class DirectedGraph;

  template< typename TSpec, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class NodePropertyTrait;

  template< uint8_t ...TWidths >
    class NodePropertyTrait< Dynamic, TWidths... > {
      private:
        using spec_type = Dynamic;
        using trait_type = GraphTrait< spec_type, TWidths... >;
      public:
        using typename trait_type::id_type;
        using typename trait_type::offset_type;
        using typename trait_type::rank_type;
        class Node
        {
          public:
            /* === TYPEDEFS  === */
            using sequence_type = std::string;
            using name_type = std::string;
            /* === LIFECYCLE === */
            Node( sequence_type s, name_type n="" )  /* constructor */
              : sequence( s ), name( n ) { }
            Node( ) : Node( "", "" ) { }             /* constructor */

            /* === DATA MEMBERS === */
            sequence_type sequence;
            name_type name;
        };  /* -----  end of class Node  ----- */
        using node_type = Node;
        using value_type = node_type;
        using typename value_type::sequence_type;
        using typename value_type::name_type;
        using container_type = std::vector< value_type >;
    };

  template< typename TSpec, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class NodeProperty;

  template< typename TSpec, typename TDir, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class EdgePropertyTrait;

  template< typename TDir, uint8_t ...TWidths >
    class EdgePropertyTrait< Dynamic, TDir, TWidths... > {
      private:
        using spec_type = Dynamic;
        using trait_type = DirectedGraphTrait< spec_type, TDir, TWidths... >;
      public:
        using typename trait_type::id_type;
        using typename trait_type::offset_type;
        using typename trait_type::link_type;
        class Edge {
          public:
            /* === LIFECYCLE === */
            Edge( offset_type overlap_=0 )    /* constructor */
              : overlap( overlap_ )
            { }

            /* === DATA MEMBERS === */
            offset_type overlap;
        };  /* -----  end of class Edge  ----- */
        using edge_type = Edge;
        using key_type = link_type;
        using value_type = edge_type;
        using container_type = google::dense_hash_map< key_type, value_type >;
    };

  template< typename TSpec, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class EdgeProperty;

  template< typename TSpec, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class GraphProperty;

  template< typename TSpec, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class SeqGraph;
}  /* -----  end of namespace gum  ----- */

#endif  /* ----- #ifndef GUM_SEQGRAPH_BASE_HPP__  ----- */
