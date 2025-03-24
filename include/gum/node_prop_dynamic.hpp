/**
 *    @file  node_prop_dynamic.hpp
 *   @brief  `NodeProperty` Dynamic specification
 *
 *  This header file includes Dynamic specification of `NodeProperty` class.
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

#ifndef GUM_NODE_PROP_DYNAMIC_HPP__
#define GUM_NODE_PROP_DYNAMIC_HPP__

#include "utils.hpp"
#include "graph_traits_dynamic.hpp"
#include "node_prop_base.hpp"
#include "iterators.hpp"


namespace gum {
  template< typename TContainer >
  struct SequenceProxyFunctor {
    using container_type = TContainer;
    using size_type = typename container_type::size_type;
    using proxy_type = decltype( std::declval< container_type >()[ std::declval< size_type >() ] );

    inline decltype(auto)
    operator()( proxy_type node ) const noexcept
    {
      return node.sequence;
    }
  };

  template< typename TNodeProp, typename TContainer >
  class SequenceProxyContainer
    : public RandomAccessProxyContainer< TContainer, SequenceProxyFunctor< TContainer > > {
  public:
    using node_prop_type = TNodeProp;
    using container_type = TContainer;
    using function_type = SequenceProxyFunctor< container_type >;
    using base_type = RandomAccessProxyContainer< container_type, function_type >;
    using typename base_type::size_type;
    using typename base_type::difference_type;
    using typename base_type::value_type;
    using typename base_type::proxy_type;
    using typename base_type::iterator;
    using typename base_type::const_iterator;
    using typename base_type::reference;
    using typename base_type::const_reference;

    SequenceProxyContainer( node_prop_type * npt,
                            container_type * cnt_ptr )
      : base_type( cnt_ptr, function_type{} ),
        npt_ptr( npt )
    { }

    inline typename value_type::size_type  // value_type == std::string
    length_sum( ) const
    {
      return this->npt_ptr->get_sequences_len_sum( );
    }

    node_prop_type * npt_ptr;
  };  /* --- end of template class SequenceProxyContainer --- */

  template< typename TContainer >
  struct NameProxyFunctor {
    using container_type = TContainer;
    using size_type = typename container_type::size_type;
    using proxy_type = decltype( std::declval< container_type >()[ std::declval< size_type >() ] );

    inline decltype(auto)
    operator()( proxy_type node ) const noexcept
    {
      return node.name;
    }
  };

  template< typename TNodeProp, typename TContainer >
  class NameProxyContainer
    : public RandomAccessProxyContainer< TContainer, NameProxyFunctor< TContainer > > {
  public:
    using node_prop_type = TNodeProp;
    using container_type = TContainer;
    using function_type = NameProxyFunctor< container_type >;
    using base_type = RandomAccessProxyContainer< container_type, function_type >;
    using typename base_type::size_type;
    using typename base_type::difference_type;
    using typename base_type::value_type;
    using typename base_type::proxy_type;
    using typename base_type::iterator;
    using typename base_type::const_iterator;
    using typename base_type::reference;
    using typename base_type::const_reference;

    NameProxyContainer( node_prop_type * npt,
                        container_type * cnt_ptr )
      : base_type( cnt_ptr, function_type{} ),
        npt_ptr( npt )
    { }

    inline typename value_type::size_type  // value_type == std::string
    length_sum( ) const
    {
      return this->npt_ptr->get_names_len_sum( );
    }

    node_prop_type * npt_ptr;
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
    using const_container_type = const std::vector< value_type >;
    using size_type = typename container_type::size_type;
    using const_reference = typename container_type::const_reference;
    using const_iterator = typename container_type::const_iterator;

    template< typename TNodeProp >
    using const_sequence_proxy_container = SequenceProxyContainer< std::add_const_t< TNodeProp >, const_container_type >;

    template< typename TNodeProp >
    using const_name_proxy_container = NameProxyContainer< std::add_const_t< TNodeProp >, const_container_type >;
  };  /* --- end of template class NodePropertyTrait --- */

  /**
   *  @brief  Node property class (dynamic).
   *
   *  Represent data associated with each node, mainly node sequences. Each data
   *  structure associated with each node are stored in a container in node rank
   *  order.
   */
  template< uint8_t ...TWidths >
  class NodeProperty< Dynamic, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using trait_type = NodePropertyTrait< spec_type, TWidths... >;
    using id_type = typename trait_type::id_type;
    using offset_type = typename trait_type::offset_type;
    using rank_type = typename trait_type::rank_type;
    using sequence_type = typename trait_type::sequence_type;
    using string_type = typename trait_type::string_type;
    using node_type = typename trait_type::node_type;
    using value_type = typename trait_type::value_type;
    using container_type = typename trait_type::container_type;
    using size_type = typename trait_type::size_type;
    using const_reference = typename trait_type::const_reference;
    using const_iterator = typename trait_type::const_iterator;
    using const_sequence_container_type =
        typename trait_type::template const_sequence_proxy_container< NodeProperty >;
    using const_name_container_type =
        typename trait_type::template const_name_proxy_container< NodeProperty >;
    using const_sequenceset_type = const_sequence_container_type;
    using const_stringset_type = const_name_container_type;
    using seq_const_reference = typename const_sequenceset_type::const_reference;
    using seq_reference = typename const_sequenceset_type::reference;
    using succinct_type = NodeProperty< Succinct, TWidths... >;
    using dynamic_type = NodeProperty;

    /* === LIFECYCLE === */
    /* constructor */
    NodeProperty( )
      : sequences_len_sum( 0 ), names_len_sum( 0 )
    { }

    NodeProperty( NodeProperty const& other ) = default;      /* copy constructor */
    NodeProperty( NodeProperty&& other ) noexcept = default;  /* move constructor */
    ~NodeProperty() noexcept = default;                       /* destructor       */

    /* === ACCESSORS === */
    inline container_type const&
    get_nodes( ) const
    {
      return this->nodes;
    }

    inline typename sequence_type::size_type
    get_sequences_len_sum( ) const
    {
      return this->sequences_len_sum;
    }

    inline typename string_type::size_type
    get_names_len_sum( ) const
    {
      return this->names_len_sum;
    }

    /* === OPERATORS === */
    NodeProperty& operator=( NodeProperty const& other ) = default;      /* copy assignment operator */
    NodeProperty& operator=( NodeProperty&& other ) noexcept = default;  /* move assignment operator */

    inline const_reference
    operator[]( size_type i ) const
    {
      return this->nodes[ i ];
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
      return this->nodes.at( i );
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

    inline size_type
    size( ) const
    {
      return this->nodes.size();
    }

    inline void
    add_node( value_type node )
    {
      this->sequences_len_sum += node.sequence.size();
      this->names_len_sum += node.name.size();
      this->nodes.push_back( std::move( node ) );
    }

    inline void
    update_node( rank_type rank, value_type node )
    {
      value_type& old = this->nodes[ rank - 1 ];
      this->sequences_len_sum += node.sequence.size() - old.sequence.size();
      this->names_len_sum += node.name.size() - old.name.size();
      old = std::move( node );
    }

    template< typename TContainer >
    inline void
    sort_nodes( TContainer const& perm )
    {
      util::permute( perm, this->nodes );
    }

    inline const_sequenceset_type
    sequences( ) const
    {
      return const_sequenceset_type( this, &( this->nodes ) );
    }

    inline const_stringset_type
    names( ) const
    {
      return const_stringset_type( this, &( this->nodes ) );
    }

    inline void
    clear( )
    {
      this->nodes.clear();
      this->sequences_len_sum = 0;
      this->names_len_sum = 0;
    }

    inline void
    shrink_to_fit( )
    {
      this->nodes.shrink_to_fit();
    }

  private:
    /* === DATA MEMBERS === */
    container_type nodes;
    typename sequence_type::size_type sequences_len_sum;
    typename string_type::size_type names_len_sum;
  };  /* --- end of template class NodeProperty --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_NODE_PROP_DYNAMIC_HPP__ --- */
