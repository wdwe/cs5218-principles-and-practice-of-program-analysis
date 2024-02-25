; ModuleID = 'tests/task5_3.c'
source_filename = "tests/task5_3.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %e = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %0 = load i32, i32* %a, align 4
  %cmp = icmp sgt i32 %0, 1
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %1 = load i32, i32* %a, align 4
  store i32 %1, i32* %a, align 4
  %2 = load i32, i32* %c, align 4
  %cmp1 = icmp eq i32 %2, 0
  br i1 %cmp1, label %if.then, label %if.else

if.then:                                          ; preds = %while.body
  %3 = load i32, i32* %a, align 4
  %4 = load i32, i32* %b, align 4
  %rem = srem i32 %3, %4
  store i32 %rem, i32* %d, align 4
  br label %if.end

if.else:                                          ; preds = %while.body
  %5 = load i32, i32* %a, align 4
  %6 = load i32, i32* %b, align 4
  %rem2 = srem i32 %5, %6
  store i32 %rem2, i32* %c, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %7 = load i32, i32* %a, align 4
  %8 = load i32, i32* %e, align 4
  %mul = mul nsw i32 %7, %8
  store i32 %mul, i32* %d, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0-3~ubuntu0.18.04.1 (tags/RELEASE_700/final)"}
