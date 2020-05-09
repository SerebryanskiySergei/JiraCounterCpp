/**
 * @file jira_client.hpp
 * @author Sergey Serebryanskiy (serebryanskiysergei@gmail.com)
 * @brief Classes and methods for taking metrics for users in sprints
 * @version 0.1
 * @date 2020-05-31
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef JIRA_CLIENT_H_
#define JIRA_CLIENT_H_

#include <jira/types.hpp>
#include <vector>
#include <set>

class JiraClient {
    public:
        
        /**
         * @brief Construct a new Jira Client object
         * 
         * @param [in] base_url URL to the atlassian jira website
         * @param [in] username username which will be used makigna connection to jira
         * @param [in] api_token token for the username for a connection with jira api
         */
        JiraClient(const std::string base_url, const std::string username, const std::string api_token);
        
        /**
         * @brief Destroy the Jira Client object
         */
        ~JiraClient();
        
        /**
         * Find a user in Jira database
         *
         * @param [in] surname the surname of Jira user for a search. Must be the same with Jira surname.
         *
         * @return the user object that has required surname
         *
         * @throws std::invalid_argument Thrown if a user with `surname` was not found.
         * @throws std::invalid_argument Thrown if more than one user with `surname` was found.
         *
         * @exceptsafe Strong exception guarantee.
         */
        JiraUser* getPerson(const std::string surname);
        
        /**
         * @brief Get the Sprint that matches exact name
         * 
         * @param [in] board_name Name of the board that will be used a source of the sprints
         * @param [in] sprint_names List of all names of sprints to search
         * @return std::vector<JiraSprint*> List of filtered by date sprints
         */
        std::vector<JiraSprint*> getSprints(const std::string board_name, std::set<std::string> sprint_names);
        
        /**
         * @brief Get the Sprints that took place between start and end date
         * 
         * @param [in] board_name Name of the board that will be used a source of the sprints
         * @param [in] start_date Counting sprints started after this date
         * @param [in] end_date Counting sprints ended before this date
         * @return std::vector<JiraSprint*> List of filtered by date sprints
         */
        std::vector<JiraSprint*> getSprints(const std::string board_name, const std::string start_date, const std::string end_date);
        
        /**
         * @brief Get reults for the person in the exact sprint
         * 
         * @param [in] user For whom method will count results
         * @param [in] sprint Sprint in which method will look for results
         * @return PersonalResult* Object with all results
         */
        PersonalResult* getPersonResults(const JiraUser& user, const JiraSprint& sprint);
    private:
        std::string api_url;
        std::string agile_url;
        std::string user;
        std::string token;
        
};

#endif // JIRA_CLIENT_H_