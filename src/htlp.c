#include <stdio.h>

#include "types.h"

//- Lexer
typedef enum token_kind token_kind;
enum token_kind
{ 
	// ASCII Reserved
	token_ASCII_END = 255,
};
global token_kind Tokens[] = {'2', '+', '8'};
//- Parser

//- VM 
typedef enum byte_instruction_kind byte_instruction_kind; 
typedef s64 value_type;

enum byte_instruction_kind
{
	bi_Write,
	bi_Return,
	bi_Count,
};

global value_type VMState[Pow2(8)] = {0};
global value_type VMStack[Pow2(8)] = {0};
u64 VMStackNext = 0;

global u8 ByteCode[Pow2(10)] = {0};
u64 NumBytes = 0;

function void
PushByte(u8 Byte)
{
	Assert(NumBytes + 1 < ArrayLen(ByteCode));
	ByteCode[NumBytes++] = Byte;
}

function void
PushDoubleByte(u8 FirstByte, u8 SecondByte)
{
	PushByte(FirstByte);
	PushByte(SecondByte);
}

function u8
ReadByte(u64 Byte)
{
	u8 Result = ByteCode[Byte];
	return (Result);
}

function void
VM_StackPush(value_type X)
{
	Assert(VMStackNext + 1 < ArrayLen(VMStack));
	VMStack[VMStackNext++] = X;
}

function value_type
VM_StackPop(void)
{
	Assert(VMStackNext);
	value_type Result = VMStack[--VMStackNext];
	return (Result);
}

function value_type
VM_StateRead(u8 Slot)
{
	Assert(Slot < ArrayLen(VMState));
	value_type Result = VMState[Slot];
	return (Result);
}

function void
VM_StateWrite(u8 Slot, value_type X)
{
	Assert(Slot < ArrayLen(VMState));
	VMState[Slot] = X;
}

#define ReadAdvanceCurrentByte() ReadByte(CurrentByte++);
function void
VM_Disassemble(void)
{
	u64 CurrentByte = 0;
	while(CurrentByte < NumBytes)
	{
		u8 ByteValue = ReadAdvanceCurrentByte();
		printf("[%ld]", CurrentByte - 1); 
		
		switch(ByteValue)
		{
			case bi_Write:
			{
				u8 Slot  = ReadAdvanceCurrentByte();
				u8 Value = ReadAdvanceCurrentByte();
				printf(" WRITE {%d} <- %d \n", Slot, Value);
			} break;
			
			case bi_Return:
			{
				u8 Value = ReadAdvanceCurrentByte();
				if(!Value) puts(" RET"); 
				else printf(" RET {%d} : %ld \n", Value, VMState[Value]);
			} break;
			
			default: {puts("INVALID BYTE INSTRUCTION");} break;
		}
	}
}

function void
VM_Run(void)
{
	u64 CurrentByte = 0;
	while(CurrentByte < NumBytes)
	{
		u8 ByteValue = ReadAdvanceCurrentByte();
		switch(ByteValue)
		{
			case bi_Write:
			{
				u8 Slot  = ReadAdvanceCurrentByte();
				u8 Value = ReadAdvanceCurrentByte();
				VM_StateWrite(Slot, Value);
			} break;
			
			case bi_Return:
			{
				u8 Slot = ReadAdvanceCurrentByte();
				value_type X = VM_StateRead(Slot);
				VM_StackPush(X);
			} break;
			
			default: {} break;
		}
	}
}

function void
VM_PrintState()
{
	for(u64 i = 0; i < ArrayLen(VMState); i++)
	{
		value_type Value = VM_StateRead(i);
		if(Value)
		{
			printf("{%ld} : %ld\n", i, Value);
		}
	}
}

void main()
{
	PushDoubleByte(bi_Return, 69);
	
	PushByte(bi_Write);
	PushDoubleByte(69, 155);
	
	VM_Disassemble();
	VM_Run();
	VM_PrintState();
}
