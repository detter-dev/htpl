typedef enum byte_instruction_kind byte_instruction_kind; 
enum byte_instruction_kind
{
	bi_Constant,
	bi_Add,
	bi_Print,
	bi_Count,
};

typedef s64 value_type;
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

function void
PushU64(u64 Value)
{
	u64 * U64 = (u64*)(ByteCode + NumBytes);
	*U64 = Value;
 NumBytes += 8;
}

function u64
ReadU64(u64 Byte)
{
	Assert(Byte < NumBytes);
	u64 * U64 = (u64*)(ByteCode + Byte);
	u64 Result = *U64;
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
	PushByte(bi_Constant);
	// TODO: Is this how you do this? Or should this value be stored in somekind of State, or similar?
	PushU64(X);
}

function void
Emit_OpAdd(void)
{
	PushByte(bi_Add);
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
				u64 U64 = ReadU64(CurrentByte);
				CurrentByte += 8;
				value_type Value = U64;
				printf("CONSTANT (%ld)\n", Value);
			} break;
			
			case bi_Add:
			{
				puts("ADD");
			} break;
			
			case bi_Print:
			{
				ReadAdvanceCurrentByte();
				puts("PRINT");
			} break;
			
			default: {puts("UNKNOWN BYTE INSTRUCTION");} break;
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
				u64 U64 = ReadU64(CurrentByte);
				CurrentByte += 8;
				value_type Value = U64;
				VM_StackPush(Value);
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