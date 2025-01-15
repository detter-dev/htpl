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
	token_Identifier,
	
	// NOTE: These are not used as token themself,
	// rather just as a way to index into TokenBP array for tokens
	// that can have more than one way of combining (e.g. unary minus and subtraction)
	token_UnaryMinus,
	
	token_Count,
};

typedef u8 token_bp;
global token_bp TokenBP[token_Count] = 
{
	//['$'] = 0,
	
	[token_Number] = 0,
	[token_Identifier] = 0,
	
	['='] = 1,
	
	['+'] = 2,
	['-'] = 2,
	
	['*'] = 3,
	['/'] = 3,
	
	['!'] = 5,
	[token_UnaryMinus] = 5,
};

typedef struct token token;
struct token
{
	token_kind Kind;
	s64 Number;
	//u8 Name;
};
#define T(kind,...) (token){.Kind=(kind), __VA_ARGS__}
#define TNum(x) T(token_Number, .Number = (x))
#define TVar(name) T(token_Identifier, .Number = (name))

global token Tokens[] = 
{
	TVar('a'), T('='), TNum(1886), T('+'), TNum(1),
	TVar('b'), T('='), TNum(4), T('*'), TNum(2), T('-'), TNum(3),
	
	T('$'), TVar('a'),
	T('$'), TVar('b'),
	
	//T('$'), 
	//('-'), TNum(6), T('*'), TNum(-5), T('/'), TNum(0), T('+'), TNum(13),
	//TNum(5),
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

function token 
TokenPrevious(void)
{
	Assert(ParserNextToken);
	token Result = Tokens[ParserNextToken-1];
	return (Result);
}

function void
TokenMatchEat(token_kind ToMatch)
{
	token_kind Current = TokenPeekKind();
	if(ToMatch == Current)
	{
		TokenEat();
	}
	else
	{
		printf("[E] Expected token ID %d, but got %d.\n", ToMatch, Current);
	}
}

function void
Parse_Number(token_bp RightBP)
{
	token Previous = TokenPrevious();
	Emit_Constant(Previous.Number);
}

function void
Parse_OpAdd(token_bp RightBP)
{
	TokenEat();
	Parse_Expression(RightBP);
	Emit_OpAdd();
}

function void
Parse_OpSub(token_bp RightBP)
{
	TokenEat();
	Parse_Expression(RightBP);
	Emit_OpSub();
}

function void
Parse_OpMul(token_bp RightBP)
{
	TokenEat();
	Parse_Expression(RightBP);
	Emit_OpMul();
}

function void
Parse_OpDiv(token_bp RightBP)
{
	TokenEat();
	Parse_Expression(RightBP);
	Emit_OpDiv();
}

function void
Parse_Neg(token_bp RightBP)
{
	token_bp LeftBP = TokenGetBP(token_UnaryMinus);
	Parse_Expression(LeftBP);
	Emit_Neg();
}

function void
Parse_Not(token_bp RightBP)
{
	token_bp LeftBP = TokenGetBP('!');
	Parse_Expression(LeftBP);
	Emit_Not();
}

function void
Parse_Identifier(token_bp RightBP)
{
	token Previous = TokenPrevious();
	Emit_VarRead(Previous.Number);
}

typedef enum operator_kind operator_kind;
enum operator_kind
{
	operator_Prefix,
	operator_Infix,
	operator_Sufix,
	
	operator_Count,
};

global void (*ExpressionFunction[token_Count][operator_Count]) (token_bp BP) = 
{
	['!'] = {Parse_Not, 0, 0},
	
	['+'] = {0, Parse_OpAdd, 0},
	['*'] = {0, Parse_OpMul, 0},
	['/'] = {0, Parse_OpDiv, 0},
	
	['-'] = {Parse_Neg, Parse_OpSub, 0},
	
	[token_Number] = {Parse_Number, 0, 0},
	
	[token_Identifier] = {Parse_Identifier, 0, 0},
};

function void
Parse_Expression(token_bp RightBP)
{
	token Nud = TokenGet();
	TokenEat();
	
	void (*FunctionPrefix)(token_bp BP) = ExpressionFunction[Nud.Kind][operator_Prefix];
	if(FunctionPrefix)
	{
		FunctionPrefix(RightBP);
	}
	
	token_kind NextToken = TokenPeekKind();
	token_bp LeftBP = TokenGetBP(NextToken);
	
	while(LeftBP > RightBP)
	{
		void (*FunctionInifx)(token_bp BP) = ExpressionFunction[NextToken][operator_Infix];
		
		if(FunctionInifx)
		{
			FunctionInifx(LeftBP);
		}
		else
		{
			TokenEat();
			printf("[E] Couldn't find Infix function for token ID %d (\"%c\").\n", NextToken, NextToken);
		}
		
		NextToken = TokenPeekKind();
		LeftBP = TokenGetBP(NextToken);
	}
	
	void (*FunctionSufix)(token_bp BP) = ExpressionFunction[NextToken][operator_Sufix];
	if(FunctionSufix)
	{
		FunctionSufix(LeftBP);
	}
}

function void
Parse_Print(void)
{
	Parse_Expression(0);
	Emit_Print();
}

function void
Parse_VarAss(void)
{
	token VarToken = TokenPrevious();
	TokenMatchEat('=');
	
	token_bp BP = TokenGetBP('=');
	Parse_Expression(BP);
	
	Emit_VarAss(VarToken.Number); 
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
		else if(Token == token_Identifier)
		{
			if(TokenPeekKind() == '=')
			{
				Parse_VarAss();
			}
			
		}
	}
}

void main()
{
	Parse();
	VM_Disassemble();
	VM_Run();
	VM_PrintGlobalState();
}
