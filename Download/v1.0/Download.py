#优化代码结构，并详细表示优化后的每行代码的作用
import sys
import tkinter as tk
from tkinter import ttk
import threading
import requests
import os
import time

import threading

if len(sys.argv) < 3:
    print("请输入下载链接和下载路径！")
    sys.exit(1)

# url = 'https://gitee.com/ChineseRabbit/first-project/releases/download/v1.0/mysetupV1.0.exe'

url = sys.argv[1]
filename = url.split("/")[-1]
download_path = sys.argv[2]
# download_path = 'D:\Download\FDM'

class DownloadApp:
    def __init__(self, root):
        self.root = root
        self.root.title("文件下载")
        self.progress = tk.DoubleVar()
        self.cancelled = False

        # 添加显示进度的Label
        self.progress_label = tk.Label(self.root, text="0%", font=("Arial", 12))
        self.progress_label.pack(pady=5)

        self.create_widgets()

    def create_widgets(self):
        self.progress_bar = ttk.Progressbar(self.root, variable=self.progress, maximum=100)
        self.progress_bar.pack(pady=20)
        self.cancel_button = tk.Button(self.root, text="取消下载", command=self.cleanup)
        self.cancel_button.pack(pady=10)

    def start_download(self):
        threading.Thread(target=self.download_file).start()

    def download_file(self):
        try:
            response = requests.get(url, stream=True)
            response.raise_for_status()
            total_size = int(response.headers.get("content-length", 0))
            block_size = 1024
            downloaded_size = 0
            with open(os.path.join(download_path, filename), "wb") as f:
                for data in response.iter_content(block_size):
                    if self.cancelled:
                        break
                    f.write(data)
                    downloaded_size += len(data)
                    self.progress.set(downloaded_size * 100 / total_size)
                    self.progress_label.config(text=f"{self.progress.get():.2f}%")
                    self.root.update()
                else:
                    # 下载完成后发送消息给Qt Creator
                    # 设置标准输出的编码为UTF-8，避免在Windows控制台中输出乱码
                    sys.stdout.buffer.write("Successful".encode('utf-8'))
                    # 下载完成后关闭应用程序
                    self.root.after(1000, self.root.destroy)  # 延迟1秒后关闭窗口
        except Exception as e:
            # 设置标准输出的编码为UTF-8，避免在Windows控制台中输出乱码
            er = "error:" + e
            sys.stdout.buffer.write(er.encode('utf-8'))

        finally:
            # 下载完成或取消下载后释放资源
            response.close()

    def cleanup(self):

        self.cancelled = True



        filepath = os.path.join(download_path, filename)
        if os.path.exists(filepath):
            # 启动一个新线程来执行文件删除操作
            delete_thread = threading.Thread(target=self.delete_file, args=(filepath,))
            delete_thread.start()



    def delete_file(self, filepath):
        max_attempts = 5  # 最大尝试次数
        attempt = 0
        while attempt < max_attempts:
            try:
                os.remove(filepath)
                break  # 文件删除成功，退出循环
            except PermissionError:
                attempt += 1
                time.sleep(1)  # 等待1秒后再尝试删除文件
        else:
            sys.stdout.buffer.write("Failed to delete file".encode('utf-8'))

        sys.stdout.buffer.write("The application has quit".encode('utf-8'))
        self.root.after(1000, self.root.destroy)  # 延迟1秒后关闭窗口

if __name__ == "__main__":
    root = tk.Tk()
    root.resizable(0, 0)
    root.geometry("150x150")

    app = DownloadApp(root)

    root.protocol("WM_DELETE_WINDOW", app.cleanup)
    app.start_download()
    root.mainloop()