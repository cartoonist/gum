/**
 *    @file  graph_traits_succinct.hpp
 *   @brief  Traits definitions for Succinct graphs
 *
 *  This is a header file for Succinct graph traits definition.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  16:23
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_GRAPH_TRAITS_SUCCINCT_HPP__
#define GUM_GRAPH_TRAITS_SUCCINCT_HPP__

#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>

#include "basic_types.hpp"
#include "graph_traits_base.hpp"


namespace gum{
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
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_GRAPH_TRAITS_SUCCINCT_HPP__ --- */
