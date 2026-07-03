; ModuleID = 'test/5-StackOverFlowEasy_genmc.c'
source_filename = "test/5-StackOverFlowEasy_genmc.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.Point = type { i32, i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@currentPos = dso_local global %struct.Point* null, align 8
@shared_p = dso_local global %struct.Point zeroinitializer, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @writer_thread(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca %struct.Point*, align 8
  store i8* %0, i8** %2, align 8
  store i32 1, i32* getelementptr inbounds (%struct.Point, %struct.Point* @shared_p, i32 0, i32 0), align 4
  store i32 2, i32* getelementptr inbounds (%struct.Point, %struct.Point* @shared_p, i32 0, i32 1), align 4
  store %struct.Point* @shared_p, %struct.Point** %3, align 8
  %4 = bitcast %struct.Point** %3 to i64*
  %5 = load i64, i64* %4, align 8
  store atomic i64 %5, i64* bitcast (%struct.Point** @currentPos to i64*) monotonic, align 8
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @reader_thread(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca %struct.Point*, align 8
  %4 = alloca %struct.Point*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i8* %0, i8** %2, align 8
  %7 = bitcast %struct.Point** %4 to i64*
  %8 = load atomic i64, i64* bitcast (%struct.Point** @currentPos to i64*) monotonic, align 8
  store i64 %8, i64* %7, align 8
  %9 = bitcast i64* %7 to %struct.Point**
  %10 = load %struct.Point*, %struct.Point** %9, align 8
  store %struct.Point* %10, %struct.Point** %3, align 8
  %11 = load %struct.Point*, %struct.Point** %3, align 8
  %12 = icmp ne %struct.Point* %11, null
  br i1 %12, label %13, label %20

13:                                               ; preds = %1
  %14 = load %struct.Point*, %struct.Point** %3, align 8
  %15 = getelementptr inbounds %struct.Point, %struct.Point* %14, i32 0, i32 0
  %16 = load i32, i32* %15, align 4
  store i32 %16, i32* %5, align 4
  %17 = load %struct.Point*, %struct.Point** %3, align 8
  %18 = getelementptr inbounds %struct.Point, %struct.Point* %17, i32 0, i32 1
  %19 = load i32, i32* %18, align 4
  store i32 %19, i32* %6, align 4
  br label %20

20:                                               ; preds = %13, %1
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %4 = call i32 @pthread_create(i64* %2, %union.pthread_attr_t* null, i8* (i8*)* @writer_thread, i8* null) #3
  %5 = call i32 @pthread_create(i64* %3, %union.pthread_attr_t* null, i8* (i8*)* @reader_thread, i8* null) #3
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
