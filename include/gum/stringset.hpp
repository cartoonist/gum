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

#include <algorithm>
#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <type_traits>

#include "alphabet.hpp"
#include "iterators.hpp"
#include "basic_utils.hpp"
#include "basic_types.hpp"


namespace gum {
  /* === Forward Declarations === */
  template< typename TAlphabet >
  class String;

  template< typename TString >
  class StringView;

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

    template< typename TAlphabet1, typename TAlphabet2, typename TInputIt, typename TOutputIt,
              typename=std::enable_if_t< !std::is_same< TAlphabet1, TAlphabet2 >::value > >
    inline TOutputIt
    convert( TInputIt first, TInputIt last, TOutputIt d_first, TAlphabet1, TAlphabet2 )
    {
      static_assert( is_superset< TAlphabet2, TAlphabet1 >::value, "incompatible alphabets" );

      return std::transform( first, last, d_first,
                             []( auto elem ) {
                               auto c = TAlphabet1::comp2char( elem );
                               return TAlphabet2::char2comp( c );
                             } );
    }

    template< typename TAlphabet, typename TInputIt, typename TOutputIt >
    inline TOutputIt
    convert( TInputIt first, TInputIt last, TOutputIt d_first, TAlphabet, TAlphabet )
    {
      return std::copy( first, last, d_first );
    }

    template< typename TAlphabet, typename TInputIt, typename TOutputIt,
              typename=std::enable_if_t< !std::is_same< TAlphabet, Char >::value > >
    inline TOutputIt
    convert( TInputIt first, TInputIt last, TOutputIt d_first, Char, TAlphabet tag )
    {
      return encode( first, last, d_first, tag );
    }

    template< typename TAlphabet, typename TInputIt, typename TOutputIt,
              typename=std::enable_if_t< !std::is_same< TAlphabet, Char >::value > >
    inline TOutputIt
    convert( TInputIt first, TInputIt last, TOutputIt d_first, TAlphabet tag, Char )
    {
      return decode( first, last, d_first, tag );
    }

    template< typename TAlphabet >
    inline void
    assign( std::string& first, String< TAlphabet > const& second, TAlphabet tag={} )
    {
      first.resize( second.size() );
      decode( second.begin(), second.end(), first.begin(), tag );
    }

    template< typename TAlphabet >
    inline void
    assign( String< TAlphabet >& first, std::string const& second, TAlphabet tag={} )
    {
      first.resize( second.size() );
      encode( second.begin(), second.end(), first.begin(), tag );
    }

    template< typename TAlphabet1, typename TAlphabet2 >
    inline void
    assign( String< TAlphabet1 >& first, String< TAlphabet2 > const& second,
            TAlphabet1 tag1={}, TAlphabet2 tag2={} )
    {
      first.resize( second.size() );
      convert( second.begin(), second.end(), first.begin(), tag2, tag1 );
    }

    template< typename TAlphabet1, typename TAlphabet2 >
    inline void
    assign( String< TAlphabet1 >& str,
            StringView< String< TAlphabet2 > > const& view,
            TAlphabet1 tag1={}, TAlphabet2={} )
    {
      str.resize( view.size() );
      encode( view.begin(), view.end(), str.begin(),
              tag1 );  // `StringView` is a `char` view
    }

    template< typename TAlphabet >
    inline void
    assign( String< TAlphabet >& str,
            StringView< String< TAlphabet > > const& view, TAlphabet tag={} )
    {
      str.resize( view.size() );
      str.assign( view.base(), view.base() + view.size() );
    }

    template< typename TAlphabet >
    inline void
    assign( std::string& str, StringView< String< TAlphabet > > const& view,
            TAlphabet tag={} )
    {
      str.resize( view.size() );
      std::copy( view.begin(), view.end(),
                 str.begin() );  // `StringView` is a `char` view
    }

    template< typename TIter >
    inline std::size_t
    length_sum( TIter begin, TIter end )
    {
      std::size_t sum = 0;
      for ( ; begin != end; ++begin ) sum += ( *begin ).size();
      return sum;
    }

