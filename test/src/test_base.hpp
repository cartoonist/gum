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

#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_template_test_macros.hpp"

#include "test_config.hpp"

#define TEMPLATE_SCENARIO TEMPLATE_TEST_CASE
#define TEMPLATE_SCENARIO_SIG TEMPLATE_TEST_CASE_SIG

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR PROJECT_SOURCE_DIR "/test/data"
#endif

static const std::string test_data_dir( TEST_DATA_DIR );

#endif  /* --- #ifndef GUM_TEST_BASE_HPP__ --- */
