#include "VoteMachine.h"
#include <string.h>
#include <ctype.h>


int InitVoteMachine(struct VoteMachine* voteMachine)
{
    // must open files first with append mode in case it does not exist because
    // opening with r+ requires that the file exists
    voteMachine->votersFile = fopen(VOTER_FILE_PATH, "a");
    if(voteMachine->votersFile == NULL)
    {
        fprintf(stderr, "error opening file %s", VOTER_FILE_PATH);
        return APPLICATION_ERROR;
    }

    voteMachine->canditatesFile = fopen(CANDITATE_FILE_PATH, "a");
    if(voteMachine->canditatesFile == NULL)
    {
        fprintf(stderr, "error opening file %s", CANDITATE_FILE_PATH);
        return APPLICATION_ERROR;
    }
    //

    // reopening files in r+ mode so that we can update voter and canditate data when a vote is registered
    voteMachine->votersFile = freopen(VOTER_FILE_PATH, "r+", voteMachine->votersFile);
    if(voteMachine->votersFile == NULL)
    {
        fprintf(stderr, "error opening file %s", VOTER_FILE_PATH);
        return APPLICATION_ERROR;
    }

    voteMachine->canditatesFile = freopen(CANDITATE_FILE_PATH, "r+", voteMachine->canditatesFile);
    if(voteMachine->canditatesFile == NULL)
    {
        fprintf(stderr, "error opening file %s", CANDITATE_FILE_PATH);
        return APPLICATION_ERROR;
    }
    //

    return NO_ERROR;
}