    template< typename TContainer >
    inline std::size_t
    length_sum( TContainer const& strset )
    {
      return strset.length_sum( );
    }
  }  /* --- end of namespace util --- */

  template< typename TAlphabet >
  class String
    : public sdsl::int_vector< TAlphabet::width > {
    public:
      /* === TYPE MEMBERS === */
      using alphabet_type = TAlphabet;
      using base_type = sdsl::int_vector<alphabet_type::width>;
      using typename base_type::const_iterator;
      using typename base_type::const_pointer;
      using typename base_type::const_reference;
      using typename base_type::difference_type;
      using typename base_type::index_category;
      using typename base_type::int_width_type;
      using typename base_type::iterator;
      using typename base_type::pointer;
      using typename base_type::rank_0_type;
      using typename base_type::rank_1_type;
      using typename base_type::reference;
      using typename base_type::select_0_type;
      using typename base_type::select_1_type;
      using typename base_type::size_type;
      using typename base_type::value_type;
      /* === LIFECYCLE === */
      using base_type::base_type;
      /* === OPERATORS === */
      template< typename TStringOrView >
      inline String&
      operator=( TStringOrView const& other )
      {
        util::assign( *this, other );
        return *this;
      }

      template< typename TString >
      inline operator TString() const
      {
        TString str;
        util::assign( str, *this );
        return str;
      }

      inline bool
      operator==( String const& other ) const
      {
        return base_type::operator==( other );
      }

      inline bool
      operator==( StringView< String > const& view ) const
      {
        return std::equal( view.base(), view.base() + view.size(),
                           this->begin() );
      }

      inline bool
      operator==( std::string const& str ) const
      {
        return std::equal( str.begin(), str.end(), this->begin(),
                           []( auto const& l, auto const& r ) {
                             return l == alphabet_type::comp2char( r );
                           } );
      }

      inline bool
      operator==( const char* cstr ) const
      {
        std::string_view str( cstr );
        return std::equal( str.begin(), str.end(), this->begin(),
                           []( auto const& l, auto const& r ) {
                             return l == alphabet_type::comp2char( r );
                           } );
      }
  };

  template< typename TAlphabet >
  inline bool
  operator==( std::string const& left, String< TAlphabet > const& right )
  {
    return right == left;
  }

  template< typename TAlphabet >
  inline bool
  operator==( const char* left, String< TAlphabet > const& right )
  {
    return right == left;
  }

  /**
   *   @brief  A constant (char) view over `String< TAlphabet >`
   *
   *   NOTE: Underlying string is always immutable.
   */
  template< typename TString >
  class StringView {
    public:
      /* === TYPE MEMBERS === */
      using string_type = TString;
      using alphabet_type = typename string_type::alphabet_type;
      using iterator = RandomAccessIterator< StringView >;
      using const_iterator = RandomAccessConstIterator< StringView >;
      using const_base_iterator = typename string_type::const_iterator;
      using reference = char;
      using const_reference = char;
      using value_type = char;
      using size_type = typename string_type::size_type;
      using difference_type = typename string_type::difference_type;
      /* === LIFECYCLE === */
      StringView( const string_type& str, size_type b, size_type l )
        : m_begin( str.begin() + b ), m_size( std::min( l, str.size() - b ) )
      {
        assert( b < str.size() );
      }

      StringView( const string_type& str, size_type b=0 )
        : m_begin( str.begin() + b ), m_size( str.size() - b )
      {
        assert( b < str.size() );
      }
      /* === OPERATORS === */
      inline const_reference
      operator[]( size_type i ) const noexcept
      {
        return alphabet_type::comp2char( *( this->m_begin + i ) );
      }

      inline bool
      operator==( string_type const& encoded_str ) const noexcept
      {
        if ( encoded_str.size() != this->size() ) return false;
        return std::equal( this->base(), this->base() + this->size(), encoded_str.begin() );
      }

      inline bool
      operator==( StringView const& view ) const noexcept
      {
        if ( view.size() != this->size() ) return false;
        return std::equal( this->base(), this->base() + this->size(), view.base() );
      }

      template< typename TContainer >
      inline bool
      operator==( TContainer const& str ) const noexcept
      {
        if ( str.size() != this->size() ) return false;
        return std::equal( this->begin(), this->end(), str.begin() );
      }

      inline bool
      operator==( const char* cstr ) const noexcept
      {
        std::string_view str( cstr );
        if ( str.size() != this->size() ) return false;
        return std::equal( this->begin(), this->end(), str.begin() );
      }

      template< typename _TString >
      inline operator _TString() const
      {
        _TString str;
        util::assign( str, *this );
        return str;
      }
      /* === METHODS === */
      inline const_reference
      at( size_type i ) const
      {
        if ( i < 0 or i >= this->size() ) throw std::runtime_error( "index out of range" );
        return ( *this )[ i ];
      }

      inline const_iterator
      begin() const noexcept
      {
        return const_iterator( this, 0 );
      }

      inline const_iterator
      end() const noexcept
      {
        return const_iterator( this, this->m_size );
      }

      inline const_reference
      front() const noexcept
      {
        return *( this->begin() );
      }

      inline const_reference
      back() const noexcept
      {
        return *( this->end() - 1 );
      }

      inline size_type
      size() const noexcept
      {
        return this->m_size;
      }

      inline const_base_iterator
      base() const noexcept
      {
        return this->m_begin;
      }
    protected:
      /* === DATA MEMBERS === */
      const_base_iterator m_begin;
      size_type m_size;
  };

  template< typename TString >
  inline bool
  operator==( std::string const& left, StringView< TString > const& right )
  {
    return right == left;
  }

  template< typename TString >
  inline bool
  operator==( const char* left, StringView< TString > const& right )
  {
    return right == left;
  }

  template< typename TAlphabet >
  class StringSetTrait {
  private:
    using alphabet_type = TAlphabet;
  public:
    using value_type = std::string;
    using container_type = String< alphabet_type >;
    using reference = StringView< container_type >;
    using const_reference = StringView< container_type >;
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
    using reference = typename trait_type::reference;
    using const_reference = typename trait_type::const_reference;
    using size_type = typename trait_type::size_type;
    using bv_type = typename trait_type::bv_type;
    using rs_type = typename trait_type::rs_type;
    using ss_type = typename trait_type::ss_type;
    using difference_type = std::ptrdiff_t;
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

    inline const_reference
    operator()( size_type pos, size_type len ) const
    {
      return const_reference( this->strset, pos, len );
    }

    inline const_reference
    operator[]( size_type i ) const
    {
      auto sp = this->start_position( i );
      auto len = this->end_position( i ) - sp;
      return const_reference( this->strset, sp, len );
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
    idx( size_type pos ) const
    {
      if ( pos == 0 ) return 0;
      return this->rank( pos );
    }

    inline size_type
    start_position( size_type i ) const
    {
      if ( i == 0 ) return 0;
      if ( i >= this->size() ) return this->strset.size();
      return this->select( i ) + 1;
    }

    inline size_type
    end_position( size_type i ) const
    {
      return this->select( i + 1 );
    }

    inline size_type
    length( size_type i ) const
    {
      return this->end_position( i ) - this->start_position( i );
    }

    inline size_type
    length_sum( ) const
    {
      return this->strset.size() - this->size() /* number of delimiters */;
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

    inline value_type
    extract( size_type begin_pos, size_type end_pos ) const
    {
      auto biter = this->strset.begin() + begin_pos;
      auto eiter = this->strset.begin() + end_pos;
      value_type elem( end_pos - begin_pos, '\0' );
      util::decode( biter, eiter, elem.begin(), alphabet_type() );
      return elem;
    }

    inline void
    push_back( value_type const& str )
    {
      size_type cpos = this->expand( str.size() + 1 );
      this->put( str, cpos );
      sdsl::util::init_support( this->rank, &this->breaks );
      sdsl::util::init_support( this->select, &this->breaks );
    }

    inline void
    push_back( value_type&& str )
    {
      this->push_back( str );
    }

    inline void
    shrink_to_fit( )
    {
      sdsl::util::bit_compress( this->strset );
      sdsl::util::bit_compress( this->breaks );
      sdsl::util::init_support( this->rank, &this->breaks );
      sdsl::util::init_support( this->select, &this->breaks );
    }

    inline void
    clear( )
    {
      sdsl::util::clear( this->strset );
      sdsl::util::clear( this->breaks );
      sdsl::util::clear( this->rank );
      sdsl::util::clear( this->select );
      this->count = 0;
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
      if ( new_size == old_size ) return old_size;
      this->strset.resize( new_size );
      this->breaks.resize( new_size );
      util::bv_izero( this->breaks, old_size );
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
      size_type cpos = this->expand( len_sum + ( end - begin ) );
      for ( ; begin != end; ++begin ) {
        // It's important to have a `const` reference from the element, because
        // it may be returned by value. In that case the `*begin` would be
        // temporary object for which different multiple call of `begin` or `end`
        // methods returns iterators for different containers.
        auto const& str = *begin;
        cpos = this->put( str, cpos );
      }
      sdsl::util::init_support( this->rank, &this->breaks );
      sdsl::util::init_support( this->select, &this->breaks );
    }

    /**
     *  @brief  Put the given string at the specified position.
     *
     *  NOTE: The required space should be allocated beforehand.
     *  NOTE: It does not initialise the rank/select supports. Those should be
     *        done afterwards.
     *
     *  @param  str A string.
     *  @param  pos Put the given string in this position in the `strset`.
     *  @return The location immediately after the last character copied in the
     *          `strset`.
     */
    inline size_type
    put( value_type const& str, size_type pos )
    {
      assert( pos < this->strset.size() );
      auto citer = this->strset.begin() + pos;
      citer = util::encode( str.begin(), str.end(), citer, alphabet_type() );
      *citer = 0;  // write delimiter
      pos += str.size() + 1;  // update current position
      this->breaks[ pos - 1 ] = 1;  // mark delimiter
      ++this->count;
      return pos;
    }
  };  /* --- end of template class StringSet --- */
}  /* --- end of namespace gum --- */

#endif /* --- #ifndef GUM_STRINGSET_HPP__ --- */
