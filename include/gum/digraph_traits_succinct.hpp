/**
 *    @file  digraph_traits_succinct.hpp
 *   @brief  Traits definitions for Succinct bi/directed graphs
 *
 *  This header file includes traits definitions for Succinct bi/directed
 *  graphs.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  18:37
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_DIGRAPH_TRAITS_SUCCINCT_HPP__
#define GUM_DIGRAPH_TRAITS_SUCCINCT_HPP__

#include "graph_traits_succinct.hpp"
#include "digraph_traits_base.hpp"


namespace gum {
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
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DIGRAPH_TRAITS_SUCCINCT_HPP__ --- */
