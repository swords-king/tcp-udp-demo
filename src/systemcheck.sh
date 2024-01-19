#!/usr/bin/expect

# 指定文件名
filename="hosts"

# 检查文件是否存在
if [ ! -f "$filename" ]; then
  echo "文件 $filename 不存在."
  exit 1
fi

# 设置服务器用户名和密码
set userName "root"
set password "kylinV10"

# 逐行读取文件内容，并保存为IP地址。然后使用上述用户名和密码进行ssh登录操作
foreach line [split [read -r $filename] "\n"] {
    spawn ssh $userName@$line
    expect {
        "yes/no" { send "yes\r"; exp_continue }
        "password:" { send "$password\r"; exp_continue }
        "*#" { send "exit\r"; exp_continue }
        "*>" { send "exit\r"; exp_continue }

        timeout { send "exit\r"; exp_continue }
        eof { exit }
        }
        expect eof
        close