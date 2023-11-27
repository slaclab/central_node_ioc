#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp> 

#include <iostream>
#include "central_node_database_tables.h"
#include "central_node_engine.h"

BOOST_AUTO_TEST_SUITE(BYPASS_TESTS)

void bypass_test_helper(BypassType type, uint32_t bypassUntil, uint32_t id, uint32_t stateId = 0) {
  time_t now;
  time(&now);
  uint32_t expirationTime = bypassUntil;
  if (expirationTime > 0) {
    expirationTime += now;
  }
  else {
    expirationTime = 0;
  }
  if (type == BYPASS_APPLICATION) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      Engine::getInstance().getBypassManager()->setBypass(BYPASS_APPLICATION, id, 0, expirationTime);
    }
  }
  else {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      Engine::getInstance().getBypassManager()->bypassFault(id, stateId, expirationTime);
    }
  }

}

BOOST_AUTO_TEST_CASE(digital_fault_bypass)
{
  // BOOST_TEST_MESSAGE("\n*** Testing all digital fault bypasses ***\n");
  uint32_t bypassUntil = 2; // must be > 1
  uint32_t stateId = 0;
  uint32_t delay = 1;
  DbFaultMapPtr faults = Engine::getInstance().getCurrentDb()->faults;
  for (DbFaultMap::iterator faultIt = faults->begin();
      faultIt != faults->end();
      faultIt++)
  {
    DbFaultPtr fault = (*faultIt).second;
    if (fault->faultInputs->begin()->second->analogChannel) {
      continue;
    }
    DbFaultStateMapPtr faultStates = fault->faultStates;
    for (DbFaultStateMap::iterator curState = faultStates->begin();
        curState != faultStates->end();
        curState++) 
      {
        if (!(*curState).second->active) {
          stateId = (*curState).second->id;
          break;
        }
      }
    std::string failMsg = "FAILED @ faultId=" + std::to_string(fault->id);
    BOOST_CHECK_MESSAGE(stateId != 0, failMsg);
    bypass_test_helper(BYPASS_DIGITAL, bypassUntil, fault->id, stateId);
    failMsg += " stateId=" + std::to_string(stateId);
    sleep(delay);

    BOOST_CHECK_MESSAGE(faults->at(fault->id)->bypassed == true, failMsg);
    BOOST_CHECK_MESSAGE(faultStates->at(stateId)->active == true, failMsg);

    sleep(bypassUntil + delay); // Let bypass expire
    BOOST_CHECK_MESSAGE(faults->at(fault->id)->bypassed == false, failMsg);
    
  }
}

BOOST_AUTO_TEST_CASE(analog_fault_bypass)
{
  // BOOST_TEST_MESSAGE("\n*** Testing all analog fault bypasses ***\n");
  uint32_t bypassUntil = 2;
  uint32_t delay = 1; // used for sleep()

  DbFaultMapPtr faults = Engine::getInstance().getCurrentDb()->faults;
  for (DbFaultMap::iterator faultIt = faults->begin();
      faultIt != faults->end();
      faultIt++)
  {
    DbFaultPtr fault = (*faultIt).second;
    if (fault->faultInputs->begin()->second->digitalChannel) {
      continue;
    }

    std::string failMsg = "FAILED @ faultId=" + std::to_string(fault->id);
    bypass_test_helper(BYPASS_ANALOG, bypassUntil, fault->id);

    BOOST_CHECK_MESSAGE(faults->at(fault->id)->bypassed == true, failMsg);
    
    sleep(bypassUntil + delay); // Let bypass expire
    BOOST_CHECK_MESSAGE(faults->at(fault->id)->bypassed == false, failMsg);
  }


}

BOOST_AUTO_TEST_CASE(app_card_bypass)
{
  // BOOST_TEST_MESSAGE("\n*** Testing all application card bypasses ***\n");
  uint32_t bypassUntil = 2;
  uint32_t delay = 1; // used for sleep()

  DbApplicationCardMapPtr appCards = Engine::getInstance().getCurrentDb()->applicationCards;
  for (DbApplicationCardMap::iterator appCardIt = appCards->begin();
      appCardIt != appCards->end();
      appCardIt++)
  {
    DbApplicationCardPtr appCard = (*appCardIt).second;
      
    std::string failMsg = "FAILED @ appCardId=" + std::to_string(appCard->id);
    bypass_test_helper(BYPASS_APPLICATION, bypassUntil, appCard->id);

    sleep(delay);
    BOOST_TEST(appCards->at(appCard->id)->bypassed == true, failMsg);

    sleep(bypassUntil + delay); // Let bypass expire
    BOOST_TEST(appCards->at(appCard->id)->bypassed == false, failMsg);
  }

}

BOOST_AUTO_TEST_SUITE_END()
