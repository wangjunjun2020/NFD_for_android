/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2015,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mgmt/general-config-section.hpp"
#include "core/privilege-helper.hpp"

#include "tests/test-common.hpp"

namespace nfd {
namespace general {
namespace tests {

using namespace nfd::tests;

class GeneralConfigSectionFixture : public BaseFixture
{
public:
  ~GeneralConfigSectionFixture()
  {
#ifdef HAVE_PRIVILEGE_DROP_AND_ELEVATE
    // revert changes to s_normalUid/s_normalGid, if any
    PrivilegeHelper::s_normalUid = ::geteuid();
    PrivilegeHelper::s_normalGid = ::getegid();
#endif // HAVE_PRIVILEGE_DROP_AND_ELEVATE
  }
};

BOOST_FIXTURE_TEST_SUITE(MgmtGeneralConfigSection, GeneralConfigSectionFixture)

BOOST_AUTO_TEST_CASE(DefaultConfig)
{
  const std::string CONFIG =
    "general\n"
    "{\n"
    "}\n";

  ConfigFile configFile;

  general::setConfigFile(configFile);
  BOOST_CHECK_NO_THROW(configFile.parse(CONFIG, true, "test-general-config-section"));
}

#ifdef HAVE_PRIVILEGE_DROP_AND_ELEVATE

BOOST_AUTO_TEST_CASE(UserAndGroupConfig)
{
  const std::string CONFIG =
    "general\n"
    "{\n"
    "  user nobody\n"
    "  group nogroup\n"
    "}\n";

  ConfigFile configFile;

  general::setConfigFile(configFile);
  BOOST_CHECK_NO_THROW(configFile.parse(CONFIG, true, "test-general-config-section"));
}

BOOST_AUTO_TEST_CASE(NoUserConfig)
{
  const std::string CONFIG =
    "general\n"
    "{\n"
    "  group nogroup\n"
    "}\n";

  ConfigFile configFile;

  general::setConfigFile(configFile);
  BOOST_CHECK_NO_THROW(configFile.parse(CONFIG, true, "test-general-config-section"));
}

BOOST_AUTO_TEST_CASE(NoGroupConfig)
{
  const std::string CONFIG =
    "general\n"
    "{\n"
    "  user nobody\n"
    "}\n";

  ConfigFile configFile;

  general::setConfigFile(configFile);
  BOOST_CHECK_NO_THROW(configFile.parse(CONFIG, true, "test-general-config-section"));
}

#endif // HAVE_PRIVILEGE_DROP_AND_ELEVATE

static bool
checkExceptionMessage(const ConfigFile::Error& error, const std::string& expected)
{
  return error.what() == expected;
}

BOOST_AUTO_TEST_CASE(InvalidUserConfig)
{
  const std::string CONFIG =
    "general\n"
    "{\n"
    "  user\n"
    "}\n";

  ConfigFile configFile;
  general::setConfigFile(configFile);

  const std::string expected = "Invalid value for \"user\" in \"general\" section";
  BOOST_REQUIRE_EXCEPTION(configFile.parse(CONFIG, true, "test-general-config-section"),
                          ConfigFile::Error,
                          bind(&checkExceptionMessage, _1, expected));
}

BOOST_AUTO_TEST_CASE(InvalidGroupConfig)
{
  const std::string CONFIG =
    "general\n"
    "{\n"
    "  group\n"
    "}\n";

  ConfigFile configFile;
  general::setConfigFile(configFile);

  const std::string expected = "Invalid value for \"group\" in \"general\" section";
  BOOST_REQUIRE_EXCEPTION(configFile.parse(CONFIG, true, "test-general-config-section"),
                          ConfigFile::Error,
                          bind(&checkExceptionMessage, _1, expected));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace general
} // namespace nfd
