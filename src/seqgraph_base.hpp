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
#include <sdsl/int_vector.hpp>

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
    using common_type = common_t< TIdWidth, TOffsetWidth >;
    using nodes_type = std::vector< id_type >;
    using size_type = typename nodes_type::size_type;
    using rank_type = typename nodes_type::size_type;
    using rank_map_type = google::sparse_hash_map< id_type, rank_type >;

    static inline void
    init_rank_map( rank_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( 0 );  // ID cannot be zero, so it can be used as empty key.
    }

    static inline void
    check_id( id_type id )
    {
      if ( id <= 0 ) throw std::runtime_error( "non-positive node ID" );
    }

    static inline void
    check_rank( rank_type )
    {
      if ( rank <= 0 ) throw std::runtime_error( "non-positive node rank");
    }
  };  /* --- end of template class GraphBaseTrait --- */

  /**
   *  @brief  Succinct graph trait.
   *
   *  The graph is stored as an integer vector with two given
   *  parameters: `np_padding` and `ep_padding`. These two parameters
   *  indicates the number of entries required by extra information about nodes
   *  and edges to be reserved and populated later with the information from
   *  node/edge properties in the graph.
   *
   *  GRAPH := NODES
   *  NODES := {HEADER, EDGES_TO, EDGES_FROM}
   *  HEADER = {id, outdegree, indegree, NODE_PROPS}
   *  EDGES_TO = {EDGE_TO, ...}
   *  EDGES_FROM = {EDGE_FROM, ...}
   *  EDGE_TO(*) = {id, type?, EDGE_PROPS}
   *  EDGE_FROM(*) = {id, type?, EDGE_PROPS}
   *  NODE_PROPS = {node_prop, ...}  // of size np_padding
   *  EDGE_PROPS = {edge_prop, ...}  // of size ep_padding
   *
   *  id = integer
   *  outdegree = integer
   *  indegree = integer
   *  type = integer
   *  node_prop = integer
   *  edge_prop = integer
   *
   *  (*) NOTE that the `type` field is only for Bidirected graphs and will be
   *  omitted for directed graphs.
   *
   *  NOTE: The node/edge property values should fit in the `common_type`.
   */
  template< uint8_t TIdWidth, uint8_t TOffsetWidth >
  class GraphBaseTrait< Succinct, TIdWidth, TOffsetWidth > {
  public:
    using id_type = integer_t< TIdWidth >;
    using offset_type = integer_t< TOffsetWidth >;
    using common_type = common_t< TIdWidth, TOffsetWidth >;
    using nodes_type = sdsl::int_vector< common_type::value >;
    using size_type = typename nodes_type::size_type;
    using rank_type = typename nodes_type::size_type;
    using bv_type = sdsl::bit_vector;
    using rank_map_type = typename bv_type::rank_1_type;
    using id_map_type = typename bv_type::select_1_type;
    using padding_type = unsigned char;

    constexpr static size_type HEADER_CORE_LEN = 3;
    constexpr static size_type OUTDEGREE_OFFSET = 1;
    constexpr static size_type INDEGREE_OFFSET = 2;

    static inline void
    check_id( id_type id )
    {
      if ( id <= 0 ) throw std::runtime_error( "non-positive node ID" );
    }

    static inline void
    check_rank( rank_type )
    {
      if ( rank <= 0 ) throw std::runtime_error( "non-positive node rank");
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
  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class DirectedGraphBaseTrait;

  template< typename TSpec, uint8_t ...TWidths >
  class DirectedGraphBaseTrait< TSpec, Bidirected, TWidths... > {
  private:
    using spec_type = TSpec;
    using graph_type = GraphBaseTrait< spec_type, TWidths... >;
    using id_type = graph_type::id_type;
    using sidetype_type = bool;
  public:
    using side_type = std::pair< id_type, sidetype_type >;
    using link_type = std::tuple< id_type, sidetype_type, id_type, sidetype_type >;
    using linktype_type = unsigned char;

    constexpr static side_type DUMMY_SIDE = { 0, false };
    constexpr static linktype_type DEFAULT_LINKTYPE =
        DirectedGraphBaseTrait::get_default_linktype();

    /* === ID === */
    constexpr static inline id_type
    from_id( link_type sides )
    {
      return std::get<0>( sides );
    }

    constexpr static inline id_type
    to_id( link_type sides )
    {
      return std::get<2>( sides );
    }

    constexpr static inline id_type
    id_of( side_type side )
    {
      return side.first;
    }

    /* === Side === */
    constexpr static inline side_type
    from_side( link_type sides )
    {
      return side_type( DirectedGraphBaseTrait::from_id( sides ),
                        DirectedGraphBaseTrait::from_sidetype( sides ) );
    }

    constexpr static inline side_type
    from_side( id_type id, linktype_type type )
    {
      assert( DirectedGraphBaseTrait::is_valid( type ) );
      return side_type( id, from_sidetype( type ) );
    }

    constexpr static inline side_type
    to_side( link_type sides )
    {
      return side_type( DirectedGraphBaseTrait::to_id( sides ),
                        DirectedGraphBaseTrait::to_sidetype( sides ) );
    }

    constexpr static inline side_type
    to_side( id_type id, linktype_type type )
    {
      assert( DirectedGraphBaseTrait::is_valid( type ) );
      return side_type( id, to_sidetype( type ) );
    }

    constexpr static inline side_type
    start_side( id_type id )
    {
      return side_type( id, DirectedGraphBaseTrait::start_sidetype() );
    }

    constexpr static inline side_type
    end_side( id_type id )
    {
      return side_type( id, DirectedGraphBaseTrait::end_sidetype() );
    }

    constexpr static inline side_type
    opposite_side( side_type side )
    {
      return side_type(
          DirectedGraphBaseTrait::id_of( side ),
          DirectedGraphBaseTrait::opposite_side(
              DirectedGraphBaseTrait::sidetype_of( side ) ) );
    }

    constexpr static inline side_type
    get_dummy_side( )
    {
      return DUMMY_SIDE;
    }

    template< typename TCallback >
    static inline bool
    for_each_side( id_type id, TCallback callback )
    {
      side_type side = { id, false };
      if ( !callback( side ) ) return false;
      if ( !callback( DirectedGraphBaseTrait::opposite_side( side ) ) ) return false;
      return true;
    }

    /* === Link === */
    constexpr static inline link_type
    make_link( side_type from, side_type to )
    {
      return link_type( DirectedGraphBaseTrait::id_of( from ),
                        DirectedGraphBaseTrait::sidetype_of( from ),
                        DirectedGraphBaseTrait::id_of( to ),
                        DirectedGraphBaseTrait::sidetype_of( to ) );
    }

    constexpr static inline link_type
    get_dummy_link( )
    {
      return DirectedGraphBaseTrait::make_link(
          DirectedGraphBaseTrait::get_dummy_side(),
          DirectedGraphBaseTrait::get_dummy_side() );
    }

    /* === Link type === */
    constexpr static inline linktype_type
    get_default_linktype( )
    {
      return DirectedGraphBaseTrait::_linktype(
          DirectedGraphBaseTrait::end_sidetype(),
          DirectedGraphBaseTrait::start_sidetype() );
    }

    constexpr static inline linktype_type
    linktype( side_type from, side_type to )
    {
      return DirectedGraphBaseTrait::_linktype(
          DirectedGraphBaseTrait::sidetype_of( from ),
          DirectedGraphBaseTrait::sidetype_of( to ) );
    }

    constexpr static inline linktype_type
    linktype( link_type sides )
    {
      return DirectedGraphBaseTrait::_linktype(
          DirectedGraphBaseTrait::from_sidetype( sides ),
          DirectedGraphBaseTrait::to_sidetype( sides ) );
    }

    constexpr static inline bool
    is_valid( linktype_type type )
    {
      return 0 <= type && type <= 3;
    }

    constexpr static inline bool
    is_valid_from( side_type from, linktype_type type )
    {
      return DirectedGraphBaseTrait::sidetype_of( from ) ==
          DirectedGraphBaseTrait::from_sidetype( type );
    }

    constexpr static inline bool
    is_valid_to( side_type to, linktype_type type )
    {
      return DirectedGraphBaseTrait::sidetype_of( to ) ==
          DirectedGraphBaseTrait::to_sidetype( type );
    }

    static inline void
    check_linktype( linktype_type type )
    {
      if ( !DirectedGraphBaseTrait::is->is_valid( type ) )
        throw std::runtime_error( "invalid link type" );
    }

  private:
    /* === Side type === */
    constexpr static inline sidetype_type
    start_sidetype( )
    {
      return false;
    }

    constexpr static inline sidetype_type
    end_sidetype( )
    {
      return true;
    }

    constexpr static inline sidetype_type
    from_sidetype( link_type sides )
    {
      return std::get<1>( sides );
    }

    constexpr static inline sidetype_type
    to_sidetype( link_type sides )
    {
      return std::get<3>( sides );
    }

    constexpr static inline sidetype_type
    sidetype_of( side_type side )
    {
      return side.second;
    }

    constexpr static inline side_type
    opposite_side( sidetype_type stype )
    {
      return !stype;
    }

    /* === Link type <-> Side type === */
    constexpr static inline sidetype_type
    from_sidetype( linktype_type type )
    {
      assert( DirectedGraphBaseTrait::is_valid( type ) );
      return type >> 1;
    }

    constexpr static inline sidetype_type
    to_sidetype( linktype_type type )
    {
      assert( DirectedGraphBaseTrait::is_valid( type ) );
      return type % 2;
    }

    constexpr static inline linktype_type
    _linktype( sidetype_type from, sidetype_type to )
    {
      return static_cast< linktype_type >( from )*2 + static_cast< linktype_type >( to );
    }
  };  /* --- end of template class DirectedGraphBaseTrait --- */

  template< typename TSpec, uint8_t ...TWidths >
  class DirectedGraphBaseTrait< TSpec, Directed, TWidths... > {
  private:
    using spec_type = TSpec;
    using graph_type = GraphBaseTrait< TSpec, TWidths... >;
    using id_type = graph_type::id_type;
  public:
    using side_type = id_type;
    using link_type = std::pair< side_type, side_type >;
    using linktype_type = unsigned char;

    constexpr static side_type DUMMY_SIDE = 0;
    constexpr static linktype_type DEFAULT_LINKTYPE = 0;

    /* === ID === */
    constexpr static inline id_type
    from_id( link_type sides )
    {
      return static_cast< id_type >( sides.first );
    }

    constexpr static inline id_type
    to_id( link_type sides )
    {
      return static_cast< id_type >( sides.second );
    }

    constexpr static inline id_type
    id_of( side_type side )
    {
      return static_cast< id_type >( side );
    }

    /* === Side === */
    constexpr static inline side_type
    from_side( link_type sides )
    {
      return sides.first;
    }

    constexpr static inline side_type
    from_side( id_type id, linktype_type type )
    {
      assert( DirectedGraphBaseTrait::is_valid( type ) );
      return static_cast< side_type >( id );
    }

    constexpr static inline side_type
    to_side( link_type sides )
    {
      return sides.second;
    }

    constexpr static inline side_type
    to_side( id_type id, linktype_type type )
    {
      assert( DirectedGraphBaseTrait::is_valid( type ) );
      return static_cast< side_type >( id );
    }

    constexpr static inline side_type
    opposite_side( side_type side )
    {
      return side;
    }

    constexpr static inline side_type
    get_dummy_side( )
    {
      return DUMMY_SIDE;
    }

    template< typename TCallback >
    static inline bool
    for_each_side( id_type id, TCallback callback )
    {
      return callback( static_cast< side_type >( id ) );
    }

    /* === Link === */
    constexpr static inline link_type
    make_link( side_type from, side_type to )
    {
      return link_type( from, to );
    }

    constexpr static inline link_type
    get_dummy_link( )
    {
      return DirectedGraphBaseTrait::make_link(
          DirectedGraphBaseTrait::get_dummy_side(),
          DirectedGraphBaseTrait::get_dummy_side() );
    }

    /* === Link type === */
    constexpr static inline linktype_type
    get_default_linktype( )
    {
      return DEFAULT_LINKTYPE;
    }

    constexpr static inline linktype_type
    linktype( side_type, side_type )
    {
      return DirectedGraphBaseTrait::get_default_linktype();
    }

    constexpr static inline linktype_type
    linktype( link_type )
    {
      return DirectedGraphBaseTrait::get_default_linktype();
    }

    constexpr static inline bool
    is_valid( linktype_type type )
    {
      return type == 0;
    }

    constexpr static inline bool
    is_valid_from( side_type from, linktype_type type )
    {
      return true;
    }

    constexpr static inline bool
    is_valid_to( side_type to, linktype_type type )
    {
      return true;
    }

    static inline void
    check_linktype( linktype_type type )
    {
      if ( !DirectedGraphBaseTrait::is->is_valid( type ) )
        throw std::runtime_error( "invalid link type" );
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
      public DirectedGraphBaseTrait< Dynamic, Bidirected, TWidths... > {
  private:
    using spec_type = Dynamic;
    using dir_type = Bidirected;
    using graph_type = GraphBaseTrait< spec_type, TWidths... >;
    using base_type = DirectedGraphBaseTrait< spec_type, dir_type, TWidths... >;
  public:
    using typename graph_type::id_type;
    using typename graph_type::offset_type;
    using typename graph_type::common_type;
    using typename graph_type::nodes_type;
    using typename graph_type::size_type;
    using typename graph_type::rank_type;
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
      public DirectedGraphBaseTrait< Dynamic, Directed, TWidths... > {
  private:
    using spec_type = Dynamic;
    using dir_type = Directed;
    using graph_type = GraphBaseTrait< spec_type, TWidths... >;
    using base_type = DirectedGraphBaseTrait< spec_type, dir_type, TWidths... >;
  public:
    using typename graph_type::id_type;
    using typename graph_type::offset_type;
    using typename graph_type::common_type;
    using typename graph_type::nodes_type;
    using typename graph_type::size_type;
    using typename graph_type::rank_type;
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

  template< uint8_t ...TWidths >
  class DirectedGraphTrait< Succinct, Bidirected, TWidths... >
    : public GraphBaseTrait< Succinct, TWidths... >,
      public DirectedGraphBaseTrait< Succinct, Bidirected, TWidths... > {
  private:
    using spec_type = Succinct;
    using dir_type = Bidirected;
    using graph_type = GraphBaseTrait< spec_type, TWidths... >;
    using base_type = DirectedGraphBaseTrait< spec_type, dir_type, TWidths... >;
  public:
    using typename graph_type::id_type;
    using typename graph_type::offset_type;
    using typename graph_type::common_type;
    using typename graph_type::nodes_type;
    using typename graph_type::size_type;
    using typename graph_type::rank_type;
    using typename graph_type::bv_type;
    using typename graph_type::rank_map_type;
    using typename graph_type::id_map_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;

    constexpr static size_type EDGE_CORE_LEN = 2;
  };  /* --- end of template class DirectedGraphTrait --- */

  template< uint8_t ...TWidths >
  class DirectedGraphTrait< Succinct, Directed, TWidths... >
    : public GraphBaseTrait< Succinct, TWidths... >,
      public DirectedGraphBaseTrait< Succinct, Directed, TWidths... > {
  private:
    using spec_type = Succinct;
    using dir_type = Directed;
    using graph_type = GraphBaseTrait< spec_type, TWidths... >;
    using base_type = DirectedGraphBaseTrait< spec_type, dir_type, TWidths... >;
  public:
    using typename graph_type::id_type;
    using typename graph_type::offset_type;
    using typename graph_type::common_type;
    using typename graph_type::nodes_type;
    using typename graph_type::size_type;
    using typename graph_type::rank_type;
    using typename graph_type::bv_type;
    using typename graph_type::rank_map_type;
    using typename graph_type::id_map_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;

    constexpr static size_type EDGE_CORE_LEN = 1;
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
