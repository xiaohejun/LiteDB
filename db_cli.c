#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void (* ExecuteFunc)();
#define ARR_SIZE(arr, type) (sizeof(arr) / sizeof(type))
void ExecuteInsertCommand();
void ExecuteSelectCommand();

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT 
} PrepareResult;

typedef enum {
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
} StatementType;

typedef struct {
    const char *name;
    StatementType type;
    ExecuteFunc executeFunc;
} Statement;

Statement g_statements[] = {
    {"insert", STATEMENT_INSERT, ExecuteInsertCommand},
    {"select", STATEMENT_SELECT, ExecuteSelectCommand}
};

typedef struct {
    char *buffer;
    size_t buffLen;
    ssize_t inputLen;
} InputBuffer;

InputBuffer* NewInputBuffer()
{
    InputBuffer *input = malloc(sizeof(sizeof(InputBuffer)));
    input->buffer = NULL;
    input->buffLen = 0;
    input->inputLen = 0;
    return input;
}

void DeleteInputBuffer(InputBuffer *input)
{
    free(input->buffer);
    free(input);
}

void PrintPrompt()
{
    printf("LiteDB > ");
}

MetaCommandResult DoMetaCommand(InputBuffer *input)
{
    if (strcmp(input->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

PrepareResult PrepareStatement(InputBuffer *input, Statement **statement)
{
    for (int i = 0; i < ARR_SIZE(g_statements, Statement); ++i) {
        Statement *st = &g_statements[i];
        if (strncmp(st->name, input->buffer, (size_t)strlen(st->name)) == 0) {
            *statement = st;
            return PREPARE_SUCCESS;
        }
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void ReadInput(InputBuffer *input)
{
    ssize_t readBytes = getline(&(input->buffer), &(input->buffLen), stdin);
    if (readBytes <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    
    // 忽略结尾的行'\n'字符
    input->inputLen = readBytes - 1;
    input->buffer[readBytes - 1] = '\0';
}

void ExecuteInsertCommand()
{
    printf("This is where we would do an insert.\n");
}

void ExecuteSelectCommand()
{
    printf("This is where we would do a select.\n");
}

void ExecuteStatement(Statement *statement)
{
    statement->executeFunc();
    printf("Executed.\n");
}

bool IsMetaCommand(InputBuffer *input)
{
    return input->buffer[0] == '.';
}

int main()
{
    InputBuffer *input = NewInputBuffer();
    while (true) {
        PrintPrompt();
        ReadInput(input);
        if (IsMetaCommand(input)) {
            MetaCommandResult ret = DoMetaCommand(input);
            if (ret == META_COMMAND_UNRECOGNIZED_COMMAND) {
                printf("Unrecognized command '%s'\n", input->buffer);
            }
            continue;
        }
        Statement *statement;
        PrepareResult ret = PrepareStatement(input, &statement);
        if (ret == PREPARE_SUCCESS) {
            ExecuteStatement(statement);
        } else if (ret == PREPARE_UNRECOGNIZED_STATEMENT) {
            printf("Unrecognized keyword at start of '%s'.\n", input->buffer);
            continue;
        }
    }
    return 0;
}