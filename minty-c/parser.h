#ifndef PARSER
#define PARSER

Expression *parse_expression(LinkedList *tokens);

Statement *parse_statement(LinkedList *tokens);

LinkedList *parse_statement_block(LinkedList *tokens);

FNDecl *parse_function(LinkedList *tokens);

Program *parse_program(LinkedList *tokens);

#endif // PARSER