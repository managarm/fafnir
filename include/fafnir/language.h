#ifndef FAFNIR_LANGUAGE_H
#define FAFNIR_LANGUAGE_H

// Fafnir has two stacks that are manipulated by operations:
// (i)  The operand stack (opstack):
//      This is used for temporary values during computations.
// (ii) The scope stack (sstack):
//      This is used for operands that live longer.
//      The scope stack is valid until the current scope exits.
//      Scope stacks of outer scopes are implicitly available to the current scope.
//      At scope exit, the values of the scope stack are pushed to the opstack.

enum {
	FNR_OP_NULL,

	// Drops a value from the opstack.
	// Opstack inputs:
	// - Value to be dropped.
	FNR_OP_DROP,

	// Duplicate an opstack value.
	// Opstack inputs:
	// - Value to be duplicated.
	// Opstack outputs:
	// - The duplicate value.
	FNR_OP_DUP,

	// Pushes a binding constant on the opstack.
	// Immediate operands:
	// - (uint) Index of binding constant.
	FNR_OP_BINDING,

	// Pushes a value onto the sstack.
	// Opstack inputs:
	// - Value to be pushed.
	FNR_OP_S_DEFINE,

	// Retrieves a value from the sstack.
	// Immediate operands:
	// - (uint) Index into the scope stack.
	FNR_OP_S_VALUE,

	// Pushes an immediate constant on the opstack.
	// Immediate operands:
	// - (int) The constant.
	FNR_OP_LITERAL,

	// Perform arithmetic operations.
	// Opstack inputs:
	// - LHS
	// - RHS
	// Opstack outputs:
	// - Result.
	FNR_OP_BITWISE_AND,
	FNR_OP_ADD,

	// Call an intrinsic function.
	// Immediate operands:
	// - (string) Name of the function.
	// Opstack inputs:
	// - Arguments to the intrinsic function.
	// Opstack outputs:
	// - Result of the function.
	FNR_OP_INTRIN,
};

#endif // FAFNIR_LANGUAGE_H
