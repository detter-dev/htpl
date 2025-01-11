#include <stdio.h>

#include "types.h"
#include "vm.h"

//- Lexer
typedef enum token_kind token_kind;
enum token_kind
{ 
	// ASCII Reserved
	token_ASCII_END = 255,
	token_Number,
};

typedef struct token token;
struct token
{
	token_kind Kind;
	s64 Number;
};
#define T(kind,...) (token){.Kind=(kind), __VA_ARGS__}
#define TNumber(x) T(token_Number, .Number = (x))

global token Tokens[] = {T('$'), TNumber(5), T('+'), TNumber(8), T('+'), TNumber(2)};
//global token Tokens[] = {T('$'), TNumber(69)};

//- Parser
u64 ParserNextToken = 0;

function void Parse_Expression();

function void
TokenEat(void)
{
	ParserNextToken++;
}

function token_kind 
TokenPeekKind(void)
{
	token_kind Result = Tokens[ParserNextToken].Kind;
	return (Result);
}

function token 
TokenGet(void)
{
	token Result = Tokens[ParserNextToken];
	return (Result);
}

function void
Parse_Number(token Token)
{
	Emit_Constant(Token.Number);
}

function void
Parse_OpAdd(void)
{
	TokenEat(); // TODO: TokenMatchEat
	Parse_Expression();
	Emit_OpAdd();
}

function void
Parse_Expression()
{
	token Left = TokenGet();
	TokenEat();
	
	if(Left.Kind == token_Number)
	{
		Parse_Number(Left);
	}
	
	if(TokenPeekKind() == '+')
	{
		Parse_OpAdd();
	}
}

function void
Parse_Print()
{
	Parse_Expression();
	Emit_Print();
}

function void
Parse(void)
{
	while(ParserNextToken < ArrayLen(Tokens))
	{
		token_kind Token = TokenPeekKind();
		TokenEat();
		
		if(Token == '$')
		{
			Parse_Print();
		}
	}
}

void main()
{
	Parse();
	VM_Disassemble();
	VM_Run();
}
