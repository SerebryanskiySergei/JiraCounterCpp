/**
 * @file types.hpp
 * @author  Sergey Serebryanskiy (serebryanskiysergei@gmail.com.com)
 * @brief Classes and structures used in Jira client
 * @version 0.1
 * @date 2020-05-31
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <vector>
#include <string>
#include <ctime>
#include <map>

// Jira has a limitation for MAX of items that will be returned by api request 
const std::string MAX_ISSUES_IN_REQUEST = "200";

/**
 * @brief Issues's statuses like Done, New, In progress etc
 */
struct IssueStatus {
    std::string id;     /** < ID of the status in Jira database */
    std::string name;   /** < Name os the status visible for users */
};

/**
 * @brief Comment for the issue
 * 
 * Comment that was written at @published_date by @author_id.
 * Contains a whole message's text.
 */
struct Comment {
    std::string id;             /** < ID of the comment in a Jira databes */
    std::string authour_id;     /** < ID of author of the comemnt */
    std::string text;           /** < Full message's text */
    time_t published_date;      /** < Time when the comment was published (published, not updated!) */
};

/**
 * @brief Type of the issue like Bug, User-story etc
 * 
 * Each type has a value which is corresponds to the ID of type in 
 * a Jira's database at 16-05-2020.
 * 
 * @note When types will be changed in the Jira settings, it will cause problems with the counter here.
 * @warning If you think that scripts doesn't count everything right, this place is a first thing to check.
 */
enum IssueType {
    Epic = 10002,           /** < Top level Epic */
    Bug = 10004,            /** < Bug */
    Task = 10000,           /** < Regular Task */
    Story = 10003,          /** < User-Story */
    Subtask = 10001,        /** < Subtask of any issue */
    Debt = 10343,           /** < Technical Debt */
    Improvement = 10005,    /** < Suggestion for Improvement */
    NewFeature = 10006,     /** < New Feature */
    Support = 10100,        /** < Support task */
    Enabler = 10344,        /** < Enabler */
    Test = 10200            /** < Test task */
};


/**
 * @brief Jira User partial representation
 */
class JiraUser {
    public:
        std::string id;     /** < ID of the user in Jira Database */
        std::string name;   /** < name displayed of the website */
        /**
         * @brief Construct a new Jira User object
         * 
         */
        JiraUser() = default;
        /**
         * @brief Destroy the Jira User object
         * 
         */
        ~JiraUser();
        /**
         * @brief Creates a JiraUser object from JSON representation
         * 
         * Provided json string will be parsed and all fields for a new object
         * will be taken from the json.
         * 
         * @param [in] json_string JSON representation of the user
         * @return JiraUser* new object of user with fields values from json
         */
        static JiraUser* fromJSON(const std::string json_string);
};

/**
 * @brief Jira Issue partial representation
 */
class JiraIssue {
    public:
        std::string id;                 /** < ID of the issue in Jira Database*/
        std::string parent_id;          /** < ID op issue's parent like epic or issue for subtask */
        IssueType type;                 /** < Type of the issue */
        std::string key;                /** < Key of the issue like MPA1-132 */
        std::string title;              /** < Issue's title */
        std::string assignee_id;        /** < ID of assigned user */
        IssueStatus status;             /** < Curernt statys of the issue */
        int story_points = 0;           /** < Story Points estimation for the issue */
        bool resolved = false;          /** < Issue was resolved ? true/false */
        time_t resolution_date = (time_t)(-1);  /** < Timestamp when issue was resolved */
        std::vector<std::string> subtasks_ids;  /** < List of issue's subtasks */
        std::vector<Comment> comments;          /** < List of issue's comments */
        /**
         * @brief Construct a new Jira Issue object
         */
        JiraIssue() = default;                  
        /**
         * @brief Destroy the Jira Issue object
         */
        ~JiraIssue();
        /**
         * @brief Creates a JiraIssue object from JSON representation
         * 
         * Provided json string will be parsed and all fields for a new object
         * will be taken from the json.
         * 
         * @param [in] json_string JSON representation of the issue
         * @return JiraIssue* new object of JiraIssue with fields values from json
         */
        static JiraIssue* fromJSON(const std::string json_string);
};


/**
 * @brief Jira sprint partial representation
 * 
 * @todo rework source when everything will br wrapped with uniq_ptr
 */
class JiraSprint {
    public:
        int id;                             /** < ID of the sprint if Jira Database */
        int board_id;                       /** < ID of the board which includes the sprint */
        std::string name;                   /** < Title of the sprint */
        time_t start_date = {};             /** < Timestamp for start date */
        time_t end_date = {};               /** < Timestamp for end date */
        time_t complete_date = {};          /** < Timestamp for complete date */
        bool is_closed = false;             /** < The sprint was closed ? tru/false */
        std::vector <JiraIssue*> issues;    /** < List of all issues included into the sprint */

        /**
         * @brief Construct a new Jira Sprint object
         */
        JiraSprint() = default;
        /**
         * @brief Destroy the Jira Sprint object
         */
        ~JiraSprint();
        /**
         * @brief Creates a JiraSprint object from JSON representation
         * 
         * Provided json string will be parsed and all fields for a new object
         * will be taken from the json.
         * 
         * @param [in] json_string JSON representation of the sprint
         * @return JiraSprint* new object of JiraSprint with fields values from json
         */
        static JiraSprint* fromJSON(const std::string json_string);
};


/**
 * @brief Results for the sprint for a concrete person
 */
class PersonalResult {
    public:
        std::string user_id;        /** < ID of the user that has these results */
        std::string sprint_id;      /** < ID of the sprint that has all issues */
        std::vector<JiraIssue*> finished;           /** < List of issues with different types completed during the sprint */
        std::vector<JiraIssue*> not_finished;       /** < List of issue with different types were not completed during the sprint */
        std::vector<Comment> comments_written;      /** < Comment written by the user in all issues inside the sprint during the sprint */
        int issues_reviwed;                         /** < Number of issues assigned to somebidy else but commented by the user during the sprint */
        
        /**
         * @brief Construct a new Personal Result object
         */
        PersonalResult() = default;
        /**
         * @brief Destroy the Personal Result object
         */
        ~PersonalResult();
};

#endif // TYPES_H_