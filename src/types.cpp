#include <string>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "jira/types.hpp"
#include "utils.hpp"

using json = nlohmann::json;

auto types_logger = spdlog::stdout_color_mt("Jira Types Parser");

JiraUser* JiraUser::fromJSON(std::string json_string) {
    types_logger->trace("JiraUser::fromJSON() called for {}", json_string);
    JiraUser *user = new JiraUser();
    json json_data = json::parse(json_string);
    json_data.at("accountId").get_to(user->id);
    json_data.at("displayName").get_to(user->name);
    types_logger->debug("Parsed json -> user\n--id: {}\n--name: {}", user->id, user->name);
    return user;
}

JiraSprint* JiraSprint::fromJSON(std::string json_string) {
    types_logger->trace("JiraSprint::fromJSON() called for {}", json_string);
    JiraSprint *sprint = new JiraSprint();
    json json_data = json::parse(json_string);
    json_data.at("id").get_to(sprint->id);
    json_data.at("name").get_to(sprint->name);
    json_data.at("originBoardId").get_to(sprint->board_id);
    sprint->is_closed = !(json_data.at("state").get<std::string>().compare("closed"));
    if (json_data.contains("startDate")) {
        sprint->start_date = Utils::parseTimestapm(json_data.at("startDate").get<std::string>());
    }
    if (json_data.contains("endDate")) {
        sprint->end_date = Utils::parseTimestapm(json_data.at("endDate").get<std::string>());
    }
    if (json_data.contains("completeDate")) {
        sprint->complete_date = Utils::parseTimestapm(json_data.at("completeDate").get<std::string>());
    }
    // TODO: when class will be improved with uniq_ptr, complete_date print shall be wrapped with IF
    types_logger->debug("Parsed json -> sprint\n--name: {}\n--start date: {}\n--end date: {}\n--complete date: {}",
        sprint->name, Utils::timeToString(sprint->start_date), Utils::timeToString(sprint->end_date), Utils::timeToString(sprint->complete_date));
    return sprint;
}

JiraIssue* JiraIssue::fromJSON(std::string json_string) {
    types_logger->trace("JiraIssue::fromJSON() called for {}", json_string);
    JiraIssue *issue = new JiraIssue();
    json json_data = json::parse(json_string);
    json_data.at("id").get_to(issue->id);
    json_data.at("key").get_to(issue->key);
    if (!json_data.at("fields").at("customfield_10125").is_null()) {
        json_data.at("fields").at("customfield_10125").get_to(issue->story_points);
    }
    if (json_data.at("fields").at("issuetype").at("subtask").get<bool>()) {
        issue->type = IssueType::Subtask;
    } else {
        issue->type = static_cast<IssueType>(stoi(json_data.at("fields").at("issuetype").at("id").get<std::string>()));
    }
    if (json_data.at("fields").contains("assignee")) {
        json_data.at("fields").at("assignee").at("accountId").get_to(issue->assignee_id);
    }
    json_data.at("fields").at("summary").get_to(issue->title);
    issue->status = IssueStatus{
        .id = json_data.at("fields").at("status").at("id").get<std::string>(), 
        .name = json_data.at("fields").at("status").at("name").get<std::string>()};
    if (!json_data.at("fields").at("resolution").is_null()) {
        issue->resolved = true;
        issue->resolution_date = Utils::parseTimestapm(json_data.at("fields").at("resolutiondate").get<std::string>());
    }
    if (json_data.at("fields").contains("parent")) {
        json_data.at("fields").at("parent").at("id").get_to(issue->parent_id);
    }
    if (json_data.at("fields").contains("comment") && !json_data.at("fields").at("comment").at("comments").is_null()) {
        for(auto json_comment : json_data.at("fields").at("comment").at("comments")) {
            std::string message = json_comment.at("body").get<std::string>();
            message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());    // remove newlines
            Comment comment = {
                .id = json_comment.at("id").get<std::string>(),
                .authour_id = json_comment.at("author").at("accountId").get<std::string>(),
                .text = message,
                .published_date = Utils::parseTimestapm(json_comment.at("created").get<std::string>())};
            issue->comments.push_back(comment);
        }
    }
    if (json_data.at("fields").contains("subtasks") && !json_data.at("fields").at("subtasks").is_null()) {
        for(auto json_subtask : json_data.at("fields").at("subtasks")) {
            issue->subtasks_ids.push_back(json_subtask.at("id").get<std::string>());
        }
    }
    types_logger->debug("Parsed json -> issue\n--id: {}\n--key: {}\n--title: {}\n--type: {}\n--assignee: {}",
        issue->id, issue->key, issue->title, issue->type, issue->assignee_id);
    return issue;
}