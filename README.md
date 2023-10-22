# minilang compiler

Compiler for a mini imperative programming language, targetting LLVM. It produces LLVM IR
which can be compiled to object code.

Lexer/Parser written with Flex and Bison.

Uses the LLVM C++ API to generate LLVM IR.

WIP

examples:

```
func fib(n) {
    if (n < 2) {
        return 1;
    }
    return fib(n-1) + fib(n-2);
}
```

output
```
define double @fib(double %n) {
entry:
  %n1 = alloca double, align 8
  store double %n, ptr %n1, align 8
  %cmp_lt_tmp = fcmp ult double %n, 2.000000e+00
  br i1 %cmp_lt_tmp, label %common.ret, label %elseconseq

common.ret:                                       ; preds = %entry, %elseconseq
  %common.ret.op = phi double [ %addtmp, %elseconseq ], [ 1.000000e+00, %entry ]
  ret double %common.ret.op

elseconseq:                                       ; preds = %entry
  %subtmp = fadd double %n, -1.000000e+00
  %calltmp = call double @fib(double %subtmp)
  %subtmp5 = fadd double %n, -2.000000e+00
  %calltmp6 = call double @fib(double %subtmp5)
  %addtmp = fadd double %calltmp, %calltmp6
  br label %common.ret
}
```

iterative fibonacci

```
func fib(n) {
    fibp1 = 1;
    fibp2 = 1;
    idx = 0;
    ret = 0;
    while (idx < n-1) {
        ret = fibp1 + fibp2;
        tmp = fibp2;
        fibp2 = ret;
        fibp1 = tmp;
        idx = idx + 1;
    }
    return ret;
}
```

output

```
define double @fib(double %n) {
entry:
  %ret = alloca double, align 8
  %idx = alloca double, align 8
  %fibp2 = alloca double, align 8
  %fibp1 = alloca double, align 8
  %n1 = alloca double, align 8
  store double %n, ptr %n1, align 8
  store double 1.000000e+00, ptr %fibp1, align 8
  store double 1.000000e+00, ptr %fibp2, align 8
  store double 0.000000e+00, ptr %idx, align 8
  store double 0.000000e+00, ptr %ret, align 8
  br label %whiletop

whiletop:                                         ; preds = %whilebody, %entry
  %fibp25 = phi double [ %addtmp, %whilebody ], [ 1.000000e+00, %entry ]
  %fibp14 = phi double [ %fibp25, %whilebody ], [ 1.000000e+00, %entry ]
  %ret11 = phi double [ %addtmp, %whilebody ], [ 0.000000e+00, %entry ]
  %idx9 = phi double [ %addtmp10, %whilebody ], [ 0.000000e+00, %entry ]
  %subtmp = fadd double %n, -1.000000e+00
  %cmp_lt_tmp = fcmp ult double %idx9, %subtmp
  br i1 %cmp_lt_tmp, label %whilebody, label %whiledone

whilebody:                                        ; preds = %whiletop
  %addtmp = fadd double %fibp14, %fibp25
  store double %addtmp, ptr %ret, align 8
  store double %addtmp, ptr %fibp2, align 8
  store double %fibp25, ptr %fibp1, align 8
  %addtmp10 = fadd double %idx9, 1.000000e+00
  store double %addtmp10, ptr %idx, align 8
  br label %whiletop

whiledone:                                        ; preds = %whiletop
  ret double %ret11
}
```
