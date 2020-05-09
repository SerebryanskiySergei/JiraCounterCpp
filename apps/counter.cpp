#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "jira/jira_client.hpp"

using json = nlohmann::json;

auto app_logger = spdlog::stdout_color_mt("Application");

int main() {
    app_logger->set_pattern("[Application] [%^%l%$] %v");
    app_logger->set_level(spdlog::level::info);
    // Open file with params
    std::ifstream params_file {"params.json"};
    json params = json::parse(params_file);

    // Connect to Jira Cleint using username:token for auth
    JiraClient *client = new JiraClient(
        params.at("jira_url"), 
        params.at("username"), 
        params.at("token"));
    
    // =========================================
    // Get data from Jira API
    // =========================================
    
    // get users for each entity in json->names array
    std::vector<JiraUser*> persons;
    for(auto name = params.at("names").begin(); name != params.at("names").end(); ++name) {
        persons.push_back(client->getPerson(*name));
    }
    std::vector<JiraSprint*> sprints;
    if (params.at("period").at("type") == "names") {
        sprints = client->getSprints(
            params.at("board_with_sprints"),
            params.at("period").at("sprint_names")
        );
    } else if (params.at("period").at("type") == "dates") {
        // get all sprints between two dates
        sprints = client->getSprints(
            params.at("board_with_sprints"),
            params.at("period").at("start_date"),
            params.at("period").at("end_date")
        );
    } else {
        throw std::invalid_argument("Incorrect period type provided. Can be 'names' or 'dates'.");
    }
    // count results for each person in each sprint
    std::map<JiraSprint*, std::map<JiraUser*, PersonalResult*>> results;
    for(auto sprint : sprints) {
        std::map<JiraUser*, PersonalResult*> sprint_results;
        for(auto person : persons) {
            sprint_results.insert({person, client->getPersonResults(*person, *sprint)});
        }
        results.insert({sprint, sprint_results});
    }
    app_logger->info("Finished counting results!");

    
    //======================================
    // Print results
    //======================================
    for(auto sprint_results : results) {
        app_logger->info("=========================================================");
        app_logger->info("Results for sprint: {}", sprint_results.first->name);
        app_logger->info("=========================================================");
        for(auto person_result : sprint_results.second) {
            app_logger->info("{} results: ", person_result.first->name);
            int subtasks_count = 0;
            int bugs_count = 0;
            int issues_count = 0;
            int other_count = 0;
            int comment_chars = 0;
            int story_points_finished = 0;
            for(auto issue : person_result.second->finished) {
                switch (issue->type) {
                case IssueType::Subtask:
                    subtasks_count ++;
                    story_points_finished += issue->story_points;
                    break;
                case IssueType::Task:
                case IssueType::Enabler:
                case IssueType::Debt:
                     issues_count ++;
                     story_points_finished += issue->story_points;
                     break;
                case IssueType::Bug:
                    bugs_count ++;
                    story_points_finished += issue->story_points;
                    break;
                case IssueType::Story:
                    issues_count++;
                    break;
                default:
                    app_logger->info("Not sure how to count {} - {}", issue->key, issue->title);
                    other_count ++;
                    break;
                }
         
            }
            for (auto comment : person_result.second->comments_written) {
                comment_chars += comment.text.length();
            }
            app_logger->info("__Finished subtasks: {}", subtasks_count);
            app_logger->info("__Finished issues: {}", issues_count);
            app_logger->info("__Finished bugs: {}", bugs_count);
            app_logger->info("__Finished others: {}", other_count);
            app_logger->info("__Story Points: {}", story_points_finished);
            app_logger->info("__Comments written: {}", person_result.second->comments_written.size());
            app_logger->info("__Comments lines: {} lines with {} total characters", comment_chars/125, comment_chars);
            app_logger->info("-----------------------------------------------------");
        }
    }

    return 0;
}