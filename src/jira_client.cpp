#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "jira/jira_client.hpp"
#include "utils.hpp"

using json = nlohmann::json;
using namespace cpr;
using namespace std;

const std::string JIRA_API_URL = "rest/api/3";
const std::string AGILE_API_URL = "rest/agile/1.0";

auto client_logger = spdlog::stdout_color_mt("Jira Client");

JiraClient::JiraClient(const string base_url, const string username, const string api_token) {
    client_logger->set_level(spdlog::level::info);
    client_logger->set_pattern("[Jira Client] [%^%l%$] %v");

    this->api_url = base_url;
    this->agile_url = base_url;
    if (this->api_url.back() != '/') {
        this->api_url += "/";
        this->agile_url += "/";
    }
    this->api_url += JIRA_API_URL;
    this->agile_url += AGILE_API_URL;
    this->user = username;
    this->token = api_token;
    // make a test requests in order to verify connection
    auto url = Url{this->api_url + "/myself"};
    auto auth = Authentication(this->user, this->token);
    auto response = Get(url, auth);
    if (response.status_code != 200) {
        throw std::invalid_argument("Incorrect url, username or token was provided.");
    }
    client_logger->info("Jira Client created for: {} with url: {}", this->user, this->api_url);
}

JiraClient::~JiraClient() {}

JiraUser* JiraClient::getPerson(const std::string surname) {
    client_logger->trace("JiraClient::getPerson() called for name {}", surname);
    auto url = Url{this->api_url + "/user/search"};
    auto auth = Authentication(this->user, this->token);
    auto params = Parameters({{"query", surname}});
    auto response = Get(url, auth, params);
    if (response.status_code != 200) {
        throw std::logic_error(std::string("Get users returned incorrect code: ") + to_string(response.status_code));
    }
    json search_result = json::parse(response.text);
    if (search_result.size() > 1) {
        std::string message = "Found more than 1 user with provided surname:";
        for(auto user : search_result){
            message.append(" -" + user.at("displayName").get<std::string>());
        }
        throw std::invalid_argument(message);
    }
    else if (search_result.size() == 0) throw std::invalid_argument("User was not found with provided surname - " + surname);
    JiraUser* user = JiraUser::fromJSON(search_result[0].dump());
    client_logger->info("Person found for name {} with id {}", user->name, user->id);
    return user;
}

std::vector<JiraSprint*> JiraClient::getSprints(const std::string board_name, std::set<std::string> sprint_names) {
    client_logger->trace("JiraClient::getSprints() called for board {}", board_name);
    vector<JiraSprint*> sprints;
    // Looking for right Jira Board
    auto url = Url{this->agile_url + "/board"};
    auto auth = Authentication(this->user, this->token);
    auto response = Get(url, auth);
    if (response.status_code != 200) {
        throw std::logic_error(std::string("Get board returned incorrect code: ") + to_string(response.status_code));
    }
    // client_logger->info(response.text);
    json all_boards = json::parse(response.text);
    json board;
    for (auto& element : all_boards["values"]) {
        if (board_name.compare(element["name"].get<string>()) == 0) {
            board = element;
        }
    }
    if (board.is_null()) {
        throw std::logic_error(std::string("Cannot find a board with namee: ") + board_name);
    }
    // Looking for all sprints in the Board that will fit dates
    url = Url{this->agile_url + "/board/" + to_string(board.at("id").get<int>()) + "/sprint" };
    response = Get(url, auth);
    if (response.status_code != 200) {
        throw std::logic_error(std::string("Get sprints returned incorrect code: ") + to_string(response.status_code));
    }
    json all_sprints = json::parse(response.text);
    for (auto& element : all_sprints["values"]) {
        JiraSprint* sprint = JiraSprint::fromJSON(element.dump());
        // filter sprint by date
        if (sprint_names.find(sprint->name) != sprint_names.end()) {
            client_logger->info("Found sprint: {}\n--Started at: {}\n--Ended at: {}", sprint->name, Utils::timeToString(sprint->start_date), Utils::timeToString(sprint->end_date));
            client_logger->info("Taking issues for the sprint ...");
            auto url = Url{this->agile_url + "/board/" + to_string(sprint->board_id) +"/sprint/" +  to_string(sprint->id) + "/issue"};
            auto auth = Authentication(this->user, this->token);
            auto params = Parameters({{"maxResults", MAX_ISSUES_IN_REQUEST}});
            auto response = Get(url, auth, params);
            if (response.status_code != 200) {
                throw std::logic_error(std::string("Get users returned incorrect code: ") + to_string(response.status_code));
            }
            json search_result = json::parse(response.text);
            for(auto json_issue : search_result["issues"]) {
                JiraIssue* issue = JiraIssue::fromJSON(json_issue.dump());
                client_logger->debug("Found issue {}", issue->key);
                sprint->issues.push_back(issue);
            }
            sprints.push_back(sprint);
        }
    }
    if (sprints.size() == 0) {
        client_logger->warn("Are you sure that provided sprint names were correct? No sprints matches names was found.");
    }
    return sprints;
}



