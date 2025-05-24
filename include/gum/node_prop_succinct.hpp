/**
 *    @file  node_prop_succinct.hpp
 *   @brief  `NodeProperty` Succinct specification
 *
 *  This header file includes Succinct specification of `NodeProperty` class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Mon Mar 24, 2025  20:32
 *  Organization:  Bielefeld University
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_NODE_PROP_SUCCINCT_HPP__
#define GUM_NODE_PROP_SUCCINCT_HPP__

#include "stringset.hpp"
#include "graph_traits_succinct.hpp"
#include "node_prop_base.hpp"


namespace gum {
  template< uint8_t ...TWidths >
  class NodePropertyTrait< Succinct, TWidths... > {
  private:
    using spec_type = Succinct;
    using trait_type = GraphBaseTrait< spec_type, TWidths... >;
  public:
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using rank_type = typename trait_type::rank_type;
    using alphabet_type = DNA5;
    using sequenceset_type = StringSet< alphabet_type >;
    using stringset_type = StringSet< Char >;
    using sequence_type = typename sequenceset_type::value_type;
    using seq_const_reference = typename sequenceset_type::const_reference;
    using seq_reference = typename sequenceset_type::reference;
    using string_type = typename trait_type::string_type;
    using str_const_reference = typename stringset_type::const_reference;
    using str_reference = typename stringset_type::reference;
    using char_type = typename alphabet_type::char_type;
    using node_type = Node< sequence_type, string_type >;
    using value_type = node_type;
  };  /* --- end of template class NodePropertyTrait --- */

  /**
   *  @brief  Node property class (succinct).
   *
   *  Represent data associated with each node, mainly node sequences.
   */
  template< uint8_t ...TWidths >
  class NodeProperty< Succinct, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Succinct;
    using trait_type = NodePropertyTrait< spec_type, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using rank_type = typename trait_type::rank_type;
    using alphabet_type = typename trait_type::alphabet_type;
    using sequenceset_type = typename trait_type::sequenceset_type;
    using stringset_type = typename trait_type::stringset_type;
    using seq_const_reference = typename trait_type::seq_const_reference;
    using seq_reference = typename trait_type::seq_reference;
    using str_const_reference = typename trait_type::str_const_reference;
    using str_reference = typename trait_type::str_reference;
    using sequence_type = typename trait_type::sequence_type;
    using string_type = typename trait_type::string_type;
    using char_type = typename trait_type::char_type;
    using node_type = typename trait_type::node_type;
    using value_type = typename trait_type::value_type;
    using container_type = NodeProperty;
    using size_type = std::size_t;
    using const_reference = value_type;  // returned by value since the object is always an rvalue
    using const_iterator = RandomAccessConstIterator< container_type >;
    using dynamic_type = NodeProperty< Dynamic, TWidths... >;
    using succinct_type = NodeProperty;

    /* === LIFECYCLE === */
    NodeProperty() = default;                                 /* constructor      */
    NodeProperty( dynamic_type const& other )
      : seqset( other.sequences() ), nameset( other.names() )
    { }

    NodeProperty( NodeProperty const& other ) = default;      /* copy constructor */
    NodeProperty( NodeProperty&& other ) noexcept = default;  /* move constructor */
    ~NodeProperty() noexcept = default;                       /* destructor       */

    /* === ACCESSORS === */
    inline container_type const&
    get_nodes( ) const
    {
      return *this;
    }

    inline typename sequence_type::size_type
    get_sequences_len_sum( ) const
    {
      return util::length_sum( this->seqset );
    }

    inline typename string_type::size_type
    get_names_len_sum( ) const
    {
      return util::length_sum( this->nameset );
    }

    /* === OPERATORS === */
    NodeProperty& operator=( NodeProperty const& other ) = default;      /* copy assignment operator */
    NodeProperty& operator=( NodeProperty&& other ) noexcept = default;  /* move assignment operator */

    inline NodeProperty&
    operator=( dynamic_type const& other )
    {
      this->seqset = sequenceset_type( other.sequences() );
      this->nameset = stringset_type( other.names() );
      return *this;
    }

    inline const_reference
    operator[]( size_type i ) const
    {
      return value_type( this->seqset[ i ], this->nameset[ i ] );
    }

    inline const_reference
    operator()( rank_type rank ) const
    {
      return ( *this )[ rank - 1 ];
    }

    /* === METHODS === */
    inline const_reference
    at( size_type i ) const
    {
      return value_type( this->seqset.at( i ), this->nameset.at( i ) );
    }

    inline const_iterator
    begin( ) const
    {
      return const_iterator( this, 0 );
    }

    inline const_iterator
    end( ) const
    {
      return const_iterator( this, this->size() );
    }

    inline const_reference
    back( ) const
    {
      return *( this->begin() );
    }

    inline const_reference
    front( ) const
    {
      return *( this->end() - 1 );
    }

    inline size_type
    size( ) const
    {
      assert( this->seqset.size() == this->nameset.size() );
      return this->seqset.size();
    }

    inline sequenceset_type const&
    sequences( ) const
    {
      return this->seqset;
    }

    inline stringset_type const&
    names( ) const
    {
      return this->nameset;
    }

    inline void
    clear( )
    {
      this->seqset.clear();
      this->nameset.clear();
    }

  private:
    /* === DATA MEMBERS === */
    sequenceset_type seqset;
    stringset_type nameset;
  };  /* --- end of template class NodeProperty --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_NODE_PROP_SUCCINCT_HPP__ --- */
