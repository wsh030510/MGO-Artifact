; ModuleID = 'test/3-sel4_genmc.c'
source_filename = "test/3-sel4_genmc.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%union.pthread_attr_t = type { i64, [48 x i8] }

@lock_tail = dso_local global i32 0, align 4
@in_cs = dso_local global i32 0, align 4
@nodes_locked = dso_local global [3 x i32] zeroinitializer, align 4
@.str = private unnamed_addr constant [8 x i8] c"cs == 0\00", align 1
@.str.1 = private unnamed_addr constant [20 x i8] c"test/3-sel4_genmc.c\00", align 1
@__PRETTY_FUNCTION__.thread_clh = private unnamed_addr constant [25 x i8] c"void *thread_clh(void *)\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread_clh(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  store i8* %0, i8** %2, align 8
  %15 = load i8*, i8** %2, align 8
  %16 = ptrtoint i8* %15 to i64
  %17 = trunc i64 %16 to i32
  store i32 %17, i32* %3, align 4
  %18 = load i32, i32* %3, align 4
  %19 = sext i32 %18 to i64
  %20 = getelementptr inbounds [3 x i32], [3 x i32]* @nodes_locked, i64 0, i64 %19
  store i32 1, i32* %4, align 4
  %21 = load i32, i32* %4, align 4
  store atomic i32 %21, i32* %20 monotonic, align 4
  %22 = load i32, i32* %3, align 4
  store i32 %22, i32* %6, align 4
  %23 = load i32, i32* %6, align 4
  %24 = atomicrmw xchg i32* @lock_tail, i32 %23 acquire
  store i32 %24, i32* %7, align 4
  %25 = load i32, i32* %7, align 4
  store i32 %25, i32* %5, align 4
  %26 = load i32, i32* %5, align 4
  %27 = icmp ne i32 %26, 0
  br i1 %27, label %28, label %48

28:                                               ; preds = %1
  %29 = load i32, i32* %5, align 4
  %30 = sext i32 %29 to i64
  %31 = getelementptr inbounds [3 x i32], [3 x i32]* @nodes_locked, i64 0, i64 %30
  %32 = load atomic i32, i32* %31 monotonic, align 4
  store i32 %32, i32* %8, align 4
  %33 = load i32, i32* %8, align 4
  %34 = icmp eq i32 %33, 0
  br i1 %34, label %35, label %47

35:                                               ; preds = %28
  store i32 1, i32* %10, align 4
  %36 = load i32, i32* %10, align 4
  %37 = atomicrmw add i32* @in_cs, i32 %36 monotonic
  store i32 %37, i32* %11, align 4
  %38 = load i32, i32* %11, align 4
  store i32 %38, i32* %9, align 4
  %39 = load i32, i32* %9, align 4
  %40 = icmp eq i32 %39, 0
  br i1 %40, label %41, label %42

41:                                               ; preds = %35
  br label %43

42:                                               ; preds = %35
  call void @__assert_fail(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([20 x i8], [20 x i8]* @.str.1, i64 0, i64 0), i32 23, i8* getelementptr inbounds ([25 x i8], [25 x i8]* @__PRETTY_FUNCTION__.thread_clh, i64 0, i64 0)) #4
  unreachable

43:                                               ; preds = %41
  store i32 1, i32* %12, align 4
  %44 = load i32, i32* %12, align 4
  %45 = atomicrmw sub i32* @in_cs, i32 %44 monotonic
  store i32 %45, i32* %13, align 4
  %46 = load i32, i32* %13, align 4
  br label %47

47:                                               ; preds = %43, %28
  br label %48

48:                                               ; preds = %47, %1
  %49 = load i32, i32* %3, align 4
  %50 = sext i32 %49 to i64
  %51 = getelementptr inbounds [3 x i32], [3 x i32]* @nodes_locked, i64 0, i64 %50
  store i32 0, i32* %14, align 4
  %52 = load i32, i32* %14, align 4
  store atomic i32 %52, i32* %51 release, align 4
  ret i8* null
}

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  store i32 0, i32* %2, align 4
  %6 = load i32, i32* %2, align 4
  store atomic i32 %6, i32* getelementptr inbounds ([3 x i32], [3 x i32]* @nodes_locked, i64 0, i64 1) monotonic, align 4
  store i32 0, i32* %3, align 4
  %7 = load i32, i32* %3, align 4
  store atomic i32 %7, i32* getelementptr inbounds ([3 x i32], [3 x i32]* @nodes_locked, i64 0, i64 2) monotonic, align 4
  %8 = call i32 @pthread_create(i64* %4, %union.pthread_attr_t* null, i8* (i8*)* @thread_clh, i8* inttoptr (i64 1 to i8*)) #5
  %9 = call i32 @pthread_create(i64* %5, %union.pthread_attr_t* null, i8* (i8*)* @thread_clh, i8* inttoptr (i64 2 to i8*)) #5
  %10 = load i64, i64* %4, align 8
  %11 = call i32 @pthread_join(i64 %10, i8** null)
  %12 = load i64, i64* %5, align 8
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
