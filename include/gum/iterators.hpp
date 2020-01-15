/**
 *    @file  iterators.hpp
 *   @brief  Generic iterators for random access containers
 *
 *  This header file defines different generic iterators for random access containers.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Wed Oct 23, 2019  17:30
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_ITERATOR_HPP__
#define GUM_ITERATOR_HPP__

#include <functional>
#include <iterator>


namespace gum {
  template< typename TContainer >
  class RandomAccessConstIterator
    : public std::iterator< std::random_access_iterator_tag, typename TContainer::value_type, typename TContainer::difference_type >
  {
  public:
    /* === FRIENDSHIP === */
    template< typename TContainer2 >
    friend typename RandomAccessConstIterator< TContainer2 >::difference_type
    operator-( const RandomAccessConstIterator< TContainer2 >& x,
               const RandomAccessConstIterator< TContainer2 >& y );

    /* === TYPEDEFS === */
    using container_type = TContainer;
    using iterator = RandomAccessConstIterator;
    using const_reference = typename container_type::const_reference;
    using difference_type = typename container_type::difference_type;
    using size_type = typename container_type::size_type;

    /* === LIFECYCLE === */
    RandomAccessConstIterator( ) : ptr( nullptr ), idx( 0 ) { }

    RandomAccessConstIterator( TContainer const* _ptr, size_type _idx = 0 )
      : ptr( _ptr ), idx( _idx ) { }

    /* === OPERATORS === */
    inline const_reference
    operator*( ) const
    {
      return ( *this->ptr )[ this->idx ];
    }

    inline iterator&
    operator++( )
    {
      ++this->idx;
      return *this;
    }

    inline iterator
    operator++( int )
    {
      iterator it = *this;
      ++( *this );
      return it;
    }

    inline iterator&
    operator--( )
    {
      --this->idx;
      return *this;
    }

    inline iterator
    operator--( int )
    {
      iterator it = *this;
      --( *this );
      return it;
    }

    inline iterator&
    operator+=( difference_type i )
    {
      if ( i < 0 ) return *this -= ( -i );
      this->idx += i;
      return *this;
    }

    inline iterator&
    operator-=( difference_type i )
    {
      if ( i < 0 ) return *this += ( -i );
      this->idx -= i;
      return *this;
    }

    inline iterator
    operator+( difference_type i ) const
    {
      iterator it = *this;
      return it += i;
    }

    inline iterator
    operator-( difference_type i ) const
    {
      iterator it = *this;
      return it -= i;
    }

    inline const_reference
    operator[]( difference_type i ) const
    {
      return *( *this + i );
    }

    inline bool
    operator==( iterator const& it ) const
    {
      return it.ptr == this->ptr && it.idx == this->idx;
    }

    inline bool
    operator!=( iterator const& it ) const
    {
      return !( *this==it );
    }

    inline bool
    operator<( const iterator& it ) const
    {
      return this->idx < it.idx;
    }

    inline bool
    operator>( iterator const& it ) const
    {
      return this->idx > it.idx;
    }

    inline bool
    operator>=( iterator const& it ) const
    {
      return !( *this < it );
    }

    inline bool
    operator<=( const iterator& it ) const
    {
      return !( *this > it );
    }

  protected:
    /* === DATA MEMBERS === */
    container_type const* ptr;
    size_type idx;
  };  /* --- end of template class RandomAccessConstIterator --- */

  template< typename TContainer >
  inline typename RandomAccessConstIterator< TContainer >::difference_type
  operator-( const RandomAccessConstIterator< TContainer >& x,
             const RandomAccessConstIterator< TContainer >& y )
  {
    using difference_type = typename RandomAccessConstIterator< TContainer >::difference_type;
    return static_cast< difference_type >( x.idx ) - static_cast< difference_type >( y.idx );
  }

  template< typename TContainer >
  inline RandomAccessConstIterator< TContainer >
  operator+( typename RandomAccessConstIterator< TContainer >::difference_type n,
             RandomAccessConstIterator< TContainer > const& it )
  {
    return it + n;
  }

  template< typename TContainer, typename TValue >
  class RandomAccessProxyContainer {
  public:
    /* === TYPEDEFS === */
    using container_type = TContainer;
    using value_type = TValue;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using proxy_type = typename container_type::value_type;
    using function_type = std::function< value_type( proxy_type const& ) >;
    using const_reference = value_type const;
    using const_iterator = RandomAccessConstIterator< RandomAccessProxyContainer >;

    /* === LIFECYCLE === */
    RandomAccessProxyContainer( )
      : ptr( nullptr ), f( []( proxy_type const& x ) -> value_type { return x; } ) { }
    RandomAccessProxyContainer( container_type const* _ptr, function_type _f )
      : ptr( _ptr ), f( _f ) { }

    /* === OPERATORS === */
    inline value_type
    operator[]( size_type i ) const
    {
      return this->f( ( *this->ptr )[ i ] );
    }

    /* === METHODS === */
    inline value_type
    at( size_type i ) const
    {
      if ( i < 0 || i >= this->size() ) throw std::runtime_error( "index out of range" );
      return ( *this )[ i ];
    }

    inline size_type
    size( ) const
    {
      return this->ptr->size();
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
      return *this->begin();
    }

    inline const_reference
    front( ) const
    {
      return *( this->end() - 1 );
    }

  private:
    /* === DATA MEMBERS === */
    container_type const* ptr;
    function_type f;
  };  /* --- end of template class RandomAccessProxyContainer --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_ITERATOR_HPP__ --- */
