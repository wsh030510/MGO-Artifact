#!/bin/bash

# 检查是否提供了源文件参数
if [ -z "$1" ]; then
  echo "请提供源文件作为参数。"
  echo "用法: $0 <源文件相对路径> (例如: Layer1/example.c)"
  exit 1
fi

# 变量配置
SOURCE_FILE="$1"
BINARY=$(basename "$SOURCE_FILE" | cut -d. -f1)

# Docker 容器 ID（通过环境变量 IRHUNTER_CONTAINER_ID 设置，或手动修改此处）
DOCKER_CONTAINER_ID="${IRHUNTER_CONTAINER_ID:-}"

# 获取当前工作目录
CURRENT_DIR=$(pwd)
LOCAL_LOG_DIR="$CURRENT_DIR/log"
LOCAL_MODULE_DIR="$CURRENT_DIR/module"
CONFIGFILE="$CURRENT_DIR/config.properties"

# UFO 容器内固定路径
UFO_TEST_TRACE_DIR="/ufo/reorder/ufo_test_trace"
UFO_TL_BUF_SIZE=512

# 确保本地日志和模块的输出目录存在
mkdir -p "$LOCAL_LOG_DIR"
mkdir -p "$LOCAL_MODULE_DIR"

# 打印并执行命令，若执行失败则退出
execute_command() {
  echo "执行命令: $1"
  eval "$1"
  if [ $? -ne 0 ]; then
    echo "命令执行失败: $1"
    exit 1
  fi
}

echo "========================================"
echo "[1/4] 将本地测试文件同步至 Docker 容器..."
# 提取源文件的相对目录 (如 Layer1)，在容器内创建对应目录结构并同步文件
TARGET_DIR="/ufo/reorder/$(dirname "$SOURCE_FILE")"
execute_command "docker exec $DOCKER_CONTAINER_ID mkdir -p $TARGET_DIR"
execute_command "docker cp \"$SOURCE_FILE\" $DOCKER_CONTAINER_ID:\"/ufo/reorder/$SOURCE_FILE\""

echo "========================================"
echo "[2/4] 使用改版 ThreadSanitizer 编译源文件..."
# 注意：修复了原脚本中 -o0 的笔误，改为 -O0 (大写字母O)
compile_command="docker exec $DOCKER_CONTAINER_ID sh -c \"/ufo/build/bin/clang -fsanitize=thread -g -O0 -Wall /ufo/reorder/$SOURCE_FILE -o /ufo/reorder/$BINARY -lkeyutils -lpthread\"" 
execute_command "$compile_command"

echo "========================================"
echo "[3/4] 清理历史日志并执行插桩程序收集 Trace..."
clean_command="docker exec $DOCKER_CONTAINER_ID sh -c 'rm -rf /ufo/reorder/ufo_test_trace_*'"
execute_command "$clean_command"

sleep 2

# 设置UFO环境变量并运行二进制文件
docker exec $DOCKER_CONTAINER_ID sh -c "timeout 300 sh -c 'UFO_ON=1 UFO_CALL=1 UFO_TDIR=$UFO_TEST_TRACE_DIR UFO_TL_BUF=$UFO_TL_BUF_SIZE /ufo/reorder/$BINARY'"

# 循环直到UFO_TEST_TRACE_PATH不为空或超过20秒
UFO_TEST_TRACE_PATH=""
MAX_ATTEMPTS=20
attempt=0
while [ -z "$UFO_TEST_TRACE_PATH" ] && [ $attempt -lt $MAX_ATTEMPTS ]; do
  UFO_TEST_TRACE_PATH=$(docker exec $DOCKER_CONTAINER_ID sh -c "ls /ufo/reorder | grep '^ufo_test_trace_' | head -n 1")
  if [ -z "$UFO_TEST_TRACE_PATH" ]; then
    sleep 1  # 等待1秒钟再试
    attempt=$((attempt + 1))
  fi
done

if [ -z "$UFO_TEST_TRACE_PATH" ]; then
  echo "❌ 在20秒内未找到 UFO 测试跟踪路径，程序可能未成功运行或未触发并发事件。"
  exit 1
else
  echo "✅ 获取的 UFO 测试跟踪路径: $UFO_TEST_TRACE_PATH"
fi

echo "========================================"
echo "[4/4] 将分析结果提取至本地宿主机..."

# 清理并拷贝 Trace 日志
clean_log_command="rm -rf $LOCAL_LOG_DIR/$BINARY"
execute_command "$clean_log_command"
copy_log_command="docker cp $DOCKER_CONTAINER_ID:/ufo/reorder/$UFO_TEST_TRACE_PATH $LOCAL_LOG_DIR/$BINARY"
execute_command "$copy_log_command"

# 清理并拷贝编译好的二进制模块
clean_binary_command="rm -rf $LOCAL_MODULE_DIR/$BINARY"
execute_command "$clean_binary_command"
copy_binary_command="docker cp $DOCKER_CONTAINER_ID:/ufo/reorder/$BINARY $LOCAL_MODULE_DIR/"
execute_command "$copy_binary_command"

# 更新本地的 config.properties 文件指向最新的 trace 目录
if [ -f "$CONFIGFILE" ]; then
    sed_command="sed -i 's|^trace_dir=.*|trace_dir=$LOCAL_LOG_DIR/$BINARY|' $CONFIGFILE"
    execute_command "$sed_command"
    sed_command_app="sed -i 's|^app_name=.*|app_name=$BINARY|' $CONFIGFILE"
    execute_command "$sed_command_app"
else
    echo "⚠️ 未找到 $CONFIGFILE，请手动确保预测工具的配置文件路径正确。"
fi

echo "========================================"
echo "🎉 动态插桩与运行阶段完成！日志已保存至: $LOCAL_LOG_DIR/$BINARY"