typedef enum byte_instruction_kind byte_instruction_kind; 
enum byte_instruction_kind
{
	bi_Constant,
	
	bi_Add,
	bi_Mul,
	
	bi_Print,
	bi_Count,
};

typedef s64 value_type;
global value_type VMStack[Pow2(8)] = {0};
u64 VMStackNext = 0;

global value_type VMState[Pow2(8)] = {0};

global u8 ByteCode[Pow2(10)] = {0};
u64 NumBytes = 0;

function void
PushByte(u8 Byte)
{
	Assert(NumBytes + 1 < ArrayLen(ByteCode));
	ByteCode[NumBytes++] = Byte;
}

function u8
ReadByte(u64 Byte)
{
	Assert(Byte < NumBytes);
	u8 Result = ByteCode[Byte];
	return (Result);
}

function void
PushDoubleByte(u8 FirstByte, u8 SecondByte)
{
	PushByte(FirstByte);
	PushByte(SecondByte);
}

function value_type
VM_ReadState(u8 Slot)
{
	Assert(Slot < ArrayLen(VMState));
	value_type Result = VMState[Slot];
	return (Result);
}

function u8
VM_WriteState(value_type Value)
{
	local u8 Slot = 0;
	Slot = (Slot + 1) % ArrayLen(VMState);
	VMState[Slot] = Value;
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
VM_StackPeek(u64 Delta)
{
	Assert(Delta < VMStackNext);
	Assert(VMStackNext);
	
	u64 Index = (VMStackNext-1) - Delta;
	Assert(Index >= 0);
	Assert(Index < ArrayLen(VMStack));
	
	value_type Result = VMStack[Index];
	return (Result);
}

function void
Emit_Constant(value_type X)
{
	u8 Slot = VM_WriteState(X);
	PushDoubleByte(bi_Constant, Slot);
}

function void
Emit_OpAdd(void)
{
	PushByte(bi_Add);
}

function void
Emit_OpMul(void)
{
	PushByte(bi_Mul);
}

function void
Emit_Print(void)
{
	PushByte(bi_Print);
}

#define ReadAdvanceCurrentByte() ReadByte(CurrentByte++);

function void
VM_Disassemble(void)
{
	u64 CurrentByte = 0;
	while(CurrentByte < NumBytes)
	{
		u8 ByteValue = ReadAdvanceCurrentByte();
		printf("[%ld] ", CurrentByte - 1); 
		
		switch(ByteValue)
		{
			case bi_Constant:
			{
				u8 Slot = ReadAdvanceCurrentByte();
				value_type Value = VM_ReadState(Slot);
				printf("CONSTANT (%ld)\n", Value);
			} break;
			
			case bi_Mul:
			{
				puts("MUL");
			} break;
			
			case bi_Add:
			{
				puts("ADD");
			} break;
			
			case bi_Print:
			{
				puts("PRINT");
			} break;
			
			default: {printf("UNKNOWN BYTE INSTRUCTION: %d \n", ByteValue);} break;
		}
	}
}

function void
VM_Run(void)
{
	puts("___ VM ___");
	u64 CurrentByte = 0;
	while(CurrentByte < NumBytes)
	{
		u8 ByteValue = ReadAdvanceCurrentByte();
		switch(ByteValue)
		{
			case bi_Constant:
			{
				u8 Slot = ReadAdvanceCurrentByte();
				value_type Value = VM_ReadState(Slot);
				VM_StackPush(Value);
			} break;
			
			case bi_Mul:
			{
				value_type Right = VM_StackPop();
				value_type Left  = VM_StackPop();
				VM_StackPush(Left * Right);
			} break;
			
			case bi_Add:
			{
				value_type Right = VM_StackPop();
				value_type Left  = VM_StackPop();
				VM_StackPush(Left + Right);
			} break;
			
			case bi_Print:
			{
				value_type Value = VM_StackPop();
				printf("print: %ld\n", Value);
			} break;
			
			default: {} break;
		}
	}
	puts("__________");
}

function void
VM_PrintStack(void)
{
	for(u64 i = 0; i < VMStackNext; i++)
	{
		value_type Value = VM_StackPeek(i);
		printf("<%ld> : %ld\n", i, Value);
	}
}