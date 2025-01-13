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
	
	token_Count,
};

typedef u8 token_bp;
global token_bp TokenBP[token_Count] = 
{
	//['$'] = 0,
	
	[token_Number] = 1,
	
	['+'] = 2,
	['*'] = 3,
};

typedef struct token token;
struct token
{
	token_kind Kind;
	s64 Number;
};
#define T(kind,...) (token){.Kind=(kind), __VA_ARGS__}
#define TNumber(x) T(token_Number, .Number = (x))

global token Tokens[] = 
{
	T('$'), 
	TNumber(3), T('*'), TNumber(5), T('+'), TNumber(2), T('*'), TNumber(4),
};

//- Parser
u64 ParserNextToken = 0;

function void Parse_Expression(u8 RightBindingPower);

function token_bp
TokenGetBP(token_kind Token)
{
	Assert(Token < token_Count);
	token_bp Result = TokenBP[Token];
	return (Result);
}

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
Parse_OpAdd(token_bp RightBP)
{
	TokenEat(); // TODO: TokenMatchEat
	Parse_Expression(RightBP);
	Emit_OpAdd();
}

function void
Parse_OpMul(token_bp RightBP)
{
	TokenEat(); // TODO: TokenMatchEat
	Parse_Expression(RightBP);
	Emit_OpMul();
}

function void
Parse_Expression(token_bp RightBP)
{
	token Nud = TokenGet();
	TokenEat();
	
	if(Nud.Kind == token_Number)
	{
		Parse_Number(Nud);
	}
	
	token_kind NextToken = TokenPeekKind();
	token_bp LeftBP = TokenGetBP(NextToken);
	
	while(LeftBP > RightBP)
	{
		if(NextToken == '+')
		{
			Parse_OpAdd(LeftBP);
		}
		else if(NextToken == '*')
		{
			Parse_OpMul(LeftBP);
		}
		
		NextToken = TokenPeekKind();
		LeftBP = TokenGetBP(NextToken);
	}
}

function void
Parse_Print(void)
{
	Parse_Expression(0);
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
