#include <gtest/gtest.h>
#include <vector>

#include "jira/jira_client.hpp"
#include "jira/types.hpp"

TEST(JiraClient, GetPersonalResulst) {
    JiraClient *client = new JiraClient(
        "https://macnicatech.atlassian.net/", 
        "sergey.serebryanskiy@macnica.com",
        "m6sxbiZKVqakD1iDN0DG1662");
    JiraUser *person = client->getPerson("nikita");
    std::vector<JiraSprint*> sprints = client->getSprints("MPA1 board", "2020-03-22T13:00:00", "2020-05-10:T13:00:00");
    client->getPersonResults(*person, *sprints[0]);
    EXPECT_EQ(person->name, "Nikita Nagorny");
}