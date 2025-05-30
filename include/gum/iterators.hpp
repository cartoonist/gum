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
#include <type_traits>

#include "basic_types.hpp"


namespace gum {
  /**
   *  @brief  Generic random-access iterator
   *
   *  It defines a generic iterator over a container whose type is determined by the
   *  template type parameter `TContainer`.
   *
   *  NOTE: Defining an iterator constant does not prevent modification of the
   *  underlying container just like const pointers. However, immutability is guarnteed
   *  when `TContainer` has const qualifier. `RandomAccessConstIterator` type alias adds
   *  const quilifier to the given container.
   */
  template< typename TContainer >
  class RandomAccessIterator {
  public:
    /* === FRIENDSHIP === */
    template< typename TContainer2 >
    friend typename RandomAccessIterator< TContainer2 >::difference_type
    operator-( const RandomAccessIterator< TContainer2 >& x,
               const RandomAccessIterator< TContainer2 >& y );
    /* === TYPEDEFS === */
    using container_type = TContainer;
    using iterator = RandomAccessIterator;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename container_type::value_type;
    using difference_type = typename container_type::difference_type;
    using pointer = typename container_type::value_type*;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;
    /* === LIFECYCLE === */
    RandomAccessIterator( ) : ptr( nullptr ), idx( 0 ) { }

    RandomAccessIterator( TContainer * _ptr, size_type _idx = 0 )
      : ptr( _ptr ), idx( _idx ) { }
    /* === OPERATORS === */
    inline decltype(auto)
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
    container_type * ptr;
    size_type idx;
  };  /* --- end of template class RandomAccessIterator --- */

  /* --- RandomAccessIterator interface functions --- */
  template< typename TContainer >
  inline typename RandomAccessIterator< TContainer >::difference_type
  operator-( const RandomAccessIterator< TContainer >& x,
             const RandomAccessIterator< TContainer >& y )
  {
    using difference_type = typename RandomAccessIterator< TContainer >::difference_type;
    return static_cast< difference_type >( x.idx ) - static_cast< difference_type >( y.idx );
  }

  template< typename TContainer >
  inline RandomAccessIterator< TContainer >
  operator+( typename RandomAccessIterator< TContainer >::difference_type n,
             RandomAccessIterator< TContainer > const& it )
  {
    return it + n;
  }
  /* --- end of RandomAccessIterator interface functions --- */

  /**
   *  @brief  Generic random-'const'-access iterator
   *
   *  NOTE: Defining an iterator constant does not prevent modification of the
   *  underlying container just like const pointers. However, immutability is guarnteed
   *  when `TContainer` has const qualifier. `RandomAccessConstIterator` type alias adds
   *  const quilifier to the given container.
   */
  template< typename TContainer >
  using RandomAccessConstIterator = RandomAccessIterator< std::add_const_t< TContainer > >;

  /**
   *  @brief  Generic random-access reverse iterator
   *
   *  It defines a generic reverse iterator over a container whose type is
   *  determined by the template type parameter `TContainer`.
   *
   *  NOTE: Defining an iterator constant does not prevent modification of the
   *  underlying container just like const pointers. However, immutability is guarnteed
   *  when `TContainer` has const qualifier. `RandomAccessConstIterator` type alias adds
   *  const quilifier to the given container.
   */
  template< typename TContainer >
  class RandomAccessReverseIterator {
  public:
    /* === FRIENDSHIP === */
    template< typename TContainer2 >
    friend typename RandomAccessReverseIterator< TContainer2 >::difference_type
    operator-( const RandomAccessReverseIterator< TContainer2 >& x,
               const RandomAccessReverseIterator< TContainer2 >& y );
    /* === TYPEDEFS === */
    using container_type = TContainer;
    using iterator = RandomAccessReverseIterator;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename container_type::value_type;
    using difference_type = typename container_type::difference_type;
    using pointer = typename container_type::value_type*;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;
    /* === LIFECYCLE === */
    RandomAccessReverseIterator( ) : ptr( nullptr ), idx( 0 ) { }

    RandomAccessReverseIterator( TContainer * _ptr )
      : ptr( _ptr ), idx( _ptr->size() ) { }

    // NOTE: Similar to the standard library, reversed iterators store the index
    // of the next element (on the right) that it actually refers to. So, a
    // reverse iterator constructed from an iterator pointing to the element at
    // index `idx` refers to the element at `idx - 1`.
    RandomAccessReverseIterator( TContainer * _ptr, size_type _idx )
      : ptr( _ptr ), idx( _idx ) { }
    /* === OPERATORS === */
    inline decltype(auto)
    operator*( ) const
    {
      return ( *this->ptr )[ this->idx - 1 ];
    }

    inline iterator&
    operator++( )
    {
      --this->idx;
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
      ++this->idx;
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
      this->idx -= i;
      return *this;
    }

