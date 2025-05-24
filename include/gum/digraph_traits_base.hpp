/**
 *    @file  digraph_traits_base.hpp
 *   @brief  Directed graph basic traits definitions
 *
 *  This header file includes traits definitions specifying edges in
 *  bi/directed graphs.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  16:40
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_DIGRAPH_TRAITS_BASE_HPP__
#define GUM_DIGRAPH_TRAITS_BASE_HPP__

#include <cassert>
#include <tuple>
#include <type_traits>

#include "graph_traits_base.hpp"


namespace gum {
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

    constexpr static inline linktype_type
    get_default_linktype( )
    {
      return DirectedGraphBaseTrait::_linktype(
          DirectedGraphBaseTrait::end_sidetype(),
          DirectedGraphBaseTrait::start_sidetype() );
    }

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

    constexpr static inline bool
    is_start_side( side_type side )
    {
      return DirectedGraphBaseTrait::sidetype_of( side ) ==
          DirectedGraphBaseTrait::start_sidetype();
    }

    constexpr static inline bool
    is_end_side( side_type side )
    {
      return DirectedGraphBaseTrait::sidetype_of( side ) ==
          DirectedGraphBaseTrait::end_sidetype();
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
      static_assert( std::is_invocable_r_v< bool, TCallback, side_type >, "received a non-invocable as callback" );

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
    is_from_start( linktype_type type )
    {
      return DirectedGraphBaseTrait::from_sidetype( type ) ==
          DirectedGraphBaseTrait::start_sidetype();
    }

    constexpr static inline bool
    is_to_end( link_type sides )
    {
      return DirectedGraphBaseTrait::to_sidetype( sides ) ==
          DirectedGraphBaseTrait::end_sidetype();
    }

    constexpr static inline bool
    is_to_end( linktype_type type )
    {
      return DirectedGraphBaseTrait::to_sidetype( type ) ==
          DirectedGraphBaseTrait::end_sidetype();
    }

    constexpr static inline bool
    is_valid( linktype_type type )
    {
      return 0 <= ( int )( type ) && type <= 3;
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

    constexpr static inline bool
    is_start_side( side_type side )
    {
      return true;
    }

    constexpr static inline bool
    is_end_side( side_type side )
    {
      return true;
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
      static_assert( std::is_invocable_r_v< bool, TCallback, side_type >, "received a non-invocable as callback" );

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
    is_from_start( linktype_type type )
    {
      return false;
    }

    constexpr static inline bool
    is_to_end( link_type sides )
    {
      return false;
    }

    constexpr static inline bool
    is_to_end( linktype_type type )
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
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_DIGRAPH_TRAITS_BASE_HPP__ --- */
