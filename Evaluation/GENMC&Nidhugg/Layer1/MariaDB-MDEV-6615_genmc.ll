; ModuleID = 'test/MariaDB-MDEV-6615_genmc.c'
source_filename = "test/MariaDB-MDEV-6615_genmc.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%union.pthread_attr_t = type { i64, [48 x i8] }

@data = dso_local global i32 0, align 4
@flag = dso_local global i32 0, align 4
@start = dso_local global i32 0, align 4
@.str = private unnamed_addr constant [2 x i8] c"0\00", align 1
@.str.1 = private unnamed_addr constant [31 x i8] c"test/MariaDB-MDEV-6615_genmc.c\00", align 1
@__PRETTY_FUNCTION__.reader = private unnamed_addr constant [21 x i8] c"void *reader(void *)\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @writer(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i8* %0, i8** %2, align 8
  br label %6

6:                                                ; preds = %11, %1
  %7 = load atomic i32, i32* @start monotonic, align 4
  store i32 %7, i32* %3, align 4
  %8 = load i32, i32* %3, align 4
  %9 = icmp ne i32 %8, 0
  %10 = xor i1 %9, true
  br i1 %10, label %11, label %12

11:                                               ; preds = %6
  br label %6

12:                                               ; preds = %6
  store i32 42, i32* %4, align 4
  %13 = load i32, i32* %4, align 4
  store atomic i32 %13, i32* @data monotonic, align 4
  store i32 1, i32* %5, align 4
  %14 = load i32, i32* %5, align 4
  store atomic i32 %14, i32* @flag monotonic, align 4
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @reader(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i8* %0, i8** %2, align 8
  br label %8

8:                                                ; preds = %13, %1
  %9 = load atomic i32, i32* @start monotonic, align 4
  store i32 %9, i32* %3, align 4
  %10 = load i32, i32* %3, align 4
  %11 = icmp ne i32 %10, 0
  %12 = xor i1 %11, true
  br i1 %12, label %13, label %14

13:                                               ; preds = %8
  br label %8

14:                                               ; preds = %8
  %15 = load atomic i32, i32* @flag monotonic, align 4
  store i32 %15, i32* %5, align 4
  %16 = load i32, i32* %5, align 4
  store i32 %16, i32* %4, align 4
  %17 = load atomic i32, i32* @data monotonic, align 4
  store i32 %17, i32* %7, align 4
  %18 = load i32, i32* %7, align 4
  store i32 %18, i32* %6, align 4
  %19 = load i32, i32* %4, align 4
  %20 = icmp eq i32 %19, 1
  br i1 %20, label %21, label %25

21:                                               ; preds = %14
  %22 = load i32, i32* %6, align 4
  %23 = icmp ne i32 %22, 42
  br i1 %23, label %24, label %25

24:                                               ; preds = %21
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([31 x i8], [31 x i8]* @.str.1, i64 0, i64 0), i32 30, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @__PRETTY_FUNCTION__.reader, i64 0, i64 0)) #4
  unreachable

25:                                               ; preds = %21, %14
  ret i8* null
}

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %4, align 4
  %6 = load i32, i32* %4, align 4
  store atomic i32 %6, i32* @start monotonic, align 4
  %7 = call i32 @pthread_create(i64* %2, %union.pthread_attr_t* null, i8* (i8*)* @writer, i8* null) #5
  %8 = call i32 @pthread_create(i64* %3, %union.pthread_attr_t* null, i8* (i8*)* @reader, i8* null) #5
  store i32 1, i32* %5, align 4
  %9 = load i32, i32* %5, align 4
  store atomic i32 %9, i32* @start monotonic, align 4
  %10 = load i64, i64* %2, align 8
  %11 = call i32 @pthread_join(i64 %10, i8** null)
  %12 = load i64, i64* %3, align 8
  %13 = call i32 @pthread_join(i64 %12, i8** null)
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #2

declare dso_local i32 @pthread_join(i64, i8**) #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noreturn nounwind }
attributes #5 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Debian clang version 11.0.1-2"}