    inline iterator&
    operator-=( difference_type i )
    {
      if ( i < 0 ) return *this += ( -i );
      this->idx += i;
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
      return this->idx > it.idx;
    }

    inline bool
    operator>( iterator const& it ) const
    {
      return this->idx < it.idx;
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
    container_type * ptr;
    size_type idx;
  };  /* --- end of template class RandomAccessReverseIterator --- */

  /* --- RandomAccessReverseIterator interface functions --- */
  template< typename TContainer >
  inline typename RandomAccessReverseIterator< TContainer >::difference_type
  operator-( const RandomAccessReverseIterator< TContainer >& x,
             const RandomAccessReverseIterator< TContainer >& y )
  {
    using difference_type = typename RandomAccessReverseIterator< TContainer >::difference_type;
    return static_cast< difference_type >( y.idx ) - static_cast< difference_type >( x.idx );
  }

  template< typename TContainer >
  inline RandomAccessReverseIterator< TContainer >
  operator+( typename RandomAccessReverseIterator< TContainer >::difference_type n,
             RandomAccessReverseIterator< TContainer > const& it )
  {
    return it + n;
  }
  /* --- end of RandomAccessReverseIterator interface functions --- */

  /**
   *  @brief  Generic random-'const'-access iterator
   *
   *  NOTE: Defining an iterator constant does not prevent modification of the
   *  underlying container just like const pointers. However, immutability is guarnteed
   *  when `TContainer` has const qualifier. `RandomAccessConstIterator` type alias adds
   *  const quilifier to the given container.
   */
  template< typename TContainer >
  using RandomAccessConstReverseIterator = RandomAccessReverseIterator< std::add_const_t< TContainer > >;

  /**
   *  @brief  Generic random-access bidirected iterator
   *
   *  It defines a generic bidirected iterator over a container whose type is
   *  determined by the template type parameter `TContainer`.
   *
   *  NOTE: Defining an iterator constant does not prevent modification of the
   *  underlying container just like const pointers. However, immutability is guarnteed
   *  when `TContainer` has const qualifier. `RandomAccessConstIterator` type alias adds
   *  const quilifier to the given container.
   */
  template< typename TContainer >
  class RandomAccessBidiIterator {
  public:
    /* === FRIENDSHIP === */
    template< typename TContainer2 >
    friend typename RandomAccessBidiIterator< TContainer2 >::difference_type
    operator-( const RandomAccessBidiIterator< TContainer2 >& x,
               const RandomAccessBidiIterator< TContainer2 >& y );
    /* === TYPEDEFS === */
    using container_type = TContainer;
    using iterator = RandomAccessBidiIterator;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename container_type::value_type;
    using difference_type = typename container_type::difference_type;
    using pointer = typename container_type::value_type*;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;
    /* === LIFECYCLE === */
    RandomAccessBidiIterator( ) : ptr( nullptr ), idx( 0 ) { }

    RandomAccessBidiIterator( TContainer* _ptr )
        : ptr( _ptr ), idx( 0 ), step( 1 ) { }

    RandomAccessBidiIterator( TContainer* _ptr, bool fwd )
        : ptr( _ptr ), idx( fwd ? 0 : _ptr->size() ), step( fwd ? 1 : -1 ) { }

    // NOTE that in case of reverse iterator, setting index to `idx` means that
    // the iterator points to the element at `idx - 1`.
    RandomAccessBidiIterator( TContainer* _ptr, size_type idx,
                              bool fwd = true )
        : ptr( _ptr ), idx( idx ), step( fwd ? 1 : -1 ) { }
    /* === OPERATORS === */
    inline decltype(auto)
    operator*( ) const
    {
      auto r = this->idx;
      if ( this->is_reverse() ) --r;
      return ( *this->ptr )[ r ];
    }

    inline iterator&
    operator++( )
    {
      this->idx += this->step;
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
      this->idx -= this->step;
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
      this->idx += i * this->step;
      return *this;
    }

    inline iterator&
    operator-=( difference_type i )
    {
      if ( i < 0 ) return *this += ( -i );
      this->idx -= i * this->step;
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
      return it.ptr == this->ptr && it.idx == this->idx
             && it.step == this->step;
    }

    inline bool
    operator!=( iterator const& it ) const
    {
      return !( *this==it );
    }

    inline bool
    operator<( const iterator& it ) const
    {
      if ( this->is_forward() ) return this->idx < it.idx;
      else return this->idx > it.idx;
    }

    inline bool
    operator>( iterator const& it ) const
    {
      if ( this->is_forward() ) return this->idx > it.idx;
      else return this->idx < it.idx;
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

    /* === METHODS === */
    inline bool
    is_forward( ) const
    {
      return this->step > 0;
    }

    inline bool
    is_reverse( ) const
    {
      return this->step < 0;
    }

  private:
    /* === DATA MEMBERS === */
    container_type* ptr;
    size_type idx;
    int8_t step;
  }; /* --- end of template class RandomAccessBidiIterator --- */

