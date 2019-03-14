#ifndef FAFNIR_DSL_HPP
#define FAFNIR_DSL_HPP

#include <fafnir/language.h>

namespace fnr {

// --------------------------------------------------------------------------------------
// Structs to represent instructions.
// --------------------------------------------------------------------------------------

namespace code {
	struct drop { };

	struct dup {
		size_t index;
	};

	struct binding {
		size_t index;
	};

	struct s_define { };

	struct s_value {
		size_t index;
	};

	struct check_if { };
	struct then { };
	struct else_then { };
	struct end { };

	struct literal {
		int64_t value;
	};

	struct add { };
	struct bitwise_and { };

	struct intrin {
		const char *name;
		int num_args;
		int num_retvals;
	};
}

using namespace code;

template<typename T>
struct code_traits;

template<>
struct code_traits<drop> {
	template<typename Iterator>
	static void emit(Iterator &it, const drop &) {
		*(it++) = FNR_OP_DROP;
	}
};

template<>
struct code_traits<dup> {
	template<typename Iterator>
	static void emit(Iterator &it, const dup &c) {
		*(it++) = FNR_OP_DUP;
		*(it++) = c.index;
	}
};

template<>
struct code_traits<binding> {
	template<typename Iterator>
	static void emit(Iterator &it, const binding &c) {
		*(it++) = FNR_OP_BINDING;
		*(it++) = c.index;
	}
};

template<>
struct code_traits<s_define> {
	template<typename Iterator>
	static void emit(Iterator &it, const s_define &c) {
		*(it++) = FNR_OP_S_DEFINE;
	}
};

template<>
struct code_traits<check_if> {
	template<typename Iterator>
	static void emit(Iterator &it, const check_if &) {
		*(it++) = FNR_OP_CHECK_IF;
	}
};

template<>
struct code_traits<then> {
	template<typename Iterator>
	static void emit(Iterator &it, const then &) {
		*(it++) = FNR_OP_THEN;
	}
};

template<>
struct code_traits<else_then> {
	template<typename Iterator>
	static void emit(Iterator &it, const else_then &) {
		*(it++) = FNR_OP_ELSE_THEN;
	}
};

template<>
struct code_traits<end> {
	template<typename Iterator>
	static void emit(Iterator &it, const end &) {
		*(it++) = FNR_OP_END;
	}
};

template<>
struct code_traits<s_value> {
	template<typename Iterator>
	static void emit(Iterator &it, const s_value &c) {
		*(it++) = FNR_OP_S_VALUE;
		*(it++) = c.index;
	}
};

template<>
struct code_traits<literal> {
	template<typename Iterator>
	static void emit(Iterator &it, const literal &c) {
		*(it++) = FNR_OP_LITERAL;
		*(it++) = c.value;
	}
};

template<>
struct code_traits<add> {
	template<typename Iterator>
	static void emit(Iterator &it, const add &) {
		*(it++) = FNR_OP_ADD;
	}
};

template<>
struct code_traits<bitwise_and> {
	template<typename Iterator>
	static void emit(Iterator &it, const bitwise_and &) {
		*(it++) = FNR_OP_BITWISE_AND;
	}
};

template<>
struct code_traits<intrin> {
	template<typename Iterator>
	static void emit(Iterator &it, const intrin &c) {
		*(it++) = FNR_OP_INTRIN;
		*(it++) = c.num_args;
		*(it++) = c.num_retvals;
		auto s = c.name;
		while(*s)
			*(it++) = *(s++);
		*(it++) = 0;
	}
};

template<typename Iterator, typename... T>
void emit_to(Iterator it, const T &... cs) {
	// Construct an initializer list to get a left-to-right evaluation order.
	// Use the comma operator to discard the void result of emit().
	auto seq = {
		(code_traits<T>::emit(it, cs), 0)...
	};
	(void)seq;
}

} // namespace fnr

#endif // FAFNIR_DSL_HPP
