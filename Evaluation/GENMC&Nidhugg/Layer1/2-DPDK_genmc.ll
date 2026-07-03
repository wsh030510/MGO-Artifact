; ModuleID = 'test/2-DPDK_genmc.c'
source_filename = "test/2-DPDK_genmc.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.mcs_node = type { %struct.mcs_node*, i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@tail = dso_local global %struct.mcs_node* null, align 8
@node1 = dso_local global %struct.mcs_node zeroinitializer, align 8
@node2 = dso_local global %struct.mcs_node zeroinitializer, align 8

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread_1(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca %struct.mcs_node*, align 8
  %4 = alloca i32, align 4
  %5 = alloca %struct.mcs_node*, align 8
  %6 = alloca %struct.mcs_node*, align 8
  %7 = alloca %struct.mcs_node*, align 8
  %8 = alloca %struct.mcs_node*, align 8
  store i8* %0, i8** %2, align 8
  store %struct.mcs_node* null, %struct.mcs_node** %3, align 8
  %9 = bitcast %struct.mcs_node** %3 to i64*
  %10 = load i64, i64* %9, align 8
  store atomic i64 %10, i64* bitcast (%struct.mcs_node* @node1 to i64*) monotonic, align 8
  store i32 1, i32* %4, align 4
  %11 = load i32, i32* %4, align 4
  store atomic i32 %11, i32* getelementptr inbounds (%struct.mcs_node, %struct.mcs_node* @node1, i32 0, i32 1) monotonic, align 8
  store %struct.mcs_node* @node1, %struct.mcs_node** %6, align 8
  %12 = bitcast %struct.mcs_node** %6 to i64*
  %13 = bitcast %struct.mcs_node** %7 to i64*
  %14 = load i64, i64* %12, align 8
  %15 = atomicrmw xchg i64* bitcast (%struct.mcs_node** @tail to i64*), i64 %14 monotonic
  store i64 %15, i64* %13, align 8
  %16 = bitcast i64* %13 to %struct.mcs_node**
  %17 = load %struct.mcs_node*, %struct.mcs_node** %16, align 8
  store %struct.mcs_node* %17, %struct.mcs_node** %5, align 8
  %18 = load %struct.mcs_node*, %struct.mcs_node** %5, align 8
  %19 = icmp ne %struct.mcs_node* %18, null
  br i1 %19, label %20, label %26

20:                                               ; preds = %1
  %21 = load %struct.mcs_node*, %struct.mcs_node** %5, align 8
  %22 = getelementptr inbounds %struct.mcs_node, %struct.mcs_node* %21, i32 0, i32 0
  store %struct.mcs_node* @node1, %struct.mcs_node** %8, align 8
  %23 = bitcast %struct.mcs_node** %22 to i64*
  %24 = bitcast %struct.mcs_node** %8 to i64*
  %25 = load i64, i64* %24, align 8
  store atomic i64 %25, i64* %23 monotonic, align 8
  br label %26

26:                                               ; preds = %20, %1
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread_2(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca %struct.mcs_node*, align 8
  %4 = alloca i32, align 4
  %5 = alloca %struct.mcs_node*, align 8
  %6 = alloca %struct.mcs_node*, align 8
  %7 = alloca %struct.mcs_node*, align 8
  %8 = alloca %struct.mcs_node*, align 8
  %9 = alloca %struct.mcs_node*, align 8
  %10 = alloca %struct.mcs_node*, align 8
  store i8* %0, i8** %2, align 8
  store %struct.mcs_node* null, %struct.mcs_node** %3, align 8
  %11 = bitcast %struct.mcs_node** %3 to i64*
  %12 = load i64, i64* %11, align 8
  store atomic i64 %12, i64* bitcast (%struct.mcs_node* @node2 to i64*) monotonic, align 8
  store i32 1, i32* %4, align 4
  %13 = load i32, i32* %4, align 4
  store atomic i32 %13, i32* getelementptr inbounds (%struct.mcs_node, %struct.mcs_node* @node2, i32 0, i32 1) monotonic, align 8
  store %struct.mcs_node* @node2, %struct.mcs_node** %6, align 8
  %14 = bitcast %struct.mcs_node** %6 to i64*
  %15 = bitcast %struct.mcs_node** %7 to i64*
  %16 = load i64, i64* %14, align 8
  %17 = atomicrmw xchg i64* bitcast (%struct.mcs_node** @tail to i64*), i64 %16 monotonic
  store i64 %17, i64* %15, align 8
  %18 = bitcast i64* %15 to %struct.mcs_node**
  %19 = load %struct.mcs_node*, %struct.mcs_node** %18, align 8
  store %struct.mcs_node* %19, %struct.mcs_node** %5, align 8
  %20 = load %struct.mcs_node*, %struct.mcs_node** %5, align 8
  %21 = icmp ne %struct.mcs_node* %20, null
  br i1 %21, label %22, label %32

22:                                               ; preds = %1
  %23 = load %struct.mcs_node*, %struct.mcs_node** %5, align 8
  %24 = getelementptr inbounds %struct.mcs_node, %struct.mcs_node* %23, i32 0, i32 0
  store %struct.mcs_node* @node2, %struct.mcs_node** %8, align 8
  %25 = bitcast %struct.mcs_node** %24 to i64*
  %26 = bitcast %struct.mcs_node** %8 to i64*
  %27 = load i64, i64* %26, align 8
  store atomic i64 %27, i64* %25 monotonic, align 8
  %28 = bitcast %struct.mcs_node** %10 to i64*
  %29 = load atomic i64, i64* bitcast (%struct.mcs_node* @node1 to i64*) monotonic, align 8
  store i64 %29, i64* %28, align 8
  %30 = bitcast i64* %28 to %struct.mcs_node**
  %31 = load %struct.mcs_node*, %struct.mcs_node** %30, align 8
  store %struct.mcs_node* %31, %struct.mcs_node** %9, align 8
  br label %32

32:                                               ; preds = %22, %1
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %4 = call i32 @pthread_create(i64* %2, %union.pthread_attr_t* null, i8* (i8*)* @thread_1, i8* null) #3
  %5 = call i32 @pthread_create(i64* %3, %union.pthread_attr_t* null, i8* (i8*)* @thread_2, i8* null) #3
  %6 = load i64, i64* %2, align 8
  %7 = call i32 @pthread_join(i64 %6, i8** null)
  %8 = load i64, i64* %3, align 8
  %9 = call i32 @pthread_join(i64 %8, i8** null)
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #1

declare dso_local i32 @pthread_join(i64, i8**) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Debian clang version 11.0.1-2"}
