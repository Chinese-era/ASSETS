import sys
import os
import requests
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QProgressBar, QDesktopWidget
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import Qt, QThread, pyqtSignal

class DownloadThread(QThread):
    update_progress = pyqtSignal(int)

    def __init__(self, url, save_path):
        super().__init__()
        self.url = url
        self.save_path = save_path

    def run(self):
        try:
            response = requests.get(self.url, stream=True, timeout=5)  # Set the timeout to 10 seconds
            response.raise_for_status()  # Check for any HTTP errors

            total_size = int(response.headers.get('content-length', 0))

            with open(self.save_path, 'wb') as f:
                downloaded_size = 0
                for data in response.iter_content(chunk_size=1024):
                    downloaded_size += len(data)
                    f.write(data)
                    progress = int((downloaded_size / total_size) * 100)
                    self.update_progress.emit(progress)

        except requests.exceptions.RequestException as e:
            if os.path.exists(self.save_path):
                os.remove(self.save_path)
            sys.stdout.buffer.write(f"Error: {str(e)}".encode('utf-8'))
            sys.exit()

        except Exception as e:
            if os.path.exists(self.save_path):
                os.remove(self.save_path)
            sys.stdout.buffer.write(f"Error: {str(e)}".encode('utf-8'))
            sys.exit()


class BrowserDownloader(QWidget):
    def __init__(self):
        super().__init__()
        self.init_ui()

    def init_ui(self):
        self.setWindowTitle('下载')
        self.setWindowIcon(QIcon('download.ico'))  # 设置图标
        self.resize(400, 100)
        self.center_on_screen()

        self.progress_bar = QProgressBar()
        self.progress_bar.setFixedHeight(35)  # Increase the height of the progress bar
        self.progress_bar.setAlignment(Qt.AlignRight | Qt.AlignVCenter)

        layout = QVBoxLayout()
        layout.addWidget(self.progress_bar)

        self.setLayout(layout)

    def center_on_screen(self):
        screen_geometry = QDesktopWidget().screenGeometry()
        x = (screen_geometry.width() - self.width()) // 2
        y = (screen_geometry.height() - self.height()) // 2
        self.move(x, y)

    def start_download(self, url, filename, download_path):
        save_path = f'{download_path}/{filename}'
        self.download_thread = DownloadThread(url, save_path)
        self.download_thread.update_progress.connect(self.update_progress_bar)
        self.download_thread.finished.connect(self.download_finished)
        self.download_thread.start()

    def update_progress_bar(self, progress):
        self.progress_bar.setValue(progress)

    def download_finished(self):
        self.progress_bar.setValue(100)
        sys.stdout.buffer.write("Successful".encode('utf-8'))
        sys.exit()

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("请输入下载链接和下载路径！")
        sys.exit(1)

    url = sys.argv[1]
    filename = url.split("/")[-1]
    download_path = sys.argv[2]

    app = QApplication(sys.argv)

    window = BrowserDownloader()
    window.show()
    window.start_download(url, filename, download_path)


    sys.exit(app.exec_())

