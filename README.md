Lazy functional language interpreter
====================================

Interpreter for a toy lazy, functional language with syntax inspired by Rust.

### Language features:

* Two build-in types: _f32_ and _function_ (string are supported only as an argument to _print_ function)
* Support for string concatenation
* Conditional expressions with build-in function _if_
* Lazy evaluation
* Lambdas
* Higher-order functions
* Semantic analysis and acceptable error messages

This was made as a final project for *Compilation Techniques* course (2020). 

### Build:

```
cmake .
make -j8
```



### Test & run:

```
./bin/interpreter_tests
./bin/interpreter input_file
```



### Sample programs:

```rust
fn main(): f32 {
	print("Hello World!"); // Hello World!
	ret 0;
}
```
---
```rust
fn cube(x: f32): f32 { ret x*x*x; }
fn mul2(x: f32): f32 { ret x*2.0; }

fn main(): f32 {
	print("" : mul2(cube(2))); // 16
	ret 0;
}
```
---
```rust
fn test(x: f32, y: f32): f32 {
	ret if((x > 0 && x < 2) || y < 0,
		10,
		42);
} 

fn main(): f32 {
	print("" : test(1, 2)); // 10 
	print("" : test(-1.25, 1)); // 42
	ret 0;
}
```
---
```rust
fn makeMul(m: f32): function {
	ret \(x: f32): f32 = { ret m*x; };
}

fn main(): f32 {
	print("" : makeMul(2)(4)); // 8
	ret 0;
}
```
---
```rust
fn fact(n: f32): f32 {
	ret if(n == 0, 1, n * fact(n-1));
}

fn main(): f32 {
	print("6! = " : fact(6)); // 6! = 720
	
	let x: f32 = 12;
	let y: f32 = x*2 + 2*(3-1);
	let res: f32 = if(x >= 6, x, y);
	
	print("" : res); // 12
	ret 0;
}
```
---
```rust
fn hang(): f32
{
  ret hang();
}

fn main(): f32
{
  let test: f32 = hang();
  let result: f32 = if(2 == 2, 42, test);
  print("Result: " : result); // Result: 42
  ret 0;
}
```

### Grammar:

```
<program> ::= <fun-decl>, { <fun-decl> | <var-decl> };

<fun-decl> ::= "fn", <iden>, "(", <arg-list-decl>, ")", ":", <type>, <block>;

<arg-list-decl> ::= [ <iden>, ":", <type>, { ",", <iden>, ":", <type> } ];
<arg-list-call> ::= [ <arg>, { ",", <arg> } ];

<var-decl> ::= "let", <iden>, ":", <type>, "=", (<arith-expr> | <lambda>), ";";
<arith-expr> ::= <add-expr>, { <bitop>, <add-expr> };
<add-expr> ::= <factor>, { ("+" | "-" | "%"), <factor> };
<factor> ::= <unary>, { ("*" | "/"), <unary> };
<unary> ::= [ "~" | "-" ], <term>;
<term> ::= <number> | "(", <logical-expr>, ")" | <iden> | <fcall>;

<logical-expr> ::= <unary-logical-expr>, { ("&&" | "||"), <unary-logical-expr> };
<unary-logical-expr> ::= [ "!" ], <comp-expr>;
<comp-expr> ::= <arith-expr>, { <cop>, <arith-expr> };

<string-expr> ::= <string>, { ":", (<string> | <arith-expr>) };

<fcall> ::= (<iden> | ("(", <lambda>, ")")), { "(", <arg-list-call>, ")" };
<arg> ::= <logical-expr> | <string-expr> | <lambda>;

<assign> ::= <iden>, <aop>, <arith-expr>, ";" | <iden>, "=", (<arith-expr> | <lambda>), ";";
<lambda> ::= "\", "(", <arg-list-decl>, ")", ":", <type>, "=", <block>;
<ret> ::= "ret", (<arith-expr> | <lambda>), ";";
<block> ::= "{", { <var-decl> | <assign> | <ret> | <call-statement> }, "}";

<call-statement> ::= <fcall>, ";";

<number> ::= [ "-" ], <nz-digit>, { <digit> } | [ "-" ], <nz-digit>, { <digit> }, ".", { <digit> } | [ "-" ] <digit>,"." { <digit> };
<iden> ::= <char>, { <char> | <digit> };

<cop> ::= "==" | "!=" | ">" | "<" | ">=" | "<=";
<aop> ::= "=" | "+=" | "-=" | "*=" | "/=" | "&=" | "|=" | "<<=" | ">>=";
<bitop> ::= "&" | "|" | "^" | "<<" | ">>";
<type> ::= "f32" | "function" | "void";
<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
<nz-digit> ::= "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
<char> ::= "a" | "b" | ... | "z" | "A" | "B" | … | "Z" | "_";
<string> ::= """, { <char> | <digit> }, """; (* simplified *) 
```

