/**
 *    @file  stringset.hpp
 *   @brief  String set class
 *
 *  This header file defines StringSet class.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Tue Oct 15, 2019  17:50
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_STRINGSET_HPP__
#define GUM_STRINGSET_HPP__

#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>

#include "alphabet.hpp"
#include "iterators.hpp"
#include "basic_utils.hpp"
#include "basic_types.hpp"


namespace gum {
  template< typename TAlphabet >
  using String = sdsl::int_vector< TAlphabet::width >;

  template< typename TAlphabet >
  class StringSet;

  namespace util {
    template< typename TAlphabet, typename TInputIt, typename TOutputIt >
    inline TOutputIt
    encode( TInputIt first, TInputIt last, TOutputIt d_first, TAlphabet )
    {
      return std::transform( first, last, d_first, TAlphabet::char2comp );
    }

    template< typename TAlphabet, typename TInputIt, typename TOutputIt >
    inline TOutputIt
    decode( TInputIt first, TInputIt last, TOutputIt d_first, TAlphabet )
    {
      return std::transform( first, last, d_first, TAlphabet::comp2char );
    }

    template< typename TInputIt, typename TOutputIt >
    inline TOutputIt
    encode( TInputIt first, TInputIt last, TOutputIt d_first, Char )
    {
      return std::copy( first, last, d_first );
    }

    template< typename TInputIt, typename TOutputIt >
    inline TOutputIt
    decode( TInputIt first, TInputIt last, TOutputIt d_first, Char )
    {
      return std::copy( first, last, d_first );
    }

    template< typename TAlphabet >
    inline void
    assign( std::string& first, String< TAlphabet > const& second, TAlphabet /* tag */ )
    {
      first.resize( second.size() );
      decode( second.begin(), second.end(), first.begin(), TAlphabet() );
    }

    template< typename TAlphabet >
    inline void
    assign( String< TAlphabet >& first, std::string const& second, TAlphabet /* tag */ )
    {
      first.resize( second.size() );
      encode( second.begin(), second.end(), first.begin(), TAlphabet() );
    }

    template< typename TIter >
    inline std::size_t
    length_sum( TIter begin, TIter end )
    {
      std::size_t sum = 0;
      for ( ; begin != end; ++begin ) sum += begin->size();
      return sum;
    }

    template< typename TContainer >
    inline std::size_t
    length_sum( TContainer strset )
    {
      return length_sum( strset.begin(), strset.end() );
    }

    template< typename TAlphabet >
    inline std::size_t
    length_sum( StringSet< TAlphabet > strset )
    {
      return strset.length_sum( );
    }
  }  /* --- end of namespace util --- */

  template< typename TAlphabet >
  class StringSetTrait {
  private:
    using alphabet_type = TAlphabet;
  public:
    using value_type = std::string;
    using container_type = String< alphabet_type >;
    using size_type = typename container_type::size_type;
    using bv_type = sdsl::bit_vector;
    using rs_type = typename bv_type::rank_1_type;
    using ss_type = typename bv_type::select_1_type;
  };  /* --- end of template class StringSetTrait --- */

  template< typename TAlphabet = DNA5 >
  class StringSet {
  public:
    /* === TYPEDEFS === */
    using alphabet_type = TAlphabet;
    using char_type = typename alphabet_type::char_type;
    using trait_type = StringSetTrait< alphabet_type >;
    using value_type = typename trait_type::value_type;
    using container_type = typename trait_type::container_type;
    using size_type = typename trait_type::size_type;
    using bv_type = typename trait_type::bv_type;
    using rs_type = typename trait_type::rs_type;
    using ss_type = typename trait_type::ss_type;
    using difference_type = std::ptrdiff_t;
    using const_reference = value_type;
    using const_iterator = RandomAccessConstIterator< StringSet >;

    /* === LIFECYCLE === */
    StringSet( ) : count( 0 ) { }

    template< typename TIter >
    StringSet( TIter begin, TIter end )
      : count( 0 )
    {
      this->extend( begin, end );
    }

    template< typename TContainer >
    StringSet( TContainer const& ext_strset )
      : count( 0 )
    {
      this->extend( ext_strset );
    }

    /* copy constructor */
    StringSet( StringSet const& other )
      : strset( other.strset ),
        breaks( other.breaks ),
        count( other.count )
    {
      sdsl::util::init_support( this->rank, &this->breaks );
      sdsl::util::init_support( this->select, &this->breaks );
    }

    /* move constructor */
    StringSet( StringSet&& other ) noexcept
      : strset( std::move( other.strset ) ),
        breaks( std::move( other.breaks ) ),
        count( other.count )
    {
      sdsl::util::init_support( this->rank, &this->breaks );
      sdsl::util::init_support( this->select, &this->breaks );
      sdsl::util::clear( other.rank );
      sdsl::util::clear( other.select );
    }

    /* destructor       */
    ~StringSet( ) noexcept
    {
      sdsl::util::clear( this->rank );
      sdsl::util::clear( this->select );
    }

    /* === OPERATORS === */
    /* copy assignment operator */
    StringSet&
    operator=( StringSet const& other )
    {
      this->strset = other.strset;
      this->breaks = other.breaks;
      this->count = other.count;
      sdsl::util::init_support( this->rank, &this->breaks );
      sdsl::util::init_support( this->select, &this->breaks );
      return *this;
    }

    /* move assignment operator */
    StringSet&
    operator=( StringSet&& other ) noexcept
    {
      this->strset = std::move( other.strset );
      this->breaks = std::move( other.breaks );
      this->count = other.count;
      sdsl::util::init_support( this->rank, &this->breaks );
      sdsl::util::init_support( this->select, &this->breaks );
      sdsl::util::clear( other.rank );
      sdsl::util::clear( other.select );
      return *this;
    }

    inline value_type
    operator()( size_type pos, size_type len ) const
    {
      return this->extract( pos, pos + len );
    }

    inline const_reference
    operator[]( size_type i ) const
    {
      return this->extract( this->position( i ), this->position( i + 1 ) );
    }

    /* === METHODS === */
    inline const_reference
    at( size_type i ) const
    {
      if ( i < 0 or i >= this->size() ) throw std::runtime_error( "index out of range" );
      return ( *this )[ i ];
    }

    inline size_type
    size( ) const
    {
      return this->count;
    }

    inline size_type
    position( size_type i ) const
    {
      return ( 0 <= i && i < this->count ) ? this->select( i + 1 ) : this->strset.size();
    }

    inline size_type
    length( size_type i ) const
    {
      return this->position( i + 1 ) - this->position( i );
    }

    inline size_type
    length_sum( ) const
    {
      return this->strset.size();
    }

    inline bool
    empty( ) const
    {
      return this->size() == 0;
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
    front( ) const
    {
      return *( this->begin() );
    }

    inline const_reference
    back( ) const
    {
      return *( this->end() - 1 );
    }

    template< typename TContainer >
    inline void
    extend( TContainer const& ext_strset )
    {
      auto len_sum = util::length_sum( ext_strset );
      this->_extend( ext_strset.begin(), ext_strset.end(), len_sum );
    }

    template< typename TIter >
    inline void
    extend( TIter begin, TIter end )
    {
      auto len_sum = util::length_sum( begin, end );
      this->_extend( begin, end, len_sum );
    }

  private:
    /* === DATA MEMBERS === */
    container_type strset;
    bv_type breaks;
    rs_type rank;
    ss_type select;
    size_type count;

    /* === METHODS === */
    inline size_type
    resize( size_type new_size )
    {
      auto old_size = this->strset.size();
      this->strset.resize( new_size );
      bv_type new_breaks( new_size, 0 );
      if ( old_size != 0 ) util::bv_icopy( this->breaks, new_breaks, 0, old_size );
      sdsl::util::assign( this->breaks, std::move( new_breaks ) );
      return old_size;
    }

    inline size_type
    expand( size_type expand_len )
    {
      return this->resize( this->strset.size() + expand_len );
    }

    template< typename TIter,
              typename = std::enable_if_t< std::is_same< typename std::iterator_traits< TIter >::iterator_category,
                                                         std::random_access_iterator_tag >::value > >
    inline void
    _extend( TIter begin, TIter end, size_type len_sum )
    {
      size_type old_size = this->expand( len_sum );
      auto cpos = this->strset.begin() + old_size;
      for ( ; begin != end; ++begin ) {
        this->breaks[ cpos - this->strset.begin() ] = 1;
        assert( cpos < this->strset.end() );
        cpos = util::encode( begin->begin(), begin->end(), cpos, alphabet_type() );
        ++count;
      }
      sdsl::util::init_support( this->rank, &this->breaks );
      sdsl::util::init_support( this->select, &this->breaks );
    }

    inline value_type
    extract( size_type begin_pos, size_type end_pos ) const
    {
      auto biter = this->strset.begin() + begin_pos;
      auto eiter = this->strset.begin() + end_pos;
      value_type elem( end_pos - begin_pos, '\0' );
      util::decode( biter, eiter, elem.begin(), alphabet_type() );
      return elem;
    }
  };  /* --- end of template class StringSet --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_STRINGSET_HPP__ --- */
