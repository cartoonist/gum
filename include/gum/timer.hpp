/**
 *    @file  timer.hpp
 *   @brief  Module for performance measurement and running-time statistics.
 *
 *  This module provides necessary tools for measuring performance and capturing
 *  running-time statistics.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@uni-bielefeld.de>
 *
 *  @internal
 *       Created:  Thu Aug 24, 2023  13:42
 *  Organization:  Universität Bielefeld
 *     Copyright:  Copyright (c) 2023, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef GUM_TIMER_HPP__
#define GUM_TIMER_HPP__

#include <chrono>
#include <unordered_map>
#include <cassert>


namespace gum {
  typedef clock_t CpuClock;
  typedef std::chrono::steady_clock SteadyClock;

  template< typename TSpec = CpuClock >
    class TimerTraits;

  template< >
    class TimerTraits< std::chrono::steady_clock > {
      public:
        /* ====================  TYPE MEMBERS  ======================================= */
        typedef std::chrono::steady_clock clock_type;
        typedef std::chrono::microseconds duration_type;
        typedef duration_type::rep rep_type;
        /* ====================  DATA MEMBERS  ======================================= */
        constexpr static const char* unit_repr = "us";
        constexpr static const duration_type zero_duration =
          std::chrono::duration_cast< duration_type >( clock_type::duration::zero() );
        constexpr static const rep_type zero_duration_rep =
          TimerTraits::zero_duration.count();
        /* ====================  METHODS       ======================================= */
          static inline duration_type
        duration( clock_type::time_point end, clock_type::time_point start,
                  duration_type pre_elapsed=zero_duration )
        {
          return std::chrono::duration_cast< duration_type >( end - start ) + pre_elapsed;
        }

          static inline rep_type
        duration_rep( clock_type::time_point end, clock_type::time_point start,
                      duration_type pre_elapsed=zero_duration )
        {
          return TimerTraits::duration( end, start, pre_elapsed ).count();
        }

          static inline std::string
        duration_str( clock_type::time_point end, clock_type::time_point start,
                      duration_type pre_elapsed=zero_duration )
        {
          return std::to_string( TimerTraits::duration_rep( end, start, pre_elapsed ) ) +
              " " + TimerTraits::unit_repr;
        }
    };  /* ---  end of class TimerTraits  --- */

  template< >
    class TimerTraits< clock_t > {
      public:
        /* ====================  TYPE MEMBERS  ======================================= */
        struct ClockTraits {
          typedef clock_t time_point;
            static inline time_point
          now()
          {
            return clock();
          }
        };
        typedef ClockTraits clock_type;
        typedef float duration_type;
        typedef duration_type rep_type;
        /* ====================  DATA MEMBERS  ======================================= */
        constexpr static const char* unit_repr = "s";
        constexpr static const duration_type zero_duration = 0;
        constexpr static const rep_type zero_duration_rep = 0;
        /* ====================  METHODS       ======================================= */
          static inline duration_type
        duration( clock_type::time_point end, clock_type::time_point start,
                  duration_type pre_elapsed=zero_duration )
        {
          return static_cast< float >( end - start ) / CLOCKS_PER_SEC + pre_elapsed;
        }

          static inline rep_type
        duration_rep( clock_type::time_point end, clock_type::time_point start,
                      duration_type pre_elapsed=zero_duration )
        {
          return TimerTraits::duration( end, start, pre_elapsed );
        }

          static inline std::string
        duration_str( clock_type::time_point end, clock_type::time_point start,
                      duration_type pre_elapsed=zero_duration )
        {
          return std::to_string( TimerTraits::duration_rep( end, start, pre_elapsed ) ) +
              " " + TimerTraits::unit_repr;
        }
    };  /* ---  end of class TimerTraits  --- */

  template< >
    class TimerTraits< void > {
      public:
        /* === TYPE MEMBERS === */
        struct ClockTraits {
          typedef void* time_point;
          static inline time_point now() { return nullptr; }
        };
        typedef ClockTraits clock_type;
        typedef float duration_type;
        typedef duration_type rep_type;
        /* === DATA MEMBERS === */
        constexpr static const char* unit_repr = "s";
        constexpr static const duration_type zero_duration = 0;
        constexpr static const rep_type zero_duration_rep = 0;
        /* === METHODS === */
        constexpr static inline duration_type
        duration( clock_type::time_point, clock_type::time_point )
        {
          return TimerTraits::zero_duration;
        }

        constexpr static inline duration_type
        duration( clock_type::time_point, clock_type::time_point, duration_type )
        {
          return TimerTraits::zero_duration;
        }

        constexpr static inline rep_type
        duration_rep( clock_type::time_point, clock_type::time_point )
        {
          return TimerTraits::zero_duration_rep;
        }

        constexpr static inline rep_type
        duration_rep( clock_type::time_point, clock_type::time_point, duration_type )
        {
          return TimerTraits::zero_duration_rep;
        }

        constexpr static inline const char*
        duration_str( clock_type::time_point, clock_type::time_point )
        {
          return "0";
        }

        constexpr static inline const char*
        duration_str( clock_type::time_point, clock_type::time_point, duration_type )
        {
          return "0";
        }
    };  /* ---  end of class TimerTraits  --- */

  /**
   *  @brief  Timers for measuring execution time.
   *
   *  Measure the time period between its instantiation and destruction. The timers are
   *  kept in static table hashed by the timer name.
   */
  template< typename TClock = CpuClock >
    class Timer
    {
      public:
        /* ====================  TYPE MEMBERS  ======================================= */
        typedef TimerTraits< TClock > traits_type;
        typedef typename traits_type::clock_type clock_type;
        typedef typename traits_type::duration_type duration_type;
        typedef typename traits_type::rep_type rep_type;
        struct TimePeriod {
          typedef Timer timer_type;

          duration_type pre_elapsed;
          typename clock_type::time_point start;
          typename clock_type::time_point end;
          TimePeriod( ) : pre_elapsed( traits_type::zero_duration ),
                          start( traits_type::zero_duration ),
                          end( traits_type::zero_duration )
          { }

          /**
           *  @brief  Get the measured duration.
           */
            inline duration_type
          duration( ) const
          {
            return traits_type::duration( this->end, this->start, this->pre_elapsed );
          }

          /**
           *  @brief  Get the measured duration (arithmetic representation).
           */
            inline rep_type
          rep( ) const
          {
            return traits_type::duration_rep( this->end, this->start, this->pre_elapsed );
          }

          /**
           *  @brief  Get the measured duration (string representation).
           */
            inline std::string
          str( ) const
          {
            return traits_type::duration_str( this->end, this->start, this->pre_elapsed );
          }

          /**
           *  @brief  Get time lap for an ongoing timer.
           */
            inline TimePeriod
          get_lap( ) const
          {
            TimePeriod period( *this );
            if ( period.end <= period.start ) period.end = clock_type::now();
            return period;
          }
        };
        typedef TimePeriod period_type;
        /* ====================  STATIC DATA   ======================================= */
        constexpr static const char* unit_repr = traits_type::unit_repr;
        constexpr static const duration_type zero_duration = traits_type::zero_duration;
        constexpr static const rep_type zero_duration_rep = traits_type::zero_duration_rep;
        /* ====================  LIFECYCLE     ======================================= */
        /**
         *  @brief  Timer constructor.
         *
         *  @param  name The name of the timer to start.
         *
         *  If timer does not exist it will be created.
         */
        Timer( const std::string& name )
        {
          this->timer_name = name;
          auto found = get_timers().find( this->timer_name );
          if ( found != get_timers().end() ) {
            assert( found->second.end >= found->second.start );
            found->second.pre_elapsed = found->second.duration();
            found->second.start = clock_type::now();
          } else get_timers()[ this->timer_name ].start = clock_type::now();
        }  /* -----  end of method Timer  (constructor)  ----- */

        /**
         *  @brief  Timer destructor.
         *
         *  Stop the timer as the Timer object dies.
         */
        ~Timer()
        {
          get_timers()[ this->timer_name ].end = clock_type::now();
        }  /* -----  end of method ~Timer  (destructor)  ----- */
        /* ====================  METHODS       ======================================= */
        /**
         *  @brief  static getter function for static timers.
         */
          static inline std::unordered_map< std::string, TimePeriod >&
        get_timers( )
        {
          static std::unordered_map< std::string, TimePeriod > timers;
          return timers;
        }  /* -----  end of method get_timers  ----- */

        /**
         *  @brief  Get the timer duration by name.
         *
         *  @param  name The name of the timer to start.
         *  @return the duration represented by requested timer.
         *
         *  Get the duration represented by the timer.
         */
          static inline duration_type
        get_duration( const std::string& name )
        {
          return get_timers()[ name ].duration();
        }  /* -----  end of method get_duration  ----- */

        /**
         *  @brief  Get the timer duration (arithmetic representation) by name.
         *
         *  @param  name The name of the timer to start.
         *  @return arithmetic representation of the requested timer duration.
         *
         *  Get the arithmetic representation of the requested timer duration.
         */
          static inline rep_type
        get_duration_rep( const std::string& name )
        {
          return get_timers()[ name ].rep();
        }  /* -----  end of method get_duration  ----- */

        /**
         *  @brief  Get the timer duration (string representation) by name.
         *
         *  @param  name The name of the timer to start.
         *  @return string representation of the requested timer duration.
         *
         *  Get the string representation of the requested timer duration.
         */
          static inline std::string
        get_duration_str( const std::string& name )
        {
          return get_timers()[ name ].str();
        }  /* -----  end of method get_duration  ----- */

        /**
         *  @brief  Get time lap for an ongoing timer.
         *
         *  @param  name The name of the timer.
         *  @return the duration since 'start' to 'now' if the timer is not finished;
         *          otherwise it returns the duration of the timer.
         */
          static inline duration_type
        get_lap_duration( const std::string& name )
        {
          return get_timers()[ name ].get_lap().duration();
        }  /* -----  end of method get_lap  ----- */

        /**
         *  @brief  Get time lap for an ongoing timer (arithmetic representation).
         *
         *  @param  name The name of the timer.
         *  @return the duration since 'start' to 'now' if the timer is not finished;
         *          otherwise it returns the duration of the timer.
         */
          static inline rep_type
        get_lap_rep( const std::string& name )
        {
          return get_timers()[ name ].get_lap().rep();
        }  /* -----  end of method get_lap  ----- */

        /**
         *  @brief  Get time lap for an ongoing timer (string representation).
         *
         *  @param  name The name of the timer.
         *  @return the duration since 'start' to 'now' if the timer is not finished;
         *          otherwise it returns the duration of the timer.
         */
          static inline std::string
        get_lap_str( const std::string& name )
        {
          return get_timers()[ name ].get_lap().str();
        }  /* -----  end of method get_lap  ----- */
      protected:
        /* ====================  DATA MEMBERS  ======================================= */
        std::string timer_name;    /**< @brief The timer name of the current instance. */
    };  /* ---  end of class Timer  --- */

  template< >
    class Timer< void >
    {
    public:
      /* === TYPE MEMBERS === */
      typedef TimerTraits< void > traits_type;
      typedef typename traits_type::clock_type clock_type;
      typedef typename traits_type::duration_type duration_type;
      typedef typename traits_type::rep_type rep_type;
      struct TimePeriod {
        typedef Timer timer_type;

          constexpr inline duration_type
        duration( ) const
        {
          return traits_type::zero_duration;
        }

          constexpr inline rep_type
        rep( ) const
        {
          return traits_type::zero_duration_rep;
        }

          constexpr inline const char*
        str( ) const
        {
          return "0";
        }

          constexpr inline TimePeriod
        get_lap( ) const
        {
          return TimePeriod();
        }
      };
      typedef TimePeriod period_type;
      /* === LIFECYCLE === */
      constexpr Timer( std::string const& ) { }
      constexpr Timer( ) { }
      /* === METHODS === */
      static inline std::unordered_map< std::string, TimePeriod >
      get_timers( )
      {
        return std::unordered_map< std::string, TimePeriod >{};
      }
      constexpr static inline duration_type get_duration( const std::string& )
      {
        return traits_type::zero_duration;
      }
      constexpr static inline rep_type get_duration_rep( const std::string& )
      {
        return traits_type::zero_duration_rep;
      }
      constexpr static inline const char* get_duration_str( const std::string& )
      {
        return "0";
      }
      constexpr static inline duration_type get_lap_duration( const std::string& )
      {
        return traits_type::zero_duration;
      }
      constexpr static inline rep_type get_lap_rep( const std::string& )
      {
        return traits_type::zero_duration_rep;
      }
      constexpr static inline const char* get_lap_str( const std::string& )
      {
        return "0";
      }
    };  /*  --- end of class Timer --- */
}  /* --- end of namespace gum --- */

#endif  /* --- #ifndef GUM_STATS_HPP__ --- */