int RegisterPerson(struct VoteMachine* voteMachine)
{
    struct Voter voter;
    memset(voter.firstName, '\0', FIRST_NAME_LEN + 1);
    memset(voter.lastName, '\0', LAST_NAME_LEN + 1);
    memset(voter.socialSecurityNum, '\0', SSN_LEN + 1);
    memset(voter.pinCode, '\0', PIN_LEN + 1);
    memset(voter.areaCode, '\0', AREA_CODE_LEN + 1);
    voter.hasVoted = FALSE;

    printf("enter your first name(example John): ");
    while(GetName(voter.firstName, FIRST_NAME_LEN) == APPLICATION_ERROR)
    {
        memset(voter.firstName, '\0', FIRST_NAME_LEN + 1);
        printf("name contains illegal characters. only A-z are allowed. please try again: ");
    }

    printf("enter your last name(example Doe): ");
    while(GetName(voter.lastName, LAST_NAME_LEN) == APPLICATION_ERROR)
    {
        memset(voter.lastName, '\0', LAST_NAME_LEN + 1);
        printf("name contains illegal characters. only A-z are allowed. please try again: ");
    }

    printf("enter your social security number(example 010224-111A): ");
    while(GetSSN(voter.socialSecurityNum, SSN_LEN) == APPLICATION_ERROR)
    {
        memset(voter.socialSecurityNum, '\0', SSN_LEN + 1);
        printf("invalid social security number, please try again: ");
    }

    // Check if voter with this SSN already is registered
    unsigned int nResults = 0;
    struct Voter* voterResults = SearchVoterBy(voteMachine->votersFile, &nResults, &voter, VOTER_SSN);
    if(nResults >= 1)
    {
        fprintf(stdout, "error! voter with this SSN is already registered\n");
        if(voterResults != NULL)
            free(voterResults);

        return NO_ERROR;
    }

    if(voterResults != NULL)
        free(voterResults);


    printf("enter your area code(example 01): ");
    while(GetAreaCode(voter.areaCode, AREA_CODE_LEN) == APPLICATION_ERROR)
    {
        memset(voter.areaCode, '\0', AREA_CODE_LEN + 1);
        printf("invalid area code, please try again: ");
    }


    printf("enter a 4 digit PIN number(example 1111): ");
    while(GetPIN(voter.pinCode, PIN_LEN) == APPLICATION_ERROR)
    {
        memset(voter.pinCode, '\0', PIN_LEN + 1);
        printf("invalid pin number, please try again: ");
    }

    if(SaveVoterToFile(voteMachine->votersFile, &voter) == APPLICATION_ERROR)
    {
        fprintf(stderr, "error writing voter to file\n");
        return APPLICATION_ERROR;
    }

    // reopening the file will update the file contents on disk. without this it would be updated when the file is closed
    if(freopen(VOTER_FILE_PATH, "r+", voteMachine->votersFile) == NULL)
    {
        fprintf(stderr, "error updating file %s", VOTER_FILE_PATH);
        return APPLICATION_ERROR;
    }

    CLEAR_CONSOLE;
    PrintVoter(&voter);
    printf("voter added succesfully, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int RegisterCanditate(struct VoteMachine* voteMachine)
{
    struct Canditate canditate;
    memset(canditate.firstName, '\0', FIRST_NAME_LEN + 1);
    memset(canditate.lastName, '\0', LAST_NAME_LEN + 1);
    canditate.nVotes = 0;
    canditate.number = 0;

    printf("enter your first name(example John): ");
    while(GetName(canditate.firstName, FIRST_NAME_LEN) == APPLICATION_ERROR)
    {
        memset(canditate.firstName, '\0', FIRST_NAME_LEN + 1);
        printf("name contains illegal characters. only A-z are allowed. please try again: ");
    }

    printf("enter your last name(example Doe): ");
    while(GetName(canditate.lastName, LAST_NAME_LEN) == APPLICATION_ERROR)
    {
        memset(canditate.lastName, '\0', LAST_NAME_LEN + 1);
        printf("name contains illegal characters. only A-z are allowed. please try again: ");
    }

    printf("enter canditate number: ");
    while(GetCanditateNum(&canditate.number) == APPLICATION_ERROR)
    {
        printf("invalid canditate number. please try again: ");
    }

    // check if canditate with this number already exists
    unsigned int nResults = 0;
    struct Canditate* canditateResults = SearchCanditateBy(voteMachine->canditatesFile, &nResults, &canditate, CANDITATE_NUMBER);
    if(nResults >= 1)
    {
        fprintf(stdout, "error! canditate with this number exists!");
        if(canditateResults != NULL)
            free(canditateResults);

        return NO_ERROR;
    }

    if(canditateResults != NULL)
        free(canditateResults);

    if(SaveCanditateToFile(voteMachine->canditatesFile, &canditate) == APPLICATION_ERROR)
    {
        fprintf(stderr, "error writing canditate to file\n");
        return APPLICATION_ERROR;
    }

    // reopening the file will update the file contents on disk. without this it would be updated when the file is closed
    if(freopen(CANDITATE_FILE_PATH, "r+", voteMachine->canditatesFile) == NULL)
    {
        fprintf(stderr, "error updating file %s", CANDITATE_FILE_PATH);
        return APPLICATION_ERROR;
    }

    CLEAR_CONSOLE;
    PrintCanditate(&canditate);
    printf("canditate added succesfully, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int SearchVoters(struct VoteMachine* voteMachine)
{
    CLEAR_CONSOLE;

    printf("[VOTE MACHINE]\n\n");
    printf("1. search by name\n");
    printf("2. search by social security number\n");
    printf("3. search by pin code\n");
    printf("4. search by area code\n");
    printf("5. search by vote status\n");
    printf("6. return to main menu\n");


    char input = getc(stdin);
    fflush(stdin);
    CLEAR_CONSOLE;
    switch(input)
    {
        case '1':
        {
            CLEAR_CONSOLE;
            if(SearchVoterByName(voteMachine) == APPLICATION_ERROR)
                return APPLICATION_ERROR;
        } break;
        case '2':
        {
            CLEAR_CONSOLE;
            if(SearchVoterBySSN(voteMachine) == APPLICATION_ERROR)
                return APPLICATION_ERROR;
        } break;
        case '3':
        {
            CLEAR_CONSOLE;
            if(SearchVoterByPIN(voteMachine) == APPLICATION_ERROR)
                return APPLICATION_ERROR;
        } break;
        case '4':
        {
            CLEAR_CONSOLE;
            if(SearchVoterByArea(voteMachine) == APPLICATION_ERROR)
                return APPLICATION_ERROR;
        } break;
        case '5':
        {
            CLEAR_CONSOLE;
            if(SearchVoterByHasVoted(voteMachine) == APPLICATION_ERROR)
                return APPLICATION_ERROR;
        } break;
        case '6':
        {
            return NO_ERROR;
        } break;
        default:
        {
            fprintf(stderr, "this should not have happened...quitting\n");
            return APPLICATION_ERROR;
        } break;
    }

    return NO_ERROR;
}

int SearchCanditates(struct VoteMachine* voteMachine)
{
    CLEAR_CONSOLE;

    printf("[VOTE MACHINE]\n\n");
    printf("1. search by name\n");
    printf("2. search by number\n");
    printf("3. return to main menu\n");


    char input = getc(stdin);
    fflush(stdin);
    CLEAR_CONSOLE;
    switch(input)
    {
        case '1':
        {
            CLEAR_CONSOLE;
            if(SearchCanditateByName(voteMachine) == APPLICATION_ERROR)
                return APPLICATION_ERROR;

        } break;
        case '2':
        {
            CLEAR_CONSOLE;
            if(SearchCanditateByNumber(voteMachine) == APPLICATION_ERROR)
                return APPLICATION_ERROR;
        } break;
        case '3':
        {
            return NO_ERROR;
        } break;
        default:
        {
            fprintf(stderr, "this should not have happened...quitting\n");
            return APPLICATION_ERROR;
        } break;
    }

    return NO_ERROR;
}

int RegisterVote(struct VoteMachine* voteMachine)
{
    // 1. Check that a voter exists with given PIN and SSN and that they have not voted
    struct Voter voter;
    printf("enter your PIN number: ");
    while(GetPIN(voter.pinCode, PIN_LEN) == APPLICATION_ERROR)
    {
        memset(voter.pinCode, '\0', PIN_LEN + 1);
        printf("invalid pin number, please try again: ");
    }

    printf("enter your social security number: ");
    while(GetSSN(voter.socialSecurityNum, SSN_LEN) == APPLICATION_ERROR)
    {
        memset(voter.socialSecurityNum, '\0', SSN_LEN + 1);
        printf("invalid social security number, please try again: ");
    }

    voter.hasVoted = FALSE;

    unsigned int nResults = 0;
    struct Voter* voterResults = SearchVoterBy(voteMachine->votersFile, &nResults, &voter, VOTER_PIN | VOTER_HAS_VOTED | VOTER_SSN);
    if(nResults != 1)
    {
        if(voterResults != NULL)
            free(voterResults);

        CLEAR_CONSOLE;
        fprintf(stdout, "error! voter does not exist or voter has already voted! enter any key to continue: ");
        getc(stdin);
        fflush(stdin);

        return NO_ERROR;
    }

    if(voterResults != NULL)
        free(voterResults);


    // 2. Check that canditate exists with given canditate number
    struct Canditate canditate;
    printf("enter canditate number: ");
    while(GetCanditateNum(&canditate.number) == APPLICATION_ERROR)
    {
        printf("invalid canditate number. please try again: ");
    }

    nResults = 0;
    struct Canditate* canditateResults = SearchCanditateBy(voteMachine->canditatesFile, &nResults, &canditate, CANDITATE_NUMBER);
    if(nResults != 1)
    {
        if(canditateResults != NULL)
            free(canditateResults);

        CLEAR_CONSOLE;
        fprintf(stdout, "error! canditate with this number does not exist! enter any key to continue: ");
        getc(stdin);
        fflush(stdin);

        return NO_ERROR;
    }

    // 3. update voters file
    if(UpdateVoterHasVotedStatusInFile(voteMachine, voter.socialSecurityNum, TRUE) == APPLICATION_ERROR)
    {
        fprintf(stderr, "UpdateVoterHasVotedStatusInFile() error\n");
        return APPLICATION_ERROR;
    }

    // 4. update canditates file
    if(UpdateCanditateNumVotesInFile(voteMachine, canditate.number, canditateResults[0].nVotes + 1) == APPLICATION_ERROR)
    {
        fprintf(stderr, "UpdateCanditateNumVotesInFile() error\n");
        return APPLICATION_ERROR;
    }

    if(canditateResults != NULL)
        free(canditateResults);

    CLEAR_CONSOLE;
    printf("vote registered succesfully, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);
}

int SearchVoterByName(struct VoteMachine* voteMachine)
{
    struct Voter voter;

    printf("enter first name: ");
    while(GetName(voter.firstName, FIRST_NAME_LEN) == APPLICATION_ERROR)
    {
        memset(voter.firstName, '\0', FIRST_NAME_LEN + 1);
        printf("name contains illegal characters. only A-z are allowed. please try again: ");
    }

    printf("enter last name: ");
    while(GetName(voter.lastName, LAST_NAME_LEN) == APPLICATION_ERROR)
    {
        memset(voter.lastName, '\0', LAST_NAME_LEN + 1);
        printf("name contains illegal characters. only A-z are allowed. please try again: ");
    }

    unsigned int nResults = 0;
    struct Voter* results = SearchVoterBy(voteMachine->votersFile, &nResults, &voter, VOTER_FIRST_NAME | VOTER_LAST_NAME);

    struct Voter* resultsPtr = results;
    fprintf(stdout, "[search results]:\n\n");
    for(unsigned int i = 0; i < nResults; ++i)
        PrintVoter(resultsPtr++);

    if(results != NULL)
        free(results);

    fprintf(stdout, "search complete, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int SearchVoterBySSN(struct VoteMachine* voteMachine)
{
    struct Voter voter;
    printf("enter social security number: ");
    while(GetSSN(voter.socialSecurityNum, SSN_LEN) == APPLICATION_ERROR)
    {
        memset(voter.socialSecurityNum, '\0', SSN_LEN + 1);
        printf("invalid social security number, please try again: ");
    }

    unsigned int nResults = 0;
    struct Voter* results = SearchVoterBy(voteMachine->votersFile, &nResults, &voter, VOTER_SSN);

    struct Voter* resultsPtr = results;
    fprintf(stdout, "[search results]:\n\n");
    for(unsigned int i = 0; i < nResults; ++i)
        PrintVoter(resultsPtr++);

    if(results != NULL)
        free(results);

    fprintf(stdout, "search complete, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int SearchVoterByPIN(struct VoteMachine* voteMachine)
{
    struct Voter voter;
    printf("enter a 4 digit PIN number: ");
    while(GetPIN(voter.pinCode, PIN_LEN) == APPLICATION_ERROR)
    {
        memset(voter.pinCode, '\0', PIN_LEN + 1);
        printf("invalid pin number, please try again: ");
    }

    unsigned int nResults = 0;
    struct Voter* results = SearchVoterBy(voteMachine->votersFile, &nResults, &voter, VOTER_PIN);

    struct Voter* resultsPtr = results;
    fprintf(stdout, "[search results]:\n\n");
    for(unsigned int i = 0; i < nResults; ++i)
        PrintVoter(resultsPtr++);

    if(results != NULL)
        free(results);

    fprintf(stdout, "search complete, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int SearchVoterByArea(struct VoteMachine* voteMachine)
{
    struct Voter voter;
    printf("enter area code): ");
    while(GetAreaCode(voter.areaCode, AREA_CODE_LEN) == APPLICATION_ERROR)
    {
        memset(voter.areaCode, '\0', AREA_CODE_LEN + 1);
        printf("invalid area code, please try again: ");
    }

    unsigned int nResults = 0;
    struct Voter* results = SearchVoterBy(voteMachine->votersFile, &nResults, &voter, VOTER_AREA);

    struct Voter* resultsPtr = results;
    fprintf(stdout, "[search results]:\n\n");
    for(unsigned int i = 0; i < nResults; ++i)
        PrintVoter(resultsPtr++);

    if(results != NULL)
        free(results);

    fprintf(stdout, "search complete, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int SearchVoterByHasVoted(struct VoteMachine* voteMachine)
{
    struct Voter voter;
    printf("enter 1 to view voters that have voted, or 0 to view voters that have not voted): ");
    char hasVoted = fgetc(stdin);
    fflush(stdin);

    if(hasVoted == '1')
        voter.hasVoted = TRUE;
    else if(hasVoted == '0')
        voter.hasVoted = FALSE;
    else
    {
        fprintf(stdout, "invalid input, enter any character to continue: ");
        getc(stdin);
        fflush(stdin);
        return NO_ERROR;
    }

    unsigned int nResults = 0;
    struct Voter* results = SearchVoterBy(voteMachine->votersFile, &nResults, &voter, VOTER_HAS_VOTED);

    struct Voter* resultsPtr = results;
    fprintf(stdout, "[search results]:\n\n");
    for(unsigned int i = 0; i < nResults; ++i)
        PrintVoter(resultsPtr++);

    if(results != NULL)
        free(results);

    fprintf(stdout, "search complete, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int SearchCanditateByName(struct VoteMachine* voteMachine)
{
    struct Canditate canditate;
    printf("enter first name: ");
    while(GetName(canditate.firstName, FIRST_NAME_LEN) == APPLICATION_ERROR)
    {
        memset(canditate.firstName, '\0', FIRST_NAME_LEN + 1);
        printf("name contains illegal characters. only A-z are allowed. please try again: ");
    }

    printf("enter last name: ");
    while(GetName(canditate.lastName, LAST_NAME_LEN) == APPLICATION_ERROR)
    {
        memset(canditate.lastName, '\0', LAST_NAME_LEN + 1);
        printf("name contains illegal characters. only A-z are allowed. please try again: ");
    }

    unsigned int nResults = 0;
    struct Canditate* results = SearchCanditateBy(voteMachine->canditatesFile, &nResults, &canditate, CANDITATE_FIRST_NAME | CANDITATE_LAST_NAME);

    struct Canditate* resultsPtr = results;
    fprintf(stdout, "[search results]:\n\n");
    for(unsigned int i = 0; i < nResults; ++i)
        PrintCanditate(resultsPtr++);

    if(results != NULL)
        free(results);

    fprintf(stdout, "search complete, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int SearchCanditateByNumber(struct VoteMachine* voteMachine)
{
    struct Canditate canditate;
    printf("enter canditate number: ");
    while(GetCanditateNum(&canditate.number) == APPLICATION_ERROR)
    {
        printf("invalid canditate number. please try again: ");
    }

    unsigned int nResults = 0;
    struct Canditate* results = SearchCanditateBy(voteMachine->canditatesFile, &nResults, &canditate, CANDITATE_NUMBER);

    struct Canditate* resultsPtr = results;
    fprintf(stdout, "[search results]:\n\n");
    for(unsigned int i = 0; i < nResults; ++i)
        PrintCanditate(resultsPtr++);

    if(results != NULL)
        free(results);

    fprintf(stdout, "search complete, enter any character to continue: ");
    getc(stdin);
    fflush(stdin);

    return NO_ERROR;
}

int GetName(char* name, unsigned int nameSize)
{
    fgets(name, nameSize, stdin);
    fflush(stdin);

    bool32 isValid = TRUE;
    while(*name++ != '\0')
    {
        if(*name == '\n')
        {
            *name = '\0';
            break;
        }
        if(isalpha(*name) == 0)
        {
            isValid = FALSE;
            break;
        }
    }

    return isValid == TRUE ? NO_ERROR : APPLICATION_ERROR;
}

int GetSSN(char* ssn, unsigned int snnSize)
{
    fgets(ssn, snnSize + 1, stdin);
    fflush(stdin);

    bool32 isValid = TRUE;
    for(unsigned int i = 0; i < snnSize; ++i)
    {
        if(ssn[i] == '\n')
        {
            ssn[i] = '\0';
            break;
        }

        if((i < 6) || (i > 6 && i < 10))
        {
            if(isdigit(ssn[i]) == 0)
            {
                isValid = FALSE;
                break;
            }
        }
        else if(i == 6)
        {
            if(ssn[i] != '-' && ssn[i] != '+' && ssn[i] != 'A')
            {
                isValid = FALSE;
                break;
            }
        }
        else if(i == 10)
        {
            if(ssn[i] == 'G' || ssn[i] == 'I' || ssn[i] == 'O' || ssn[i] == 'Q' || ssn[i] == 'Z')
            {
                isValid = FALSE;
                break;
            }
            if((isdigit(ssn[i]) == 0) && (ssn[i] < 65 || ssn[i] > 90))
            {
                isValid = FALSE;
                break;
            }
        }
    }

    if(strlen(ssn) != SSN_LEN)
        isValid = FALSE;

    return isValid == TRUE ? NO_ERROR : APPLICATION_ERROR;
}

int GetPIN(char* pin, unsigned int pinSize)
{
    fgets(pin, pinSize + 1, stdin);
    fflush(stdin);

    bool32 isValid = TRUE;
    for(unsigned int i = 0; i < pinSize; ++i)
    {
        if(pin[i] == '\n')
        {
            pin[i] = '\0';
            break;
        }

        if(isdigit(pin[i]) == 0)
        {
            isValid = FALSE;
            break;
        }
    }

    if(strlen(pin) != PIN_LEN)
        isValid = FALSE;

    return isValid == TRUE ? NO_ERROR : APPLICATION_ERROR;
}

int GetAreaCode(char* areaCode, unsigned int areaCodeSize)
{
    fgets(areaCode, areaCodeSize + 1, stdin);
    fflush(stdin);

    if(areaCode[0] == '0')
    {
        if(areaCode[1] < 49 || areaCode[1] > 57)
            return APPLICATION_ERROR;
    }
    else if(areaCode[0] == '1')
    {
        if(areaCode[1] < 48 || areaCode[1] > 51)
            return APPLICATION_ERROR;
    }
    else
    {
        return APPLICATION_ERROR;
    }

    return NO_ERROR;
}

int GetCanditateNum(unsigned int* canditateNum)
{
    char buffer[8];
    fgets(buffer, sizeof(buffer) + 1, stdin);
    fflush(stdin);

    *canditateNum = atoi(buffer);
    if(*canditateNum > 1)
        return NO_ERROR;

    return APPLICATION_ERROR;
}

int UpdateVoterHasVotedStatusInFile(struct VoteMachine* voteMachine, const char* voterSSN, bool32 newVal)
{
    FILE* updatedVotersFile = fopen("voters_updated.txt", "w");
    if(updatedVotersFile == NULL)
    {
        fprintf(stdout, "error opening updated voters file\n");
        return APPLICATION_ERROR;
    }

    rewind(voteMachine->votersFile);
    char voterStr[MAX_VOTER_STR_SIZE];
    memset(voterStr, '\0', MAX_VOTER_STR_SIZE);
    while(fgets(voterStr, sizeof(voterStr), voteMachine->votersFile) != NULL)
    {
        struct Voter voterInFile;

        if(StrToVoter(&voterInFile, voterStr) == APPLICATION_ERROR)
        {
            fprintf(stderr, "StrToVoter() error\n");
            fclose(updatedVotersFile);
            return APPLICATION_ERROR;
        }

        if(strcmp(voterSSN, voterInFile.socialSecurityNum) == 0)
            voterInFile.hasVoted = newVal;

        if(SaveVoterToFile(updatedVotersFile, &voterInFile) == APPLICATION_ERROR)
        {
            fprintf(stderr, "error saving voter to file\n");
            fclose(updatedVotersFile);
            return APPLICATION_ERROR;
        }
    }

    // close so that we can remove/rename
    fclose(voteMachine->votersFile);
    fclose(updatedVotersFile);

    if(remove(VOTER_FILE_PATH) != 0)
    {
        fprintf(stderr, "remove() error!\n");
        return APPLICATION_ERROR;
    }

    if(rename("voters_updated.txt", VOTER_FILE_PATH) != 0)
    {
        fprintf(stderr, "rename() error!\n");
        return APPLICATION_ERROR;
    }

    voteMachine->votersFile = fopen(VOTER_FILE_PATH, "r+");
    if(voteMachine->votersFile == NULL)
    {
        fprintf(stderr, "error opening file %s", VOTER_FILE_PATH);
        return APPLICATION_ERROR;
    }

    return NO_ERROR;
}

int UpdateCanditateNumVotesInFile(struct VoteMachine* voteMachine, unsigned int canditateNumber, unsigned int newVal)
{
    FILE* updatedCanditatesFile = fopen("canditates_updated.txt", "w");
    if(updatedCanditatesFile == NULL)
    {
        fprintf(stdout, "error opening updated canditates file\n");
        return APPLICATION_ERROR;
    }

    rewind(voteMachine->canditatesFile);
    char canditateStr[MAX_CANDITATE_STR_SIZE];
    memset(canditateStr, '\0', MAX_CANDITATE_STR_SIZE);
    while(fgets(canditateStr, sizeof(canditateStr), voteMachine->canditatesFile) != NULL)
    {
        struct Canditate canditateInFile;

        if(StrToCanditate(&canditateInFile, canditateStr) == APPLICATION_ERROR)
        {
            fprintf(stderr, "StrToCanditate() error\n");
            fclose(updatedCanditatesFile);
            return APPLICATION_ERROR;
        }

        if(canditateNumber == canditateInFile.number)
            canditateInFile.nVotes = newVal;

        if(SaveCanditateToFile(updatedCanditatesFile, &canditateInFile) == APPLICATION_ERROR)
        {
            fprintf(stderr, "error saving canditate to file\n");
            fclose(updatedCanditatesFile);
            return APPLICATION_ERROR;
        }
    }

    // close so that we can remove/rename
    fclose(voteMachine->canditatesFile);
    fclose(updatedCanditatesFile);

    if(remove(CANDITATE_FILE_PATH) != 0)
    {
        fprintf(stderr, "remove() error!\n");
        return APPLICATION_ERROR;
    }

    if(rename("canditates_updated.txt", CANDITATE_FILE_PATH) != 0)
    {
        fprintf(stderr, "rename() error!\n");
        return APPLICATION_ERROR;
    }

    voteMachine->canditatesFile = fopen(CANDITATE_FILE_PATH, "r+");
    if(voteMachine->canditatesFile == NULL)
    {
        fprintf(stderr, "error opening file %s", CANDITATE_FILE_PATH);
        return APPLICATION_ERROR;
    }

    return NO_ERROR;
}

struct Voter* SearchVoterBy(FILE* votersFile, unsigned int* nResults, const struct Voter* voterMask, const unsigned int dataToSearch)
{
    rewind(votersFile);

    struct Voter voter;
    char voterStr[MAX_VOTER_STR_SIZE];
    memset(voterStr, '\0', sizeof(voterStr));

    struct Voter* results = NULL;
    *nResults = 0;

    while(fgets(voterStr, sizeof(voterStr), votersFile) != NULL)
    {
        if(StrToVoter(&voter, voterStr) == APPLICATION_ERROR)
            return NULL;

        if((dataToSearch & VOTER_FIRST_NAME) == VOTER_FIRST_NAME)
        {
            if(strcmp(voter.firstName, voterMask->firstName) != 0)
                continue;
        }

        if((dataToSearch & VOTER_LAST_NAME) == VOTER_LAST_NAME)
        {
            if(strcmp(voter.lastName, voterMask->lastName) != 0)
                continue;
        }

        if((dataToSearch & VOTER_SSN) == VOTER_SSN)
        {
            if(strcmp(voter.socialSecurityNum, voterMask->socialSecurityNum) != 0)
                continue;
        }

        if((dataToSearch & VOTER_PIN) == VOTER_PIN)
        {
            if(strcmp(voter.pinCode, voterMask->pinCode) != 0)
                continue;
        }

        if((dataToSearch & VOTER_AREA) == VOTER_AREA)
        {
            if(strcmp(voter.areaCode, voterMask->areaCode) != 0)
                continue;
        }

        if((dataToSearch & VOTER_HAS_VOTED) == VOTER_HAS_VOTED)
        {
            if(voter.hasVoted != voterMask->hasVoted)
                continue;
        }

        struct Voter* newResults = (struct Voter*)malloc(sizeof(struct Voter) * (*nResults + 1));
        memcpy(newResults, results, sizeof(struct Voter) * (*nResults));
        memcpy(newResults + *nResults, &voter, sizeof(struct Voter));
        if(results)
            free(results);
        results = newResults;

        *nResults += 1;
    }

    return results;
}

struct Canditate* SearchCanditateBy(FILE* canditatesFile, unsigned int* nResults, const struct Canditate* canditateMask, const unsigned int dataToSearch)
{
    rewind(canditatesFile);

    struct Canditate canditate;
    char canditateStr[MAX_CANDITATE_STR_SIZE];
    memset(canditateStr, '\0', sizeof(canditateStr));

    struct Canditate* results = NULL;

    while(fgets(canditateStr, sizeof(canditateStr), canditatesFile) != NULL)
    {
        if(StrToCanditate(&canditate, canditateStr) == APPLICATION_ERROR)
            return NULL;

        if((dataToSearch & CANDITATE_FIRST_NAME) == CANDITATE_FIRST_NAME)
        {
            if(strcmp(canditate.firstName, canditateMask->firstName) != 0)
                continue;
        }

        if((dataToSearch & CANDITATE_LAST_NAME) == CANDITATE_LAST_NAME)
        {
            if(strcmp(canditate.lastName, canditateMask->lastName) != 0)
                continue;
        }

        if((dataToSearch & CANDITATE_NUMBER) == CANDITATE_NUMBER)
        {
            if(canditate.number != canditateMask->number)
                continue;
        }

        struct Canditate* newResults = (struct Canditate*)malloc(sizeof(struct Canditate) * (*nResults + 1));
        memcpy(newResults, results, sizeof(struct Canditate) * (*nResults));
        memcpy(newResults + *nResults, &canditate, sizeof(struct Canditate));
        if(results)
            free(results);
        results = newResults;

        *nResults += 1;
    }

    return results;
}

int StrToVoter(struct Voter* voter, const char* voterStr)
{
    int res = sscanf(voterStr, "%s %s %s %s %s %u\n", voter->firstName, voter->lastName, voter->socialSecurityNum, voter->pinCode, voter->areaCode, &voter->hasVoted);

    if(res != 6)
    {
        fprintf(stderr, "sscanf error\n");
        return APPLICATION_ERROR;
    }

    return NO_ERROR;
}

int StrToCanditate(struct Canditate* canditate, const char* canditateStr)
{
    int res = sscanf(canditateStr, "%s %s %u %u\n", canditate->firstName, canditate->lastName, &canditate->number, &canditate->nVotes);
    if(res != 4)
    {
        fprintf(stderr, "sscanf error");
        return APPLICATION_ERROR;
    }

    return NO_ERROR;
}

int VoterToStr(char* voterStr, const unsigned int voterStrSize, const struct Voter* voter)
{
    int res = snprintf(voterStr, voterStrSize, "%s %s %s %s %s %u", voter->firstName, voter->lastName,
        voter->socialSecurityNum, voter->pinCode, voter->areaCode, voter->hasVoted);

    if(res > 0 && res < voterStrSize)
        return NO_ERROR;
    else
        return APPLICATION_ERROR;
}

int CanditateToStr(char* canditateStr, const unsigned int canditateStrSize, const struct Canditate* canditate)
{
    int res = snprintf(canditateStr, canditateStrSize, "%s %s %u %u", canditate->firstName,
        canditate->lastName, canditate->number, canditate->nVotes);

    if(res > 0 && res < canditateStrSize)
        return NO_ERROR;
    else
        return APPLICATION_ERROR;
}

void FreeVoteMachine(struct VoteMachine* voteMachine)
{
    if(voteMachine->votersFile != NULL)
        fclose(voteMachine->votersFile);

    if(voteMachine->canditatesFile != NULL)
        fclose(voteMachine->canditatesFile);
}

int SaveVoterToFile(FILE* voterFile, const struct Voter* voter)
{
    if(fseek(voterFile, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "fseek error");
        return APPLICATION_ERROR;
    }

    int res = fprintf(voterFile, "%s %s %s %s %s %u\n",
        voter->firstName, voter->lastName, voter->socialSecurityNum, voter->pinCode, voter->areaCode,
        voter->hasVoted);

    if(res < 0)
        return APPLICATION_ERROR;

    return NO_ERROR;
}

int SaveCanditateToFile(FILE* canditateFile, const struct Canditate* canditate)
{
    if(fseek(canditateFile, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "fseek error");
        return APPLICATION_ERROR;
    }

    int res = fprintf(canditateFile, "%s %s %u %u\n",
        canditate->firstName, canditate->lastName, canditate->number, canditate->nVotes);

    if(res < 0)
        return APPLICATION_ERROR;

    return NO_ERROR;
}

void PrintVoter(const struct Voter* voter)
{
    printf("first name: %s\nlast name: %s\nsocial security number: %s\npin code: %s\narea code: %s\nhas voted: %s\n----------\n",
           voter->firstName, voter->lastName, voter->socialSecurityNum, voter->pinCode, voter->areaCode,
           voter->hasVoted == TRUE ? "yes" : "no");
}

void PrintCanditate(const struct Canditate* canditate)
{
    printf("first name: %s\nlast name: %s\nnumber: %u\nnumber of votes: %u\n----------\n",
           canditate->firstName, canditate->lastName, canditate->number, canditate->nVotes);
}
