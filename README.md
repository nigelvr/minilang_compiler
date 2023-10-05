# minilang compiler

Compiler for a mini imperative programming language, targetting LLVM. It produces LLVM IR
which can be compiled to object code.

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