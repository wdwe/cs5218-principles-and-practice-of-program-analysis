; ModuleID = 'tests/task4_2.c'
source_filename = "tests/task4_2.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %sink = alloca i32, align 4
  %source = alloca i32, align 4
  %N = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 0, i32* %a, align 4
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %0 = load i32, i32* %a, align 4
  %1 = load i32, i32* %N, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %2 = load i32, i32* %a, align 4
  %rem = srem i32 %2, 2
  %cmp1 = icmp eq i32 %rem, 1
  br i1 %cmp1, label %if.then, label %if.else

if.then:                                          ; preds = %while.body
  %3 = load i32, i32* %source, align 4
  store i32 %3, i32* %i, align 4
  br label %if.end

if.else:                                          ; preds = %while.body
  %4 = load i32, i32* %i, align 4
  store i32 %4, i32* %j, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %5 = load i32, i32* %a, align 4
  %inc = add nsw i32 %5, 1
  store i32 %inc, i32* %a, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %6 = load i32, i32* %j, align 4
  store i32 %6, i32* %sink, align 4
  br label %while.cond2

while.cond2:                                      ; preds = %if.end9, %while.end
  %7 = load i32, i32* %a, align 4
  %8 = load i32, i32* %N, align 4
  %mul = mul nsw i32 2, %8
  %cmp3 = icmp slt i32 %7, %mul
  br i1 %cmp3, label %while.body4, label %while.end10

while.body4:                                      ; preds = %while.cond2
  %9 = load i32, i32* %a, align 4
  %10 = load i32, i32* %N, align 4
  %mul5 = mul nsw i32 2, %10
  %sub = sub nsw i32 %mul5, 1
  %cmp6 = icmp eq i32 %9, %sub
  br i1 %cmp6, label %if.then7, label %if.else8

if.then7:                                         ; preds = %while.body4
  %11 = load i32, i32* %N, align 4
  %add = add nsw i32 %11, 1
  store i32 %add, i32* %i, align 4
  br label %if.end9

if.else8:                                         ; preds = %while.body4
  %12 = load i32, i32* %i, align 4
  store i32 %12, i32* %j, align 4
  br label %if.end9

if.end9:                                          ; preds = %if.else8, %if.then7
  br label %while.cond2

while.end10:                                      ; preds = %while.cond2
  %13 = load i32, i32* %j, align 4
  store i32 %13, i32* %sink, align 4
  %14 = load i32, i32* %retval, align 4
  ret i32 %14
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0-3~ubuntu0.18.04.1 (tags/RELEASE_700/final)"}
