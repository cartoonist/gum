/**
 *    @file  edge_prop_dynamic.hpp
 *   @brief  `EdgeProperty` Dynamic specification
 *
 *  This header file includes Dynamic specification of `EdgeProperty` class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  20:26
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_EDGE_PROP_DYNAMIC_HPP__
#define GUM_EDGE_PROP_DYNAMIC_HPP__

#include "digraph_traits_dynamic.hpp"
#include "edge_prop_base.hpp"


namespace gum {
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
    using container_type
        = phmap::flat_hash_map< key_type, value_type,
                                typename trait_type::hash_link >;

    static inline void
    init_container( container_type& c )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //c.set_empty_key( trait_type::get_dummy_link( ) );
    }
  };  /* --- end of template class EdgePropertyTrait --- */

  /**
   *  @brief  Edge property class (dynamic).
   *
   *  Represent data associated with each edge, mainly directionality. Each data
   *  structure associated with each edge are stored in a hash map with node ID
   *  pairs as keys.
   */
  template< typename TDir, uint8_t ...TWidths >
  class EdgeProperty< Dynamic, TDir, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using dir_type = TDir;
    using trait_type = EdgePropertyTrait< spec_type, dir_type, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using link_type = typename trait_type::link_type;
    using edge_type = typename trait_type::edge_type;
    using key_type = typename trait_type::key_type;
    using value_type = typename trait_type::value_type;
    using container_type = typename trait_type::container_type;

    /* === LIFECYCLE === */
    EdgeProperty( )
    {                                         /* constructor      */
      trait_type::init_container( this->edges );
    }

    EdgeProperty( EdgeProperty const& other ) = default;      /* copy constructor */
    EdgeProperty( EdgeProperty&& other ) noexcept = default;  /* move constructor */
    ~EdgeProperty() noexcept = default;                       /* destructor       */

    /* === ACCESSORS === */
    inline container_type const&
    get_edges( ) const
    {
      return this->edges;
    }

    /* === OPERATORS === */
    EdgeProperty& operator=( EdgeProperty const& other ) = default;      /* copy assignment operator */
    EdgeProperty& operator=( EdgeProperty&& other ) noexcept = default;  /* move assignment operator */

    inline edge_type const&
    operator[]( key_type sides ) const
    {
      return this->edges.find( sides )->second;
    }

    /* === METHODS === */
    inline edge_type const&
    at( key_type sides ) const
    {
      auto found = this->edges.find( sides );
      if ( found == this->edges.end() ) throw std::runtime_error( "no such edge" );
      return found->second;
    }

    inline void
    add_edge( key_type sides, value_type edge )
    {
      this->edges[ sides ] = edge;
    }

    inline bool
    has_edge( key_type sides ) const
    {
      return this->edges.find( sides ) != this->edges.end();
    }

    inline void
    clear( )
    {
      this->edges.clear();
    }

  private:
    /* === DATA MEMBERS === */
    container_type edges;
  };  /* --- end of template class EdgeProperty --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_EDGE_PROP_DYNAMIC_HPP__ --- */
