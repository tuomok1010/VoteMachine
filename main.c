#include <stdio.h>

#include "VoteMachine.h"


int main()
{
    struct VoteMachine voteMachine;
    InitVoteMachine(&voteMachine);

    bool32 running = TRUE;

    CLEAR_CONSOLE;

    while(running == TRUE)
    {
        CLEAR_CONSOLE;

        printf("[VOTE MACHINE]\n\n");
        printf("1. register new voter\n");
        printf("2. register canditate\n");
        printf("3. search voters\n");
        printf("4. search canditates\n");
        printf("5. register vote\n");
        printf("6. exit\n");

        char input = getc(stdin);
        fflush(stdin);
        CLEAR_CONSOLE;
        switch(input)
        {
            case '1':
            {
                if(RegisterPerson(&voteMachine) == APPLICATION_ERROR)
                {
                    FreeVoteMachine(&voteMachine);
                    return APPLICATION_ERROR;
                }

            } break;
            case '2':
            {
                if(RegisterCanditate(&voteMachine) == APPLICATION_ERROR)
                {
                    FreeVoteMachine(&voteMachine);
                    return APPLICATION_ERROR;
                }
            } break;
            case '3':
            {
                if(SearchVoters(&voteMachine) == APPLICATION_ERROR)
                {
                    FreeVoteMachine(&voteMachine);
                    return APPLICATION_ERROR;
                }
            } break;
            case '4':
            {
                if(SearchCanditates(&voteMachine) == APPLICATION_ERROR)
                {
                    FreeVoteMachine(&voteMachine);
                    return APPLICATION_ERROR;
                }
            } break;
            case '5':
            {
                if(RegisterVote(&voteMachine) == APPLICATION_ERROR)
                {
                    FreeVoteMachine(&voteMachine);
                    return APPLICATION_ERROR;
                }
            } break;
            case '6':
            {
                fprintf(stdout, "quitting...\n");
                FreeVoteMachine(&voteMachine);
                running = FALSE;
            } break;
            default:
            {
                fprintf(stderr, "this should not have happened...quitting\n");
                FreeVoteMachine(&voteMachine);
                return APPLICATION_ERROR;
            } break;
        }
    }

    return 0;
}