  /* --- RandomAccessBidiIterator interface functions --- */
  template< typename TContainer >
  inline typename RandomAccessBidiIterator< TContainer >::difference_type
  operator-( const RandomAccessBidiIterator< TContainer >& x,
             const RandomAccessBidiIterator< TContainer >& y )
  {
    using difference_type = typename RandomAccessBidiIterator< TContainer >::difference_type;
    if ( x.is_forward() ) {
      return static_cast< difference_type >( x.idx ) - static_cast< difference_type >( y.idx );
    }
    return static_cast< difference_type >( y.idx ) - static_cast< difference_type >( x.idx );
  }

  template< typename TContainer >
  inline RandomAccessBidiIterator< TContainer >
  operator+( typename RandomAccessBidiIterator< TContainer >::difference_type n,
             RandomAccessBidiIterator< TContainer > const& it )
  {
    return it + n;
  }
  /* --- end of RandomAccessBidiIterator interface functions --- */

  /**
   *  @brief  Generic random-'const'-access iterator
   *
   *  NOTE: Defining an iterator constant does not prevent modification of the
   *  underlying container just like const pointers. However, immutability is guarnteed
   *  when `TContainer` has const qualifier. `RandomAccessConstIterator` type alias adds
   *  const quilifier to the given container.
   */
  template< typename TContainer >
  using RandomAccessConstBidiIterator = RandomAccessBidiIterator< std::add_const_t< TContainer > >;

  /**
   *  @brief  Generic random-access proxy container
   *
   *  NOTE: A const proxy container only exposes const references to elements (in case
   *  `TContainer` return references via indexing -- i.e. `operator[]` ). This is
   *  independent of `TContainer` const-ness.
   */
  template< typename TContainer, typename TFunction >
  class RandomAccessProxyContainer {
  public:
    /* === TYPEDEFS === */
    using container_type = TContainer;
    using function_type = TFunction;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using proxy_type = decltype( std::declval< TContainer >()[ std::declval< size_type >() ] );
    using reference = decltype( std::declval< TFunction& >()( std::declval< proxy_type >() ) );
    using value_type = std::decay_t< reference >;
    using const_reference = add_const_east_t< reference >;
    using iterator = RandomAccessIterator< RandomAccessProxyContainer >;
    using const_iterator = RandomAccessConstIterator< RandomAccessProxyContainer >;
    using reverse_iterator = RandomAccessReverseIterator< RandomAccessProxyContainer >;
    using const_reverse_iterator = RandomAccessConstReverseIterator< RandomAccessProxyContainer >;
    /* === LIFECYCLE === */
    RandomAccessProxyContainer( )
      : ptr( nullptr ), f( []( proxy_type x ) -> proxy_type { return x; } ) { }

    RandomAccessProxyContainer( container_type * _ptr, function_type _f )
      : ptr( _ptr ), f( _f ) { }
    /* === OPERATORS === */
    inline reference
    operator[]( size_type i ) noexcept
    {
      return this->f( ( *this->ptr )[ i ] );
    }

    inline const_reference
    operator[]( size_type i ) const noexcept
    {
      return this->f( ( *this->ptr )[ i ] );
    }
    /* === METHODS === */
    inline reference
    at( size_type i )
    {
      if ( i < 0 || i >= this->size() ) throw std::runtime_error( "index out of range" );
      return ( *this )[ i ];
    }

    inline const_reference
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

    inline bool
    empty( ) const
    {
      return this->size() == 0;
    }

    inline iterator
    begin( ) noexcept
    {
      return iterator( this, 0 );
    }

    inline const_iterator
    begin( ) const noexcept
    {
      return const_iterator( this, 0 );
    }

    inline iterator
    end( ) noexcept
    {
      return iterator( this, this->size() );
    }

    inline const_iterator
    end( ) const noexcept
    {
      return const_iterator( this, this->size() );
    }

    inline reverse_iterator
    rbegin(  ) noexcept
    {
      return reverse_iterator( this, this->size() );
    }

    inline const_reverse_iterator
    rbegin(  ) const noexcept
    {
      return const_reverse_iterator( this, this->size() );
    }

    inline reverse_iterator
    rend(  ) noexcept
    {
      return reverse_iterator( this, 0 );
    }

    inline const_reverse_iterator
    rend(  ) const noexcept
    {
      return const_reverse_iterator( this, 0 );
    }

    inline reference
    back( )
    {
      return *this->begin();
    }

    inline const_reference
    back( ) const
    {
      return *this->begin();
    }

    inline reference
    front( )
    {
      return *( this->end() - 1 );
    }

    inline const_reference
    front( ) const
    {
      return *( this->end() - 1 );
    }
  private:
    /* === DATA MEMBERS === */
    container_type * ptr;
    function_type f;
  };  /* --- end of template class RandomAccessProxyContainer --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_ITERATOR_HPP__ --- */
