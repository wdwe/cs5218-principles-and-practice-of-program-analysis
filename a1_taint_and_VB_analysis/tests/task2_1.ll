; ModuleID = 'tests/task2_1.c'
source_filename = "tests/task2_1.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %k = alloca i32, align 4
  %sink = alloca i32, align 4
  %source = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %0 = load i32, i32* %source, align 4
  store i32 %0, i32* %i, align 4
  %1 = load i32, i32* %j, align 4
  %cmp = icmp sgt i32 %1, 1
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %if.end

if.else:                                          ; preds = %entry
  %2 = load i32, i32* %i, align 4
  store i32 %2, i32* %k, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %3 = load i32, i32* %k, align 4
  store i32 %3, i32* %sink, align 4
  %4 = load i32, i32* %retval, align 4
  ret i32 %4
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0-3~ubuntu0.18.04.1 (tags/RELEASE_700/final)"}
