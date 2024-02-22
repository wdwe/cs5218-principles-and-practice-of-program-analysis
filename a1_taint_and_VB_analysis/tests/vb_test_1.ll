; ModuleID = 'tests/vb_test_1.c'
source_filename = "tests/vb_test_1.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %0 = load i32, i32* %a, align 4
  %1 = load i32, i32* %b, align 4
  %cmp = icmp sgt i32 %0, %1
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %2 = load i32, i32* %b, align 4
  %3 = load i32, i32* %a, align 4
  %sub = sub nsw i32 %2, %3
  store i32 %sub, i32* %x, align 4
  %4 = load i32, i32* %a, align 4
  %5 = load i32, i32* %b, align 4
  %sub1 = sub nsw i32 %4, %5
  store i32 %sub1, i32* %y, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  %6 = load i32, i32* %b, align 4
  %7 = load i32, i32* %a, align 4
  %sub2 = sub nsw i32 %6, %7
  store i32 %sub2, i32* %y, align 4
  %8 = load i32, i32* %a, align 4
  %9 = load i32, i32* %b, align 4
  %sub3 = sub nsw i32 %8, %9
  store i32 %sub3, i32* %x, align 4
  store i32 1, i32* %retval, align 4
  br label %return

if.end:                                           ; preds = %if.then
  store i32 0, i32* %retval, align 4
  br label %return

return:                                           ; preds = %if.end, %if.else
  %10 = load i32, i32* %retval, align 4
  ret i32 %10
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 7.0.0-3~ubuntu0.18.04.1 (tags/RELEASE_700/final)"}
