/**
 *    @file  test_base.hpp
 *   @brief  Base header file for test sub-modules.
 *
 *  This header file should be included in any test sub-module.
 *
 *  @author  Ali Ghaffaari (\@cartoonist), <ali.ghaffaari@mpi-inf.mpg.de>
 *
 *  @internal
 *       Created:  Fri Aug 23, 2019  11:39
 *  Organization:  Max-Planck-Institut fuer Informatik
 *     Copyright:  Copyright (c) 2019, Ali Ghaffaari
 *
 *  This source code is released under the terms of the MIT License.
 *  See LICENSE file for more information.
 */

#ifndef  GUM_TEST_BASE_HPP__
#define  GUM_TEST_BASE_HPP__

#include <string>
#include <random>
#include <iostream>

#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_template_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include <catch2/generators/catch_generators_range.hpp>
#include "catch2/catch_get_random_seed.hpp"
#include "catch2/reporters/catch_reporter_event_listener.hpp"
#include "catch2/reporters/catch_reporter_registrars.hpp"

#include "test_config.hpp"

#define TEMPLATE_SCENARIO TEMPLATE_TEST_CASE
#define TEMPLATE_SCENARIO_SIG TEMPLATE_TEST_CASE_SIG

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR PROJECT_SOURCE_DIR "/test/data"
#endif

namespace rnd {
  inline std::random_device&
  get_rd()
  {
    thread_local static std::random_device rd;
    return rd;
  }

  inline unsigned int&
  get_iseed()
  {
    thread_local static unsigned int iseed = get_rd()();
    return iseed;
  }

  inline std::mt19937&
  get_rgn()
  {
    thread_local static std::mt19937 rgn( get_iseed() );
    return rgn;
  }

  inline void
  set_seed( unsigned int seed )
  {
    if ( seed != 0 ) {
      get_iseed() = seed;
      get_rgn().seed( seed );
    }
  }
}  /* ---  end of namespace rnd  --- */

class MyEventListener : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void
    set_rnd_seed()
    {
      auto seed = Catch::getSeed();
      if ( seed != 0 ) {
        rnd::set_seed( seed );
      }
    }

    void
    testRunStarting( Catch::TestRunInfo const& ) override
    {
      this->set_rnd_seed();
    }
};

CATCH_REGISTER_LISTENER( MyEventListener )

static const std::string test_data_dir( TEST_DATA_DIR );

#endif  /* --- #ifndef GUM_TEST_BASE_HPP__ --- */
