# Jira Sprint results lib

This is a library for analyzing a personal results during a Jira Sprint

## The requirements are

* GCC 5.1 or higher
* CMake 3.11 or better; 3.14+ highly recommended.
* Curl
* A C++11 compatible compiler
* Git
* Doxygen (optional)

## Build

1. Make a folder for build artifacts

```bash
cd Project
mkdir build
```

2. Configure a project:

```bash
cmake -S . -B build
```

OR for build without unit-tests 

```bash
cmake -S . -B build -DBUILD_TESTING=OFF
```

3. Build a project:

```bash
cmake --build build
```

3a. To build docs (requires Doxygen, output in `build/docs/html`):

```bash
cmake --build build --target docs
```

## Run application

#### 1. Update params.json file inside build/apps with your data

```json
{
    "jira_url": "<your jira web site URL>",
    "username": "<your jira's user name>",
    "token": "<your api token for access to jira instance>",
    "board_with_sprints": "<your project board's name>",
    ...
}
```

#### 2. Define how to organize search for results inside "period" object:

``` json
"jira_url": "<your jira web site URL>",
"username": "<your jira's user name>",
"token": "<your api token for access to jira instance>",
"board_with_sprints": "<your project board's name>",
"period" : {
 ...
}
```

#### 2a. Select type of search - by provided array of Sprint's names

```json
    "period" : {
        "type": "names",
        "sprint_names": [
            "SW Sprint 16",
            "SW Sprint 17"
        ]
    },
```
#### 2b. Select type of search - by provided start and end dates

```json
    "period" : {
        "type": "dates",
        "start_date" : "2020-05-27",
        "end_date": "2020-06-10"
    },
```

#### 3. Add a list of people's names (can be "name surname" or just "surname" if you are sure that your surname is uniq) for whom results will be counted

```json
    {
    "jira_url": " <-- put your values here -->",
    "username": "<-- put your values here -->",
    "token": "<-- put your values here -->",
    "board_with_sprints": "MPA1 board",
    "period" : {
        ...
    },
    "names": [
        "Strukov",
        "nikita",
        "tyutyarev"
    ]
}
```

#### 4. Run the counter app

Important! Please be sure that you are running application inside Project/build/apps folder. The counter app will look for the params.json file near the executable.

```bash
cd Project/build/apps
./counter
```

PS. If you need to change params.json file, change a file inside Project/build/apps directory. Counter app will look into this file, not in file that is in Project/apps.

PSS. Build process wil rewrite params.json file inside Project/build/apps with content from Project/apps/params.json. Remember it when you will try to change something and rebuild a solution.
