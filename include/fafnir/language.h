#ifndef FAFNIR_LANGUAGE_H
#define FAFNIR_LANGUAGE_H

enum {
	FNR_OP_NULL,

	FNR_OP_DROP,

	FNR_OP_DUP,

	// Pushes a binding constant on the opstack.
	// Immediate operands:
	// - (uint) Index of binding constant.
	FNR_OP_BINDING,

	// Pushes an immediate constant on the opstack.
	// Immediate operands:
	// - (int) The constant.
	FNR_OP_CONST,

	// Perform arithmetic operations.
	// Opstack inputs:
	// - LHS
	// - RHS
	// Opstack outputs:
	// - Result.
	FNR_OP_ADD,
	FNR_OP_AND,

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
