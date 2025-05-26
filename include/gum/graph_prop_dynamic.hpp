/**
 *    @file  graph_prop_dynamic.hpp
 *   @brief  `GraphProperty` Dynamic specification
 *
 *  This header file includes Dynamic specification of `GraphProperty` class.
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

#ifndef GUM_GRAPH_PROP_DYNAMIC_HPP__
#define GUM_GRAPH_PROP_DYNAMIC_HPP__

#include "digraph_traits_dynamic.hpp"
#include "graph_prop_base.hpp"


namespace gum {
  template< typename TDir, uint8_t ...TWidths >
  class GraphPropertyTrait< Dynamic, TDir, TWidths... > {
  private:
    using spec_type = Dynamic;
    using dir_type = TDir;
    using trait_type = DirectedGraphTrait< spec_type, dir_type, TWidths... >;
  public:
    using id_type = typename trait_type::id_type;
    using rank_type = typename trait_type::rank_type;
    using offset_type = typename trait_type::offset_type;
    using common_type = typename trait_type::common_type;
    using string_type = typename trait_type::string_type;

    class Path : public PathBase< id_type > {
    public:
      /* === TYPEDEFS === */
      using base_type = PathBase< id_type >;
      using typename base_type::value_type;
      using container_type = std::vector< value_type >;
      using const_reference = typename container_type::const_reference;
      using const_iterator = typename container_type::const_iterator;
      using size_type = typename container_type::size_type;

      /* === LIFECYCLE === */
      Path( id_type id_, string_type name_="" )    /* constructor */
        : id( id_ ), name( std::move( name_ ) ) { }

      /* === ACCESSORS === */
      inline id_type
      get_id( ) const
      {
        return this->id;
      }

      inline string_type const&
      get_name( ) const
      {
        return this->name;
      }

      inline container_type const&
      get_nodes( ) const
      {
        return this->nodes;
      }

      /* === METHODS === */
      inline void
      add_node( id_type id, bool reversed=false )
      {
        this->nodes.push_back( base_type::encode( id, reversed ) );
      }

      inline void
      flip_orientation( /*node*/ id_type node_id ) noexcept
      {
        for ( auto& n : this->nodes ) {
          if ( base_type::id_of( n ) == node_id ) {
            base_type::reverse( n );
          }
        }
      }

      template< typename TSet >
      inline void
      flip_orientation( TSet const& node_set )
      {
        for ( auto& n : this->nodes ) {
          if ( node_set.count( base_type::id_of( n ) ) ) {
            base_type::reverse( n );
          }
        }
      }

      template< typename TCallback >
      inline bool
      for_each_node( TCallback callback ) const
      {
        static_assert( std::is_invocable_r_v< bool, TCallback, id_type, bool >, "received a non-invocable as callback" );

        for ( auto it = this->nodes.begin(); it != this->nodes.end(); ++it ) {
          if ( !callback( base_type::id_of( *it ), base_type::is_reverse( *it ) ) ) {
            return false;
          }
        }
        return true;
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

      inline id_type
      id_of( value_type value ) const
      {
        return base_type::id_of( value );
      }

      inline bool
      is_reverse( value_type value ) const
      {
        return base_type::is_reverse( value );
      }

      inline rank_type
      size( ) const
      {
        return this->nodes.size();
      }

      inline void
      clear( )
      {
        this->id = 0;
        this->name.clear();
        this->nodes.clear();
      }

      inline void
      shrink_to_fit( )
      {
        this->name.shrink_to_fit();
        this->nodes.shrink_to_fit();
      }

    private:
      /* === DATA MEMBERS === */
      id_type id;
      string_type name;
      container_type nodes;
    };  /* --- end of class Path --- */

    using path_type = Path;
    using value_type = path_type;
    using container_type = std::vector< value_type >;
    using const_reference = typename container_type::const_reference;
    using const_iterator = typename container_type::const_iterator;
    using size_type = typename container_type::size_type;
    using rank_map_type = phmap::flat_hash_map< id_type, rank_type >;

    static inline void
    init_rank_map( rank_map_type& m )
    {
      // `dense_hash_map` requires to set empty key before any `insert` call.
      //m.set_empty_key( 0 );  // ID cannot be zero, so it can be used as empty key.
    }
  };  /* --- end of template class GraphPropertyTrait --- */

  /**
   *  @brief  Graph property class (dynamic).
   *
   *  Represent data associated with each graph, mainly paths.
   */
  template< typename TDir, uint8_t ...TWidths >
  class GraphProperty< Dynamic, TDir, TWidths... > {
  public:
    /* === TYPEDEFS === */
    using spec_type = Dynamic;
    using dir_type = TDir;
    using trait_type = GraphPropertyTrait< spec_type, dir_type, TWidths... >;
    using id_type = typename trait_type::id_type;
    using rank_type = typename trait_type::rank_type;
    using offset_type = typename trait_type::offset_type;
    using common_type = typename trait_type::common_type;
    using string_type = typename trait_type::string_type;
    using path_type = typename trait_type::path_type;
    using value_type = typename trait_type::value_type;
    using container_type = typename trait_type::container_type;
    using const_reference = typename trait_type::const_reference;
    using const_iterator = typename trait_type::const_iterator;
    using size_type = typename trait_type::size_type;
    using rank_map_type = typename trait_type::rank_map_type;
    using succinct_type = GraphProperty< Succinct, dir_type, TWidths... >;
    using dynamic_type = GraphProperty;

    /* === LIFECYCLE === */
    GraphProperty( )
      : max_id( 0 ), path_count( 0 )
    {
      trait_type::init_rank_map( this->path_rank );
    }

    GraphProperty( GraphProperty const& other ) = default;      /* copy constructor */
    GraphProperty( GraphProperty&& other ) noexcept = default;  /* move constructor */
    ~GraphProperty() noexcept = default;

    /* === ACCESSORS === */
    inline container_type const&
    get_paths( ) const
    {
      return this->paths;
    }

    inline rank_type
    get_path_count( ) const
    {
      return this->path_count;
    }

    /* === OPERATORS === */
    GraphProperty& operator=( GraphProperty const& other ) = default;      /* copy assignment operator */
    GraphProperty& operator=( GraphProperty&& other ) noexcept = default;  /* move assignment operator */

    /* === METHODS === */
    /**
     *  @brief  Return the rank of a path by its ID.
     *
     *  @param  id A path ID.
     *  @return The corresponding path rank.
     */
    inline rank_type
    id_to_rank( id_type id ) const
    {
      assert( id > 0 );
      auto found = this->path_rank.find( id );
      if ( found == this->path_rank.end() ) return 0;
      return found->second;
    }

    /**
     *  @brief  Return the ID of a path by its rank.
     *
     *  NOTE: This function assumes that path rank is within the range
     *  [1, path_count], otherwise the behaviour is undefined. The path rank
     *  should be verified beforehand.
     *
     *  @param  rank A path rank.
     *  @return The corresponding path ID.
     */
    inline id_type
    rank_to_id( rank_type rank ) const
    {
      assert( 0 < rank && rank <= this->path_count );
      return this->paths[ rank - 1 ].get_id();
    }

    /**
     *  @brief  Return the ID of the successor path in rank.
     *
     *  @param  id A path id.
     *  @return The path ID of the successor path of a path whose ID is `id` in the rank.
     */
    inline id_type
    successor_id( id_type id ) const
    {
      rank_type rank = this->id_to_rank( id );
      if ( rank == this->path_count ) return 0;
      return this->rank_to_id( rank + 1 );
    }

    inline id_type
    add_path( string_type name )
    {
      id_type new_id = this->add_path_imp( std::move( name ) );
      this->set_last_rank();
      return new_id;
    }

    template< typename TIter >
    inline id_type
    add_path( TIter n_begin, TIter n_end, string_type name="" )
    {
      id_type new_id = this->add_path_imp( n_begin, n_end, std::move( name ) );
      this->set_last_rank();
      return new_id;
    }

    template< typename TIter1, typename TIter2 >
    inline id_type
    add_path( TIter1 n_begin, TIter1 n_end, TIter2 o_begin, TIter2 o_end,
              string_type name="" )
    {
      id_type new_id = this->add_path_imp( n_begin, n_end, o_begin, o_end,
                                           std::move( name ) );
      this->set_last_rank();
      return new_id;
    }

    inline void
    extend_path( id_type pid, id_type nid, bool reversed=false )
    {
      this->path( pid ).add_node( nid, reversed );
    }

    template< typename TIter >
    inline void
    extend_path( id_type id, TIter n_begin, TIter n_end )
    {
      value_type& path = this->path( id );
      for ( ; n_begin != n_end; ++n_begin ) {
        path.add_node( *n_begin );
      }
    }

    template< typename TIter1, typename TIter2 >
    inline void
    extend_path( id_type id, TIter1 n_begin, TIter1 n_end, TIter2 o_begin, TIter2 o_end )
    {
      assert( n_end - n_begin == o_end - o_begin );
      value_type& path = this->path( id );
      for ( ; n_begin != n_end && o_begin != o_end; ++n_begin, ++o_begin ) {
        path.add_node( *n_begin, *o_begin );
      }
    }

    inline bool
    has_path( id_type id ) const
    {
      return this->path_rank.find( id ) != this->path_rank.end();
    }

    inline void
    flip_orientation( /*node*/ id_type node_id )
    {
      for ( auto& path : this->paths ) { path.flip_orientation( node_id ); }
    }

    template< typename TSet >
    inline void
    flip_orientation( TSet const& node_set )
    {
      for ( auto& path : this->paths ) { path.flip_orientation( node_set ); }
    }

    template< typename TCallback >
    inline bool
    for_each_path( TCallback callback,
                   rank_type rank=1 ) const
    {
      static_assert( std::is_invocable_r_v< bool, TCallback, rank_type, id_type >, "received a non-invocable as callback" );

      if ( rank > this->get_path_count() ) return true;

      for ( auto itr = this->paths.begin() + rank - 1;
            itr != this->paths.end(); ++itr ) {
        if ( !callback( rank, itr->get_id() ) ) return false;
        ++rank;
      }
      return true;
    }

    inline rank_type
    path_length( id_type id ) const
    {
      rank_type rank = this->id_to_rank( id );
      assert( rank != 0 );
      return this->paths[ rank - 1 ].size();
    }

    inline string_type
    path_name( id_type id ) const
    {
      rank_type rank = this->id_to_rank( id );
      assert( rank != 0 );
      return this->paths[ rank - 1 ].get_name( );
    }

    inline path_type const&
    path( id_type id ) const
    {
      rank_type rank = this->id_to_rank( id );
      assert( rank != 0 );
      return this->paths[ rank - 1 ];
    }

    inline void
    clear( )
    {
      this->paths.clear();
      this->path_rank.clear();
      this->max_id = 0;
      this->path_count = 0;
    }

    inline void
    shrink_to_fit( )
    {
      for ( auto& p : this->paths ) p.shrink_to_fit();
      this->paths.shrink_to_fit();
    }

  protected:
    /* === ACCESSORS === */
    inline container_type&
    get_paths( )
    {
      return this->paths;
    }

    /* === METHODS === */
    inline path_type&
    path( id_type id )
    {
      rank_type rank = this->id_to_rank( id );
      assert( rank != 0 );
      return this->paths[ rank - 1 ];
    }

    inline id_type
    add_path_imp( string_type name )
    {
      value_type path( ++this->max_id, std::move( name ) );
      this->paths.push_back( std::move( path ) );
      return this->max_id;
    }

    template< typename TIter >
    inline id_type
    add_path_imp( TIter n_begin, TIter n_end, string_type name="" )
    {
      value_type path( ++this->max_id, std::move( name ) );
      for ( ; n_begin != n_end; ++n_begin ) {
        path.add_node( *n_begin );
      }
      this->paths.push_back( std::move( path ) );
      return this->max_id;
    }

    template< typename TIter1, typename TIter2 >
    inline id_type
    add_path_imp( TIter1 n_begin, TIter1 n_end, TIter2 o_begin, TIter2 o_end,
                  string_type name="" )
    {
      assert( n_end - n_begin == o_end - o_begin );
      value_type path( ++this->max_id, std::move( name ) );
      for ( ; n_begin != n_end && o_begin != o_end; ++n_begin, ++o_begin ) {
        path.add_node( *n_begin, *o_begin );
      }
      this->paths.push_back( std::move( path ) );
      return this->max_id;
    }

  private:
    /* === DATA MEMBERS === */
    container_type paths;
    rank_map_type path_rank;
    id_type max_id;
    rank_type path_count;

    /* === METHODS === */
    inline void
    set_rank( const_iterator begin, const_iterator end )
    {
      assert( end - begin + this->path_count == this->paths.size() );
      for ( ; begin != end; ++begin ) {
        bool inserted;
        std::tie( std::ignore, inserted ) =
            this->path_rank.insert( { (*begin).get_id(), ++this->path_count } );
        assert( inserted );  // avoid duplicate insertion from upstream.
      }
    }

    inline void
    set_rank( )
    {
      this->set_rank( this->paths.begin(), this->paths.end() );
    }

    inline void
    set_last_rank( )
    {
      this->set_rank( this->paths.end() - 1, this->paths.end() );
    }
  };  /* --- end of template class GraphProperty --- */
}

#endif /* --- #ifndef GUM_GRAPH_PROP_DYNAMIC_HPP__ --- */
