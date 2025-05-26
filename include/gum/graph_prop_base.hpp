/**
 *    @file  graph_prop_base.hpp
 *   @brief  `GraphProperty` base definitions
 *
 *  This header file includes basic definitions related to `GraphProperty`
 *  class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  19:18
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_GRAPH_PROP_BASE_HPP__
#define GUM_GRAPH_PROP_BASE_HPP__

#include <type_traits>

#include "basic_types.hpp"


namespace gum {
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

    constexpr static inline void
    reverse( value_type& value )
    {
      value ^= PathBase::get_orientation_bit();
    }
  private:
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

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class GraphPropertyTrait;

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  class GraphProperty;

  template< typename TSpec, typename TDir, uint8_t ...TWidths >
  using DefaultGraphProperty = GraphProperty< TSpec, TDir, TWidths... >;
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_GRAPH_PROP_BASE_HPP__ --- */
