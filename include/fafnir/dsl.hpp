#ifndef FAFNIR_DSL_HPP
#define FAFNIR_DSL_HPP

#include <fafnir/language.h>

namespace fnr {

// --------------------------------------------------------------------------------------
// Structs to represent instructions.
// --------------------------------------------------------------------------------------

template<typename C>
struct expr_crtp;

template<typename F, typename... Args>
struct apply_expr : expr_crtp<apply_expr<F, Args...>> {
	apply_expr(F fn_, Args... args_)
	: fn{fn_}, args{args_...} { }

	F fn;
	std::tuple<Args...> args;
};

template<typename L, typename R>
struct add_expr {
	L lhs;
	R rhs;
};

template<typename L, typename R>
struct and_expr {
	L lhs;
	R rhs;
};

template<typename C>
struct expr_crtp {
	template<typename... Args>
	apply_expr<C, Args...> operator() (Args... args) const {
		return apply_expr<C, Args...>{*static_cast<const C *>(this), args...};
	}

	template<typename R>
	add_expr<C, R> operator+ (R rhs) const {
		return add_expr<C, R>{*static_cast<const C *>(this), rhs};
	}

	template<typename R>
	and_expr<C, R> operator& (R rhs) const {
		return and_expr<C, R>{*static_cast<const C *>(this), rhs};
	}
};

namespace code {
	struct drop : expr_crtp<drop> {
	};

	struct dup : expr_crtp<dup> {
		size_t index;
	};

	struct binding : expr_crtp<binding> {
		binding(size_t index_)
		: index{index_} { }

		size_t index;
	};

	struct scope_push : expr_crtp<scope_push> { };

	struct scope_get : expr_crtp<scope_get> {
		scope_get(size_t index_)
		: index{index_} { }

		size_t index;
	};

	struct check_if : expr_crtp<check_if> { };
	struct then : expr_crtp<then> { };
	struct else_then : expr_crtp<else_then> { };
	struct end : expr_crtp<end> { };

	struct literal : expr_crtp<literal> {
		literal(int64_t value_)
		: value{value_} { }

		int64_t value;
	};

	struct add : expr_crtp<add> { };
	struct bitwise_and : expr_crtp<bitwise_and> { };

	struct intrin : expr_crtp<intrin> {
		intrin(const char *name_, int num_args_, int num_retvals_)
		: name{name_}, num_args{num_args_}, num_retvals{num_retvals_} { }

		const char *name;
		int num_args;
		int num_retvals;
	};
}

using namespace code;

template<typename T>
struct code_traits;

template <typename T, typename = void>
struct is_code : std::false_type {};

template <typename T>
struct is_code<T, std::void_t<decltype(code_traits<T>::emit(std::declval<std::vector<char>::iterator>(), std::declval<const T &>))>>
: std::true_type {};

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
struct code_traits<scope_push> {
	template<typename Iterator>
	static void emit(Iterator &it, const scope_push &c) {
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
struct code_traits<scope_get> {
	template<typename Iterator>
	static void emit(Iterator &it, const scope_get &c) {
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

template<typename F, typename... Args>
struct code_traits<apply_expr<F, Args...>> {
	template<typename Iterator>
	static void emit(Iterator &it, const apply_expr<F, Args...> &c) {
		_emit_tuple(it, c.args, std::index_sequence_for<Args...>{});
		code_traits<F>::emit(it, c.fn);
	}

private:
	template<typename Iterator, size_t... I>
	static void _emit_tuple(Iterator &it, const std::tuple<Args...> &t,
			std::index_sequence<I...>) {
		// Same trick as in emit_to().
		auto seq = {
			(code_traits<Args>::emit(it, std::get<I>(t)), 0)...
		};
		(void)seq;
	}
};

template<typename L, typename R>
struct code_traits<add_expr<L, R>> {
	template<typename Iterator>
	static void emit(Iterator &it, const add_expr<L, R> &c) {
		code_traits<L>::emit(it, c.lhs);
		code_traits<R>::emit(it, c.rhs);
		code_traits<add>::emit(it, add{});
	}
};

template<typename L, typename R>
struct code_traits<and_expr<L, R>> {
	template<typename Iterator>
	static void emit(Iterator &it, const and_expr<L, R> &c) {
		code_traits<L>::emit(it, c.lhs);
		code_traits<R>::emit(it, c.rhs);
		code_traits<bitwise_and>::emit(it, bitwise_and{});
	}
};

} // namespace fnr

#endif // FAFNIR_DSL_HPP