std::vector<JiraSprint*> JiraClient::getSprints(const string board_name, const string start_date, const string end_date){
    client_logger->trace("JiraClient::getSprints() called for board {} between {} and {}", board_name, start_date, end_date);
    vector<JiraSprint*> sprints;
    time_t request_start_date = Utils::parseTimestapm(start_date);
    time_t request_end_date = Utils::parseTimestapm(end_date);
    // Looking for right Jira Board
    auto url = Url{this->agile_url + "/board"};
    auto auth = Authentication(this->user, this->token);
    auto response = Get(url, auth);
    if (response.status_code != 200) {
        throw std::logic_error(std::string("Get board returned incorrect code: ") + to_string(response.status_code));
    }
    // client_logger->info(response.text);
    json all_boards = json::parse(response.text);
    json board;
    for (auto& element : all_boards["values"]) {
        if (board_name.compare(element["name"].get<string>()) == 0) {
            board = element;
        }
    }
    if (board.is_null()) {
        throw std::logic_error(std::string("Cannot find a board with namee: ") + board_name);
    }
    // Looking for all sprints in the Board that will fit dates
    url = Url{this->agile_url + "/board/" + to_string(board.at("id").get<int>()) + "/sprint" };
    response = Get(url, auth);
    if (response.status_code != 200) {
        throw std::logic_error(std::string("Get sprints returned incorrect code: ") + to_string(response.status_code));
    }
    json all_sprints = json::parse(response.text);
    for (auto& element : all_sprints["values"]) {
        JiraSprint* sprint = JiraSprint::fromJSON(element.dump());
        // filter sprint by date
        bool started_inside_requested_period = difftime(sprint->start_date, request_start_date) > 0;
        bool ended_inside_requested_period = difftime(sprint->complete_date, request_end_date) < 0;
        client_logger->debug("Checking that sprint {} was\n--started after {} - {}\n--ended before {} - {}",
                        sprint->name,
                        Utils::timeToString(sprint->start_date), started_inside_requested_period,
                        Utils::timeToString(sprint->end_date), ended_inside_requested_period);
        if (started_inside_requested_period && ended_inside_requested_period) {
            client_logger->info("Found sprint: {}\n--Started at: {}\n--Ended at: {}", sprint->name, Utils::timeToString(sprint->start_date), Utils::timeToString(sprint->end_date));
            client_logger->info("Taking issues for the sprint ...");
            auto url = Url{this->agile_url + "/board/" + to_string(sprint->board_id) +"/sprint/" +  to_string(sprint->id) + "/issue"};
            auto auth = Authentication(this->user, this->token);
            auto params = Parameters({{"maxResults", MAX_ISSUES_IN_REQUEST}});
            auto response = Get(url, auth, params);
            if (response.status_code != 200) {
                throw std::logic_error(std::string("Get users returned incorrect code: ") + to_string(response.status_code));
            }
            json search_result = json::parse(response.text);
            for(auto json_issue : search_result["issues"]) {
                JiraIssue* issue = JiraIssue::fromJSON(json_issue.dump());
                client_logger->debug("Found issue {}", issue->key);
                sprint->issues.push_back(issue);
            }
            sprints.push_back(sprint);
        }
    }
    if (sprints.size() == 0) {
        client_logger->warn("Are you sure that start and end date a correct? No sprints inside period {} - {} was found.", Utils::timeToString(request_start_date), Utils::timeToString(request_end_date));
    }
    return sprints;
}

PersonalResult* JiraClient::getPersonResults(const JiraUser& person, const JiraSprint& sprint) {
    client_logger->info("Looking at sprint: {}", sprint.name);
    client_logger->info("Counting issues for: {}", person.name);
    PersonalResult *results = new PersonalResult();
    for(auto issue : sprint.issues) {
        bool reviewed;
        for(auto comment: issue->comments) {
            if (difftime(comment.published_date, sprint.end_date) < 0 && difftime(comment.published_date, sprint.start_date) > 0 ) {
                if (comment.authour_id == person.id) {
                    client_logger->debug("Found comment for {} with text: {}", person.name, comment.text.substr(0, 15));
                    results->comments_written.push_back(comment);
                }
                if (comment.authour_id == person.id && (issue->assignee_id != person.id)) {
                    reviewed = true;
                    client_logger->debug("Found review for {} in issue {}", person.name, issue->key);
                }
            }
        }
        if (reviewed) {
            results->issues_reviwed++;
        }
        if (issue->assignee_id == person.id) {
            if (issue->resolved && ( difftime(issue->resolution_date, sprint.end_date) < 0)) {
                results->finished.push_back(issue);
            } else {
                results->not_finished.push_back(issue);
            }
        }
    }
    client_logger->info("Finished issues: {}", results->finished.size());
    return results;
}
