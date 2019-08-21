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

  template< typename TSpec, uint8_t TIdWidth = 64, uint8_t TOffsetWidth = 64 >
    class GraphTrait;

  template< uint8_t TIdWidth, uint8_t TOffsetWidth >
    class GraphTrait< Dynamic, TIdWidth, TOffsetWidth > {
      public:
        using id_type = integer_t< TIdWidth >;
        using offset_type = integer_t< TOffsetWidth >;
        using nodes_type = std::vector< id_type >;
        using rank_type = typename nodes_type::size_type;
        using rank_map_type = google::dense_hash_map< id_type, rank_type >;
    };  /* ----------  end of template class GraphTrait  ---------- */

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
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

        constexpr static side_type dummy_side = { 0, false };

          constexpr static inline side_type
        get_dummy_side( ) {
          return dummy_side;
        }  /* -----  end of method DirectedGraphTrait::get_dummy_side  ----- */

          constexpr static inline link_type
        get_dummy_link( ) {
          return DirectedGraphTrait::merge_sides( dummy_side, dummy_side );
        }  /* -----  end of method DirectedGraphTrait::get_dummy_link  ----- */

          static inline void
        init_rank_map( rank_map_type& m )
        {
          // `dense_hash_map` requires to set empty key before any `insert` call.
          m.set_empty_key( 0 );  // ID cannot be zero, so it can be used as empty key.
        }  /* -----  end of method DirectedGraphTrait::init_rank_map  ----- */

          static inline void
        init_adj_map( adj_map_type& m )
        {
          // `dense_hash_map` requires to set empty key before any `insert` call.
          m.set_empty_key( DirectedGraphTrait::get_dummy_side() );
        }  /* -----  end of method DirectedGraphTrait::init_adj_map  ----- */

          constexpr static inline side_type
        from_side( link_type sides ) {
          return side_type( std::get<0>( sides ), std::get<1>( sides ) );
        }  /* -----  end of method DirectedGraphTrait::from_side  ----- */

          constexpr static inline side_type
        to_side( link_type sides ) {
          return side_type( std::get<2>( sides ), std::get<3>( sides ) );
        }  /* -----  end of method DirectedGraphTrait::to_side  ----- */

          constexpr static inline link_type
        merge_sides( side_type from, side_type to ) {
          return link_type( from.first, from.second, to.first, to.second );
        }  /* -----  end of method DirectedGraphTrait::merge_sides  ----- */
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

        constexpr static side_type dummy_side = 0;

          constexpr static inline side_type
        get_dummy_side( ) {
          return dummy_side;
        }  /* -----  end of method DirectedGraphTrait::get_dummy_side  ----- */

          constexpr static inline link_type
        get_dummy_link( ) {
          return DirectedGraphTrait::merge_sides( dummy_side, dummy_side );
        }  /* -----  end of method DirectedGraphTrait::get_dummy_link  ----- */

          static inline void
        init_rank_map( rank_map_type& m )
        {
          // `dense_hash_map` requires to set empty key before any `insert` call.
          m.set_empty_key( 0 );  // ID cannot be zero, so it can be used as empty key.
        }  /* -----  end of method DirectedGraphTrait::init_rank_map  ----- */

          static inline void
        init_adj_map( adj_map_type& m )
        {
          // `dense_hash_map` requires to set empty key before any `insert` call.
          m.set_empty_key( DirectedGraphTrait::get_dummy_side() );
        }  /* -----  end of method DirectedGraphTrait::init_adj_map  ----- */

          constexpr static inline side_type
        from_side( link_type sides ) {
          return sides.first;
        }  /* -----  end of method DirectedGraphTrait::from_side  ----- */

          constexpr static inline side_type
        to_side( link_type sides ) {
          return sides.second;
        }  /* -----  end of method DirectedGraphTrait::to_side  ----- */

          constexpr static inline link_type
        merge_sides( side_type from, side_type to ) {
          return link_type( from, to );
        }  /* -----  end of method DirectedGraphTrait::merge_sides  ----- */
    };  /* ----------  end of template class DirectedGraphTrait  ---------- */

  template< typename TSpec, typename TDir = Bidirected, uint8_t ...TWidths >
    class DirectedGraph;

  template< typename TSpec, uint8_t ...TWidths >
    class NodePropertyTrait;

  template< uint8_t ...TWidths >
    class NodePropertyTrait< Dynamic, TWidths... > {
      private:
        using spec_type = Dynamic;
        using trait_type = GraphTrait< spec_type, TWidths... >;
      public:
        using id_type = typename trait_type::id_type;
        using offset_type = typename trait_type::offset_type;
        using rank_type = typename trait_type::rank_type;
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
        using sequence_type = typename value_type::sequence_type;
        using name_type = typename value_type::name_type;
        using container_type = std::vector< value_type >;
    };

  template< typename TSpec, uint8_t ...TWidths >
    class NodeProperty;

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
    class EdgePropertyTrait;

  template< typename TDir, uint8_t ...TWidths >
    class EdgePropertyTrait< Dynamic, TDir, TWidths... > {
      private:
        using spec_type = Dynamic;
        using trait_type = DirectedGraphTrait< spec_type, TDir, TWidths... >;
      public:
        using id_type = typename trait_type::id_type;
        using offset_type = typename trait_type::offset_type;
        using link_type = typename trait_type::link_type;
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

          static inline void
        init_container( container_type& c ) {
          c.set_empty_key( trait_type::get_dummy_link( ) );
        }  /* -----  end of method EdgePropertyTrait::init_container  ----- */
    };

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
    class EdgeProperty;

  template< typename TSpec, uint8_t TIdWidth, uint8_t TOffsetWidth >
    class GraphProperty;

  template< typename TSpec, uint8_t ...TWidths >
    class SeqGraph;

  template< typename TSpec, uint8_t ...TWidths >
    class DiSeqGraph;
}  /* -----  end of namespace gum  ----- */

#endif  /* ----- #ifndef GUM_SEQGRAPH_BASE_HPP__  ----- */
