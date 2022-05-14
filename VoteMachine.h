#ifndef VOTEMACHINE_H
#define VOTEMACHINE_H

#include <stdlib.h>
#include <stdio.h>

#define SYSTEM_LINUX    0 // TODO: test program in linux
#define SYSTEM_WINDOWS  1

#ifdef SYSTEM_WINDOWS
#define CLEAR_CONSOLE ({system("cls");})
#elif
#define CLEAR_CONSOLE ({system("clear");})
#endif

#define TRUE            1
#define FALSE           0

#define APPLICATION_WARNING -2
#define APPLICATION_ERROR   -1
#define NO_ERROR            0

#define FIRST_NAME_LEN  24
#define LAST_NAME_LEN   48
#define SSN_LEN         11
#define PIN_LEN         4
#define AREA_CODE_LEN   2

#define VOTER_FILE_PATH     "voters.txt"
#define CANDITATE_FILE_PATH "canditates.txt"

typedef unsigned int bool32;

// used in SearchVoterBy(), can be OR:ed together to search by multiple requirements
#define VOTER_FIRST_NAME    1
#define VOTER_LAST_NAME     2
#define VOTER_SSN           4
#define VOTER_PIN           8
#define VOTER_AREA          16
#define VOTER_HAS_VOTED     32

// used in SearchCanditateBy(), can be OR:ed together to search by multiple requirements
#define CANDITATE_FIRST_NAME    1
#define CANDITATE_LAST_NAME     2
#define CANDITATE_NUMBER        4

#define MAX_VOTER_STR_SIZE      512
#define MAX_CANDITATE_STR_SIZE  512

struct Voter
{
    char firstName[FIRST_NAME_LEN + 1];
    char lastName[LAST_NAME_LEN + 1];
    char socialSecurityNum[SSN_LEN + 1];
    char pinCode[PIN_LEN +1];
    char areaCode[AREA_CODE_LEN + 1];
    bool32 hasVoted;
};

struct Canditate
{
    char firstName[FIRST_NAME_LEN + 1];
    char lastName[LAST_NAME_LEN + 1];
    unsigned int number;
    unsigned int nVotes;
};

struct VoteMachine
{
    FILE* votersFile;
    FILE* canditatesFile;
};

int InitVoteMachine(struct VoteMachine* voteMachine);

// main program interface functions
int RegisterPerson(struct VoteMachine* voteMachine);
int RegisterCanditate(struct VoteMachine* voteMachine);
int SearchVoters(struct VoteMachine* voteMachine);
int SearchCanditates(struct VoteMachine* voteMachine);
int RegisterVote(struct VoteMachine* voteMachine);
//

// internal helper functions
int SearchVoterByName(struct VoteMachine* voteMachine);
int SearchVoterBySSN(struct VoteMachine* voteMachine);
int SearchVoterByPIN(struct VoteMachine* voteMachine);
int SearchVoterByArea(struct VoteMachine* voteMachine);
int SearchVoterByHasVoted(struct VoteMachine* voteMachine);
int SearchCanditateByName(struct VoteMachine* voteMachine);
int SearchCanditateByNumber(struct VoteMachine* voteMachine);

int GetName(char* name, unsigned int nameSize);
int GetSSN(char* ssn, unsigned int snnSize);
int GetPIN(char* pin, unsigned int pinSize);
int GetAreaCode(char* areaCode, unsigned int areaCodeSize);
int GetCanditateNum(unsigned int* canditateNum);

int UpdateVoterHasVotedStatusInFile(struct VoteMachine* voteMachine, const char* voterSSN, bool32 newVal);
int UpdateCanditateNumVotesInFile(struct VoteMachine* voteMachine, unsigned int canditateNumber, unsigned int newVal);

/*
    returns a pointer to results, must be free()d
    dataToSearch can be any of these and can be OR:ed together to filter by multiple fields:
    VOTER_FIRST_NAME
    VOTER_LAST_NAME
    VOTER_SSN
    VOTER_PIN
    VOTER_AREA
    VOTER_HAS_VOTED

    for example if dataToSearch is VOTER_SSN | VOTER_HAS_VOTED, only voters
    who have the same SSN and hasVoted status as voterMask will be added to the results
*/
struct Voter* SearchVoterBy(FILE* votersFile, unsigned int* nResults, const struct Voter* voterMask, const unsigned int dataToSearch);

/*
    returns a pointer to results, must be free()d
    dataToSearch can be any of these and can be OR:ed together to filter by multiple fields:
    dataToSearch can be any of these and can be OR:ed together:
    CANDITATE_FIRST_NAME
    CANDITATE_LAST_NAME
    CANDITATE_NUMBER

    for example if dataToSearch is CANDIATE_NUMBER | CANDITATE_FIRST_NAME, only canditates
    who have the same number and first name as canditateMask will be added to the results
*/
struct Canditate* SearchCanditateBy(FILE* canditatesFile, unsigned int* nResults, const struct Canditate* canditateMask, const unsigned int dataToSearch);

int StrToVoter(struct Voter* voter, const char* voterStr);
int StrToCanditate(struct Canditate* canditate, const char* canditateStr);
int VoterToStr(char* voterStr, const unsigned int voterStrSize, const struct Voter* voter);
int CanditateToStr(char* canditateStr, const unsigned int canditateStrSize, const struct Canditate* canditate);

void FreeVoteMachine(struct VoteMachine* voteMachine);
int SaveVoterToFile(FILE* voterFile, const struct Voter* voter);
int SaveCanditateToFile(FILE* canditateFile, const struct Canditate* canditate);
//

// Other
void PrintVoter(const struct Voter* voter);
void PrintCanditate(const struct Canditate* canditate);
//

#endif // VOTEMACHINE_H
