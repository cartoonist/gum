/**
 *    @file  graph_traits_base.hpp
 *   @brief  Graph traits basic definitions
 *
 *  This is a base header file for graph traits definition.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  16:07
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_GRAPH_TRAITS_BASE_HPP__
#define GUM_GRAPH_TRAITS_BASE_HPP__

#include <cstdint>


namespace gum{
  /* Dynamic specialization tag. */
  struct Dynamic {};
  /* Succinct specialization tag. */
  struct Succinct {};
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

  template< typename TObject, typename ...TArgs >
  struct make_dynamic {
    using type = typename TObject::template dynamic_template< TArgs... >;
  };  /* --- end of template struct make_dynamic --- */

  template< typename TObject, typename ...TArgs >
  using make_dynamic_t = typename make_dynamic< TObject, TArgs... >::type;

  template< typename TObject, typename ...TArgs >
  struct make_succinct {
    using type = typename TObject::template succinct_template< TArgs... >;
  };  /* --- end of template struct make_succinct --- */

  template< typename TObject, typename ...TArgs >
  using make_succinct_t = typename make_succinct< TObject, TArgs... >::type;
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_GRAPH_TRAITS_BASE_HPP__ --- */
