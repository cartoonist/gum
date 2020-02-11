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
#include <sdsl/bit_vectors.hpp>

#include "stringset.hpp"
#include "basic_types.hpp"


namespace gum {
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
    using offset_type = uinteger_t< TOffsetWidth >;
    using common_type = common< TIdWidth, TOffsetWidth >;
    using value_type = typename common_type::type;
    using nodes_type = std::vector< id_type >;
    using size_type = typename nodes_type::size_type;
    using rank_type = typename nodes_type::size_type;
    using rank_map_type = google::sparse_hash_map< id_type, rank_type >;
    using string_type = std::string;  // for node and path names

    static inline void
    init_rank_map( rank_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( 0 );  // ID cannot be zero, so it can be used as empty key.
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
   *  GRAPH := {NODE, ...}
   *  NODE := {HEADER, EDGES_OUT, EDGES_IN}
   *  HEADER := {id, outdegree, indegree, NODE_PROPS}
   *  EDGES_OUT := {EDGE_OUT, ...}
   *  EDGES_IN := {EDGE_IN, ...}
   *  EDGE_OUT(*) := {id, type?, EDGE_PROPS}
   *  EDGE_IN(*) := {id, type?, EDGE_PROPS}
   *  NODE_PROPS := {node_prop, ...}  // of size np_padding
   *  EDGE_PROPS := {edge_prop, ...}  // of size ep_padding
   *
   *  id: integer
   *  outdegree: integer
   *  indegree: integer
   *  type: integer
   *  node_prop: integer
   *  edge_prop: integer
   *
   *  (*) NOTE that the `type` field is only for Bidirected graphs and will be
   *  omitted for directed graphs.
   *
   *  NOTE: The node/edge property values should fit in the `value_type`.
   */
  template< uint8_t TIdWidth, uint8_t TOffsetWidth >
  class GraphBaseTrait< Succinct, TIdWidth, TOffsetWidth > {
  public:
    using id_type = integer_t< TIdWidth >;
    using offset_type = uinteger_t< TOffsetWidth >;
    using common_type = common< TIdWidth, TOffsetWidth >;
    using value_type = typename common_type::type;
    using nodes_type = sdsl::int_vector< common_type::value >;
    using size_type = typename nodes_type::size_type;
    using rank_type = typename nodes_type::size_type;
    using bv_type = sdsl::bit_vector;
    using rank_map_type = typename bv_type::rank_1_type;
    using id_map_type = typename bv_type::select_1_type;
    using string_type = std::string;  // for node and path names
    using padding_type = unsigned char;

    constexpr static size_type HEADER_CORE_LEN = 3;
    constexpr static size_type OUTDEGREE_OFFSET = 1;
    constexpr static size_type INDEGREE_OFFSET = 2;

    static inline rank_type
    get_outdegree( nodes_type const& nodes, id_type id )
    {
      return nodes[ id + GraphBaseTrait::OUTDEGREE_OFFSET ];
    }

    static inline void
    set_outdegree( nodes_type& nodes, id_type id, rank_type value )
    {
      nodes[ id + GraphBaseTrait::OUTDEGREE_OFFSET ] = value;
    }

    static inline rank_type
    get_indegree( nodes_type const& nodes, id_type id )
    {
      return nodes[ id + GraphBaseTrait::INDEGREE_OFFSET ];
    }

    static inline void
    set_indegree( nodes_type& nodes, id_type id, rank_type value )
    {
      nodes[ id + GraphBaseTrait::INDEGREE_OFFSET ] = value;
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
    using id_type = typename graph_type::id_type;
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
      return side_type( id, DirectedGraphBaseTrait::to_sidetype( type ) );
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

    static inline bool
    for_each_side( id_type id, std::function< bool( side_type ) > callback )
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
    make_link( id_type from_id, id_type to_id, linktype_type type )
    {
      return link_type( from_id,
                        DirectedGraphBaseTrait::from_sidetype( type ),
                        to_id,
                        DirectedGraphBaseTrait::to_sidetype( type ) );
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
    is_from_start( link_type sides )
    {
      return DirectedGraphBaseTrait::from_sidetype( sides ) ==
          DirectedGraphBaseTrait::start_sidetype();
    }

    constexpr static inline bool
    is_to_end( link_type sides )
    {
      return DirectedGraphBaseTrait::to_sidetype( sides ) ==
          DirectedGraphBaseTrait::end_sidetype();
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

    constexpr static inline sidetype_type
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
    using id_type = typename graph_type::id_type;
  public:
    using side_type = std::tuple< id_type >;
    using link_type = std::pair< side_type, side_type >;
    using linktype_type = unsigned char;

    constexpr static side_type DUMMY_SIDE =  { 0 };
    constexpr static linktype_type DEFAULT_LINKTYPE = 0;

    /* === ID === */
    constexpr static inline id_type
    from_id( link_type sides )
    {
      return std::get<0>( sides.first );
    }

    constexpr static inline id_type
    to_id( link_type sides )
    {
      return std::get<0>( sides.second );
    }

    constexpr static inline id_type
    id_of( side_type side )
    {
      return std::get<0>( side );
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
      return side_type( id );
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
      return side_type( id );
    }

    constexpr static inline side_type
    start_side( id_type id )
    {
      return side_type( id );
    }

    constexpr static inline side_type
    end_side( id_type id )
    {
      return side_type( id );
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

    static inline bool
    for_each_side( id_type id, std::function< bool( side_type ) > callback )
    {
      return callback( side_type( id ) );
    }

    /* === Link === */
    constexpr static inline link_type
    make_link( side_type from, side_type to )
    {
      return link_type( from, to );
    }

    constexpr static inline link_type
    make_link( id_type from_id, id_type to_id, linktype_type type )
    {
      return link_type( DirectedGraphBaseTrait::from_side( from_id, type ),
                        DirectedGraphBaseTrait::to_side( to_id, type ) );
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
    is_from_start( link_type sides )
    {
      return false;
    }

    constexpr static inline bool
    is_to_end( link_type sides )
    {
      return false;
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
    using typename graph_type::value_type;
    using typename graph_type::nodes_type;
    using typename graph_type::size_type;
    using typename graph_type::rank_type;
    using typename graph_type::rank_map_type;
    using typename graph_type::string_type;
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
    using typename graph_type::value_type;
    using typename graph_type::nodes_type;
    using typename graph_type::size_type;
    using typename graph_type::rank_type;
    using typename graph_type::rank_map_type;
    using typename graph_type::string_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;
    using adjs_type = std::vector< side_type >;

    struct hash_side {
      inline std::size_t
      operator()( side_type const& side ) const
      {
        return std::hash< id_type >{}( std::get<0>( side ) );
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
    using typename graph_type::value_type;
    using typename graph_type::nodes_type;
    using typename graph_type::size_type;
    using typename graph_type::rank_type;
    using typename graph_type::bv_type;
    using typename graph_type::rank_map_type;
    using typename graph_type::id_map_type;
    using typename graph_type::string_type;
    using typename graph_type::padding_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;
    using adjs_type = std::vector< side_type >;

    constexpr static size_type EDGE_CORE_LEN = 2;
    constexpr static size_type ADJ_ID_OFFSET = 0;
    constexpr static size_type ADJ_LINKTYPE_OFFSET = 1;

    /**
     *  @brief  Get the ID of the adjacent node from nodes array.
     *
     *  @param  nodes Reference to nodes array.
     *  @param  pos Start position of the edge entry.
     */
    static inline id_type
    get_adj_id( nodes_type const& nodes, size_type pos )
    {
      return nodes[ pos + DirectedGraphTrait::ADJ_ID_OFFSET ];
    }

    /**
     *  @brief  Set the ID of the adjacent node from nodes array.
     *
     *  @param  nodes Reference to nodes array.
     *  @param  pos Start position of the edge entry.
     *  @param  value The value to be set.
     */
    static inline void
    set_adj_id( nodes_type& nodes, size_type pos, id_type value )
    {
      nodes[ pos + DirectedGraphTrait::ADJ_ID_OFFSET ] = value;
    }

    /**
     *  @brief  Get the type of the link to the adjacent node from nodes array.
     *
     *  @param  nodes Reference to nodes array.
     *  @param  pos Start position of the edge entry.
     */
    static inline linktype_type
    get_adj_linktype( nodes_type const& nodes, size_type pos )
    {
      return nodes[ pos + DirectedGraphTrait::ADJ_LINKTYPE_OFFSET ];
    }

    /**
     *  @brief  Set the type of the link to the adjacent node from nodes array.
     *
     *  @param  nodes Reference to nodes array.
     *  @param  pos Start position of the edge entry.
     *  @param  value The value to be set.
     */
    static inline void
    set_adj_linktype( nodes_type& nodes, size_type pos, linktype_type value )
    {
      nodes[ pos + DirectedGraphTrait::ADJ_LINKTYPE_OFFSET ] = value;
    }
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
    using typename graph_type::value_type;
    using typename graph_type::nodes_type;
    using typename graph_type::size_type;
    using typename graph_type::rank_type;
    using typename graph_type::bv_type;
    using typename graph_type::rank_map_type;
    using typename graph_type::id_map_type;
    using typename graph_type::string_type;
    using typename graph_type::padding_type;
    using typename base_type::side_type;
    using typename base_type::link_type;
    using typename base_type::linktype_type;
    using adjs_type = std::vector< side_type >;

    constexpr static size_type EDGE_CORE_LEN = 1;
    constexpr static size_type ADJ_ID_OFFSET = 0;

    /**
     *  @brief  Get the ID of the adjacent node from nodes array.
     *
     *  @param  nodes Reference to nodes array.
     *  @param  pos Start position of the edge entry.
     */
    static inline id_type
    get_adj_id( nodes_type const& nodes, size_type pos )
    {
      return nodes[ pos + DirectedGraphTrait::ADJ_ID_OFFSET ];
    }

    /**
     *  @brief  Set the ID of the adjacent node from nodes array.
     *
     *  @param  nodes Reference to nodes array.
     *  @param  pos Start position of the edge entry.
     *  @param  value The value to be set.
     */
    static inline void
    set_adj_id( nodes_type& nodes, size_type pos, id_type value )
    {
      nodes[ pos + DirectedGraphTrait::ADJ_ID_OFFSET ] = value;
    }

    /**
     *  @brief  Get the type of the link to the adjacent node from nodes array.
     *
     *  @param  nodes Reference to nodes array.
     *  @param  pos Start position of the edge entry.
     */
    static inline linktype_type
    get_adj_linktype( nodes_type const&, size_type )
    {
      return base_type::get_default_linktype();
    }

    /**
     *  @brief  Set the type of the link to the adjacent node from nodes array.
     *
     *  @param  nodes Reference to nodes array.
     *  @param  pos Start position of the edge entry.
     */
    static inline void
    set_adj_linktype( nodes_type&, size_type, linktype_type )
    {
      return;  /* NOOP */
    }
  };  /* --- end of template class DirectedGraphTrait --- */

  template< typename TSpec, typename TDir = Bidirected, uint8_t ...TWidths >
  class DirectedGraph;

  template< typename TSequence, typename TString >
  class Node {
  public:
    /* === TYPEDEFS === */
    using sequence_type = TSequence;
    using string_type = TString;
    /* === LIFECYCLE === */
    Node( sequence_type s, string_type n="" )  /* constructor */
      : sequence( std::move( s ) ), name( std::move( n ) ) { }
    Node( ) : Node( "", "" ) { }             /* constructor */
    /* === DATA MEMBERS === */
    sequence_type sequence;
    string_type name;
  };  /* --- end of class Node --- */

  template< typename TSpec, uint8_t ...TWidths >
  class NodePropertyTrait;

  template< typename TNodeProp, typename TContainer, typename TValue >
  class SequenceProxyContainer
    : public RandomAccessProxyContainer< TContainer, TValue > {
  public:
    using node_prop_type = TNodeProp;
    using container_type = TContainer;
    using value_type = TValue;
    using base_type = RandomAccessProxyContainer< container_type, value_type >;
    using typename base_type::size_type;
    using typename base_type::difference_type;
    using typename base_type::proxy_type;
    using typename base_type::function_type;
    using typename base_type::const_iterator;
    using typename base_type::const_reference;

    SequenceProxyContainer( node_prop_type const* npt,
                            container_type const& cnt )
      : base_type( &cnt,
                   []( proxy_type const& node ) -> value_type {
                     return node.sequence;
                   } ),
        npt_ptr( npt )
    { }

    inline typename value_type::size_type
    length_sum( ) const
    {
      return this->npt_ptr->get_sequences_len_sum( );
    }

    node_prop_type const* npt_ptr;
  };  /* --- end of template class SequenceProxyContainer --- */

  template< typename TNodeProp, typename TContainer, typename TValue >
  class NameProxyContainer
    : public RandomAccessProxyContainer< TContainer, TValue > {
  public:
    using node_prop_type = TNodeProp;
    using container_type = TContainer;
    using value_type = TValue;
    using base_type = RandomAccessProxyContainer< container_type, value_type >;
    using typename base_type::size_type;
    using typename base_type::difference_type;
    using typename base_type::proxy_type;
    using typename base_type::function_type;
    using typename base_type::const_iterator;
    using typename base_type::const_reference;

    NameProxyContainer( node_prop_type const* npt,
                        container_type const& cnt )
      : base_type( &cnt,
                   []( proxy_type const& node ) -> value_type {
                     return node.name;
                   } ),
        npt_ptr( npt )
    { }

    inline typename value_type::size_type
    length_sum( ) const
    {
      return this->npt_ptr->get_names_len_sum( );
    }

    node_prop_type const* npt_ptr;
  };  /* --- end of template class NameProxyContainer --- */

  template< uint8_t ...TWidths >
  class NodePropertyTrait< Dynamic, TWidths... > {
  private:
    using spec_type = Dynamic;
    using trait_type = GraphBaseTrait< spec_type, TWidths... >;
  public:
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using rank_type = typename trait_type::rank_type;
    using sequence_type = std::string;
    using string_type = typename trait_type::string_type;
    using node_type = Node< sequence_type, string_type >;
    using value_type = node_type;
    using container_type = std::vector< value_type >;
    using size_type = typename container_type::size_type;
    using const_reference = typename container_type::const_reference;
    using const_iterator = typename container_type::const_iterator;

    template< typename TNodeProp >
    using sequence_proxy_container =
        SequenceProxyContainer< TNodeProp, container_type, sequence_type >;

    template< typename TNodeProp >
    using name_proxy_container =
        NameProxyContainer< TNodeProp, container_type, string_type >;
  };  /* --- end of template class NodePropertyTrait --- */

  template< uint8_t ...TWidths >
  class NodePropertyTrait< Succinct, TWidths... > {
  private:
    using spec_type = Succinct;
    using trait_type = GraphBaseTrait< spec_type, TWidths... >;
  public:
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using rank_type = typename trait_type::rank_type;
    using alphabet_type = gum::DNA5;
    using sequenceset_type = StringSet< alphabet_type >;
    using stringset_type = StringSet< Char >;
    using sequence_type = typename sequenceset_type::value_type;
    using string_type = typename trait_type::string_type;
    using char_type = typename alphabet_type::char_type;
    using node_type = Node< sequence_type, string_type >;
    using value_type = node_type;
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
    using dir_type = TDir;
    using trait_type = DirectedGraphTrait< spec_type, dir_type, TWidths... >;
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

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class GraphPropertyTrait;

  /**
   *  @brief  PathBase class.
   *
   *  The base class for `Path` defining the methods for encoding and decoding
   *  ID and orientation of nodes in a path.
   *
   *  NOTE: `value_type` stores the encoded value of a node ID and corresponding
   *  orientation. The first bit of encoded value (MSB) denotes the orientation
   *  of the node. It is set if the node is reversed in the path. Other bits are
   *  use for storing the node ID.
   *
   *  NOTE: All IDs share the same type `id_type`; both paths and nodes.
   */
  template< typename TId >
  class PathBase {
  public:
    /* === TYPEDEFS === */
    using id_type = TId;
    using value_type = std::make_unsigned_t< id_type >;

    constexpr static uint8_t VALUE_WIDTH = widthof< value_type >::value;

    /* === METHODS === */
    constexpr static inline value_type
    encode( id_type id, bool reversed )
    {
      value_type value = static_cast< value_type >( id );
      if ( reversed ) value |= PathBase::get_orientation_bit();
      return value;
    }

    constexpr static inline id_type
    id_of( value_type value )
    {
      return value & PathBase::get_orientation_mask();
    }

    constexpr static inline bool
    is_reverse( value_type value )
    {
      return value >> ( PathBase::VALUE_WIDTH - 1 );
    }

    constexpr static inline value_type
    get_orientation_bit( )
    {
      return 1UL << ( PathBase::VALUE_WIDTH - 1 );
    }

    constexpr static inline value_type
    get_orientation_mask( )
    {
      return ~( PathBase::get_orientation_bit() );
    }
  };  /* --- end of template class PathBase --- */

  template< typename TDir, uint8_t ...TWidths >
  class GraphPropertyTrait< Dynamic, TDir, TWidths... > {
  private:
    using spec_type = Dynamic;
    using dir_type = TDir;
    using trait_type = DirectedGraphTrait< spec_type, dir_type, TWidths... >;
  public:
    using id_type = typename trait_type::id_type;
    using rank_type = typename trait_type::rank_type;
    using offset_type = typename trait_type::offset_type;
    using common_type = typename trait_type::common_type;
    using string_type = typename trait_type::string_type;

    class Path : public PathBase< id_type > {
    public:
      /* === TYPEDEFS === */
      using base_type = PathBase< id_type >;
      using typename base_type::value_type;
      using container_type = std::vector< value_type >;
      using const_reference = typename container_type::const_reference;
      using const_iterator = typename container_type::const_iterator;
      using size_type = typename container_type::size_type;

      /* === LIFECYCLE === */
      Path( id_type id_, string_type name_="" )    /* constructor */
        : id( id_ ), name( std::move( name_ ) ) { }

      /* === ACCESSORS === */
      inline id_type
      get_id( ) const
      {
        return this->id;
      }

      inline string_type const&
      get_name( ) const
      {
        return this->name;
      }

      inline container_type const&
      get_nodes( ) const
      {
        return this->nodes;
      }

      /* === METHODS === */
      inline void
      add_node( id_type id, bool reversed=false )
      {
        this->nodes.push_back( base_type::encode( id, reversed ) );
      }

      inline void
      for_each_node( std::function< bool( id_type, bool ) > callback )
      {
        for ( auto it = this->nodes.begin(); it != this->nodes.end(); ++it ) {
          callback( base_type::id_of( *it ), base_type::is_reverse( *it ) );
        }
      }

      inline const_iterator
      begin( ) const
      {
        return this->nodes.begin();
      }

      inline const_iterator
      end( ) const
      {
        return this->nodes.end();
      }

      inline const_reference
      back( ) const
      {
        return this->nodes.back();
      }

      inline const_reference
      front( ) const
      {
        return this->nodes.front();
      }

      inline id_type
      id_of( value_type value ) const
      {
        return base_type::id_of( value );
      }

      inline bool
      is_reverse( value_type value ) const
      {
        return base_type::is_reverse( value );
      }

      inline rank_type
      size( ) const
      {
        return this->nodes.size();
      }

    private:
      /* === DATA MEMBERS === */
      id_type id;
      string_type name;
      container_type nodes;
    };  /* --- end of class Path --- */

    using path_type = Path;
    using value_type = path_type;
    using container_type = std::vector< value_type >;
    using const_reference = typename container_type::const_reference;
    using const_iterator = typename container_type::const_iterator;
    using size_type = typename container_type::size_type;
    using rank_map_type = google::sparse_hash_map< id_type, rank_type >;

    static inline void
    init_rank_map( rank_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( 0 );  // ID cannot be zero, so it can be used as empty key.
    }
  };  /* --- end of template class GraphPropertyTrait --- */

  /**
   *  @brief  Succinct graph property trait.
   *
   *  In the graph property class, paths are stored as an integer vector.
   *
   *  PROPERTY := {PATH, ...}
   *  PATH := {HEADER, NODES}
   *  HEADER := {id, plen, NAME}
   *  NAME := {start, nlen}
   *  NODES := {node, ...}
   *
   *  id: integer      // path ID
   *  plen: integer    // path length
   *  start: integer   // starting position of the path name
   *  nlen: integer    // length of the path name
   *  node: integer    // encoded value of node ID and orientation
   */
  template< typename TDir, uint8_t ...TWidths >
  class GraphPropertyTrait< Succinct, TDir, TWidths... > {
  private:
    using spec_type = Succinct;
    using dir_type = TDir;
    using trait_type = DirectedGraphTrait< spec_type, dir_type, TWidths... >;
  public:
    using id_type = typename trait_type::id_type;
    using rank_type = typename trait_type::rank_type;
    using offset_type = typename trait_type::offset_type;
    using common_type = typename trait_type::common_type;
    using string_type = typename trait_type::string_type;
    using stringsize_type = typename string_type::size_type;
    using value_type = typename common_type::type;
    using container_type = sdsl::int_vector< common_type::value >;
    using size_type = typename container_type::size_type;
    using bv_type = sdsl::bit_vector;
    using rank_map_type = typename bv_type::rank_1_type;
    using id_map_type = typename bv_type::select_1_type;

    class Path : public PathBase< id_type > {
    public:
      /* === TYPEDEFS === */
      using base_type = PathBase< id_type >;
      using typename base_type::value_type;
      using const_reference = typename container_type::const_reference;
      using const_iterator = typename container_type::const_iterator;

      /* === LIFECYCLE === */
      Path( id_type id_,
            typename string_type::const_iterator name_itr_, stringsize_type name_len_,
            const_iterator nodes_itr_, size_type nodes_len_ )
        : id( id_ ), name_itr( name_itr_ ), name_len( name_len_ ),
          nodes_itr( nodes_itr_ ), nodes_len( nodes_len_ )
      { }

      /* === ACCESSORS === */
      inline id_type
      get_id( ) const
      {
        return this->id;
      }

      inline string_type
      get_name( ) const
      {
        string_type name( this->name_len, '\0' );
        std::copy( this->name_itr, this->name_itr+this->name_len, name.begin() );
        return name;
      }

      inline container_type
      get_nodes( ) const
      {
        container_type nodes( nodes_len, 0 );
        std::copy( this->nodes_itr, this->nodes_itr+this->nodes_len, nodes.begin() );
        return nodes;
      }

      /* === METHODS === */
      inline void
      for_each_node( std::function< bool( id_type, bool ) > callback )
      {
        auto nodes_end = this->end();
        for ( auto it = this->begin(); it != nodes_end; ++it ) {
          callback( base_type::id_of( *it ), base_type::is_reverse( *it ) );
        }
      }

      inline const_iterator
      begin( ) const
      {
        return this->nodes_itr;
      }

      inline const_iterator
      end( ) const
      {
        return this->nodes_itr + this->nodes_len;
      }

      inline const_reference
      back( ) const
      {
        return *( this->nodes_itr + this->nodes_len - 1 );
      }

      inline const_reference
      front( ) const
      {
        return *this->nodes_itr;
      }

      inline id_type
      id_of( value_type value ) const
      {
        return base_type::id_of( value );
      }

      inline bool
      is_reverse( value_type value ) const
      {
        return base_type::is_reverse( value );
      }

      inline rank_type
      size( ) const
      {
        return this->nodes_len;
      }

    private:
      /* === DATA MEMBERS === */
      id_type id;
      typename string_type::const_iterator name_itr;
      stringsize_type name_len;
      const_iterator nodes_itr;
      size_type nodes_len;
    };  /* --- end of class Path --- */

    using path_type = Path;

    constexpr static size_type HEADER_ENTRY_LEN = 4;
    constexpr static size_type PATH_LEN_OFFSET = 1;
    constexpr static size_type NAME_POS_OFFSET = 2;
    constexpr static size_type NAME_LEN_OFFSET = 3;

    static inline rank_type
    get_path_length( container_type const& paths, id_type id )
    {
      return paths[ id + GraphPropertyTrait::PATH_LEN_OFFSET ];
    }

    static inline void
    set_path_length( container_type& paths, id_type id, rank_type value )
    {
      paths[ id + GraphPropertyTrait::PATH_LEN_OFFSET ] = value;
    }

    static inline stringsize_type
    get_name_position( container_type const& paths, id_type id )
    {
      return paths[ id + GraphPropertyTrait::NAME_POS_OFFSET ];
    }

    static inline void
    set_name_position( container_type& paths, id_type id, stringsize_type value )
    {
      paths[ id + GraphPropertyTrait::NAME_POS_OFFSET ] = value;
    }

    static inline stringsize_type
    get_name_length( container_type const& paths, id_type id )
    {
      return paths[ id + GraphPropertyTrait::NAME_LEN_OFFSET ];
    }

    static inline void
    set_name_length( container_type& paths, id_type id, stringsize_type value )
    {
      paths[ id + GraphPropertyTrait::NAME_LEN_OFFSET ] = value;
    }
  };  /* --- end of template class GraphPropertyTrait --- */

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class GraphProperty;

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  using DefaultGraphProperty = GraphProperty< TSpec, TDir, TWidths... >;

  template< typename TSpec,
            template< class, uint8_t ... > class TNodeProp = DefaultNodeProperty,
            template< class, class, uint8_t ... > class TEdgeProp = DefaultEdgeProperty,
            template< class, class, uint8_t ... > class TGraphProp = DefaultGraphProperty,
            uint8_t ...TWidths >
  class SeqGraph;

  template< typename TSpec, uint8_t ...TWidths >
  class DiSeqGraph;
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_SEQGRAPH_BASE_HPP__ --- */
