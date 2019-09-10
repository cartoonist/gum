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

#include <sparsehash/sparse_hash_map>

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

  /**
   *  @brief  General graph trait.
   *
   *  This class defines any traits that are common between all kinds of graphs
   *  independent of their directionality and implementation.
   */
  template< typename TSpec, uint8_t TIdWidth = 64, uint8_t TOffsetWidth = 64 >
  class GraphBaseTrait;

  template< uint8_t TIdWidth, uint8_t TOffsetWidth >
  class GraphBaseTrait< Dynamic, TIdWidth, TOffsetWidth > {
  public:
    using id_type = integer_t< TIdWidth >;
    using offset_type = integer_t< TOffsetWidth >;
    using nodes_type = std::vector< id_type >;
    using rank_type = typename nodes_type::size_type;
    using rank_map_type = google::sparse_hash_map< id_type, rank_type >;

    static inline void
    init_rank_map( rank_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( 0 );  // ID cannot be zero, so it can be used as empty key.
    }
  };  /* --- end of template class GraphBaseTrait --- */

  /**
   *  @brief  General directed graph trait.
   *
   *  This class defines any traits that are common between all kinds of graphs
   *  of the same directionality independent of their implementation.
   *
   *  NOTE: In a bidirected graph, each node has two sides. Here, `side_type` can get a
   *  value indicating each of these sides. It is defined as a pair `<id_type, bool>` in
   *  which the first element shows the node ID of the side and the second one indicates
   *  which side of the node is meant. Although the graph class __usually__ doesn't care
   *  how this boolean is being used (e.g. either it can store `from_start`/`to_end` or
   *  head/tail), all higher-level functions (e.g. interface functions in `io_utils`
   *  module) assume that the boolean value shows head (`false`) and tail (`true`) of
   *  the node if required; i.e.:
   *
   *              ─┬──────╮     ╭───────┬────┬──────╮     ╭───────┬─
   *           ... │ true ├────⯈│ false │ ID │ true ├────⯈│ false │ ...
   *              ─┴──────╯     ╰───────┴────┴──────╯     ╰───────┴─
   *
   *  In addition, each link can be represented by an integer called 'link type':
   *
   *           ╭────────┬────────┬──────╮
   *           │ From   │ To     │ Type │
   *           ├────────┼────────┼──────┤
   *           │ start  │ start  │ 0    │
   *           │ start  │ end    │ 1    │
   *           │ end    │ start  │ 2    │
   *           │ end    │ end    │ 3    │
   *           ╰────────┴────────┴──────╯
   */
  template< typename TDir >
  class DirectedGraphBaseTrait;

  template< >
  class DirectedGraphBaseTrait< Bidirected > {
  public:
    using side_type = std::pair< id_type, bool >;
    using link_type = std::tuple< id_type, bool, id_type, bool >;
    using linktype_type = unsigned char;

    constexpr static side_type dummy_side = { 0, false };

    constexpr static inline side_type
    from_side( link_type sides )
    {
      return side_type( std::get<0>( sides ), std::get<1>( sides ) );
    }

    constexpr static inline side_type
    to_side( link_type sides )
    {
      return side_type( std::get<2>( sides ), std::get<3>( sides ) );
    }

    constexpr static inline link_type
    merge_sides( side_type from, side_type to )
    {
      return link_type( from.first, from.second, to.first, to.second );
    }

    constexpr static inline linktype_type
    get_type( link_type sides )
    {
      return static_cast< linktype_type >( std::get<1>( sides ) ) * 2 +
          static_cast< linktype_type >( std::get< 3 >( sides ) );
    }

    constexpr static inline side_type
    opposite_side( side_type side )
    {
      return side_type( side.first, !side.second );
    }

    constexpr static inline side_type
    get_dummy_side( )
    {
      return dummy_side;
    }

    constexpr static inline link_type
    get_dummy_link( )
    {
      return DirectedGraphBaseTrait::merge_sides(
          DirectedGraphBaseTrait::get_dummy_side(),
          DirectedGraphBaseTrait::get_dummy_side() );
    }
  };  /* --- end of template class DirectedGraphBaseTrait --- */

  template< >
  class DirectedGraphBaseTrait< Directed > {
  public:
    using side_type = id_type;
    using link_type = std::pair< id_type, id_type >;
    using linktype_type = unsigned char;

    constexpr static side_type dummy_side = 0;

    constexpr static inline side_type
    from_side( link_type sides )
    {
      return sides.first;
    }

    constexpr static inline side_type
    to_side( link_type sides )
    {
      return sides.second;
    }

    constexpr static inline link_type
    merge_sides( side_type from, side_type to )
    {
      return link_type( from, to );
    }

    constexpr static inline linktype_type
    get_type( link_type sides )
    {
      return 0;
    }

    constexpr static inline side_type
    opposite_side( side_type side )
    {
      return side;
    }

    constexpr static inline side_type
    get_dummy_side( )
    {
      return dummy_side;
    }

    constexpr static inline link_type
    get_dummy_link( )
    {
      return DirectedGraphBaseTrait::merge_sides(
          DirectedGraphBaseTrait::get_dummy_side(),
          DirectedGraphBaseTrait::get_dummy_side() );
    }
  };  /* --- end of template class DirectedGraphBaseTrait --- */

  /**
   *  @brief  Directed graph trait specialized by implementation (`TSpec`).
   */
  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class DirectedGraphTrait;

  template< uint8_t ...TWidths >
  class DirectedGraphTrait< Dynamic, Bidirected, TWidths... >
    : public GraphBaseTrait< Dynamic, TWidths... >,
      public DirectedGraphBaseTrait< Bidirected > {
  private:
    using spec_type = Dynamic;
    using dir_type = Bidirected;
    using graph_type = GraphBaseTrait< spec_type, TWidths... >;
    using base_type = DirectedGraphBaseTrait< dir_type >;
  public:
    using typename graph_type::id_type;
    using typename graph_type::offset_type;
    using typename graph_type::rank_type;
    using typename graph_type::nodes_type;
    using typename graph_type::rank_map_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;
    using adjs_type = std::vector< side_type >;

    struct hash_side {
      inline std::size_t
      operator()( side_type const& side ) const
      {
        auto hash = std::hash< id_type >{}( side.first );
        return side.second ? ~hash : hash;
      }
    };  /* --- end of struct hash_side --- */

    struct hash_link {
      inline std::size_t
      operator()( link_type const& sides ) const
      {
        auto hash1 = hash_side{}( base_type::from_side( sides ) );
        auto hash2 = hash_side{}( base_type::to_side( sides ) );
        return hash1 ^ hash2;
      }
    };  /* --- end of struct hash_link --- */

    using adj_map_type = google::sparse_hash_map< side_type, adjs_type, hash_side >;

    static inline void
    init_adj_map( adj_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( base_type::get_dummy_side() );
    }
  };  /* --- end of template class DirectedGraphTrait --- */

  template< uint8_t ...TWidths >
  class DirectedGraphTrait< Dynamic, Directed, TWidths... >
    : public GraphBaseTrait< Dynamic, TWidths... >,
      public DirectedGraphBaseTrait< Directed > {
  private:
    using spec_type = Dynamic;
    using dir_type = Directed;
    using graph_type = GraphBaseTrait< spec_type, TWidths... >;
    using base_type = DirectedGraphBaseTrait< dir_type >;
  public:
    using typename graph_type::id_type;
    using typename graph_type::offset_type;
    using typename graph_type::rank_type;
    using typename graph_type::nodes_type;
    using typename graph_type::rank_map_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;
    using adjs_type = nodes_type;
    using hash_side = std::hash< side_type >;

    struct hash_link {
      inline std::size_t
      operator()( link_type const& sides ) const
      {
        auto hash1 = hash_side{}( base_type::from_side( sides ) );
        auto hash2 = hash_side{}( base_type::to_side( sides ) );
        return hash1 ^ hash2;
      }
    };  /* --- end of struct hash_link --- */

    using adj_map_type = google::sparse_hash_map< side_type, adjs_type >;

    static inline void
    init_adj_map( adj_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( base_type::get_dummy_side() );
    }
  };  /* --- end of template class DirectedGraphTrait --- */

  template< typename TSpec, typename TDir = Bidirected, uint8_t ...TWidths >
  class DirectedGraph;

  template< typename TSpec, uint8_t ...TWidths >
  class NodePropertyTrait;

  template< uint8_t ...TWidths >
  class NodePropertyTrait< Dynamic, TWidths... > {
  private:
    using spec_type = Dynamic;
    using trait_type = GraphBaseTrait< spec_type, TWidths... >;
  public:
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using rank_type = typename trait_type::rank_type;

    class Node {
    public:
      /* === TYPEDEFS === */
      using sequence_type = std::string;
      using name_type = std::string;
      /* === LIFECYCLE === */
      Node( sequence_type s, name_type n="" )  /* constructor */
        : sequence( s ), name( n ) { }
      Node( ) : Node( "", "" ) { }             /* constructor */
      /* === DATA MEMBERS === */
      sequence_type sequence;
      name_type name;
    };  /* --- end of class Node --- */

    using node_type = Node;
    using value_type = node_type;
    using sequence_type = typename value_type::sequence_type;
    using name_type = typename value_type::name_type;
    using container_type = std::vector< value_type >;
  };  /* --- end of template class NodePropertyTrait --- */

  template< typename TSpec, uint8_t ...TWidths >
  class NodeProperty;

  template< typename TSpec, uint8_t ...TWidths >
  using DefaultNodeProperty = NodeProperty< TSpec, TWidths... >;

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
        : overlap( overlap_ ) { }
      /* === DATA MEMBERS === */
      offset_type overlap;
    };  /* --- end of class Edge --- */

    using edge_type = Edge;
    using key_type = link_type;
    using value_type = edge_type;
    using container_type = google::sparse_hash_map< key_type, value_type,
                                                    typename trait_type::hash_link >;

    static inline void
    init_container( container_type& c )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //c.set_empty_key( trait_type::get_dummy_link( ) );
    }
  };  /* --- end of template class EdgePropertyTrait --- */

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class EdgeProperty;

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  using DefaultEdgeProperty = EdgeProperty< TSpec, TDir, TWidths... >;

  template< typename TSpec, uint8_t TIdWidth, uint8_t TOffsetWidth >
  class GraphProperty;

  template< typename TSpec,
            template< class, uint8_t ... > class TNodeProp = DefaultNodeProperty,
            template< class, class, uint8_t ... > class TEdgeProp = DefaultEdgeProperty,
            uint8_t ...TWidths >
  class SeqGraph;

  template< typename TSpec, uint8_t ...TWidths >
  class DiSeqGraph;
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_BASE_HPP__ --- */
