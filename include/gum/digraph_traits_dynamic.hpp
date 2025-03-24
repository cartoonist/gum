/**
 *    @file  digraph_traits_dynamic.hpp
 *   @brief  Traits definitions for Dynamic bi/directed graphs
 *
 *  This header file includes traits definitions for Dynamic bi/directed
 *  graphs.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  18:17
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_DIGRAPH_TRAITS_DYNAMIC_HPP__
#define GUM_DIGRAPH_TRAITS_DYNAMIC_HPP__

#include "graph_traits_dynamic.hpp"
#include "digraph_traits_base.hpp"


namespace gum {
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
        auto hash = std::hash< id_type >{}( base_type::id_of( side ) );
        return side.second ? ~hash : hash;
      }
    };  /* --- end of struct hash_side --- */

    struct hash_link {
      inline std::size_t
      operator()( link_type const& sides ) const
      {
        return std::hash< id_type >{}( base_type::from_id( sides ) ) + base_type::to_id( sides );
      }
    };  /* --- end of struct hash_link --- */

    using adj_map_type = phmap::flat_hash_map< side_type, adjs_type, hash_side >;

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
        return std::hash< id_type >{}( base_type::id_of( side ) );
      }
    };  /* --- end of struct hash_side --- */

    struct hash_link {
      inline std::size_t
      operator()( link_type const& sides ) const
      {
        return std::hash< id_type >{}( base_type::from_id( sides ) ) + base_type::to_id( sides );
      }
    };  /* --- end of struct hash_link --- */

    using adj_map_type = phmap::flat_hash_map< side_type, adjs_type, hash_side >;

    static inline void
    init_adj_map( adj_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( base_type::get_dummy_side() );
    }
  };  /* --- end of template class DirectedGraphTrait --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DIGRAPH_TRAITS_DYNAMIC_HPP__ --- */
