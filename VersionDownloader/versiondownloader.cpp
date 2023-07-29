#include "versiondownloader.h"

#include <QProcess>

#include <QFile>
#include <QDir>

#include "QtGui/private/qzipreader_p.h"
#include "QtGui/private/qzipwriter_p.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QEventLoop>

VersionDownloader::VersionDownloader(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &VersionDownloader::processUrlResponse);
}

/**
 * @brief 开始下载操作
 * @param url 下载地址
 * @param savePath 保存路径
 * @param app 下载程序的路径
 */
QString VersionDownloader::Download(QString url, QString savePath, QString app)
{
    QFile file(app);
    if (file.exists()) {
        QProcess *process = new QProcess();
        // 启动下载程序并传递参数进行下载
        process->start(app, QStringList() << url << savePath);
        process->waitForFinished(-1);
        QByteArray output = process->readAllStandardOutput();
        QString outputString = QString::fromUtf8(output);

        return outputString;
    } else {
        return "缺少下载程序";
    }
}

/**
 * @brief 检查给定的URL是否存在
 * @param url 要检查的URL
 */
void VersionDownloader::checkUrl(const QString& url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // 创建事件循环对象
    QEventLoop loop;

    // 将事件循环与 reply 的 finished() 信号关联
    QNetworkReply* reply = manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    // 阻塞线程，等待 reply 完成响应
    loop.exec();

    // 处理响应
    processUrlResponse(reply);
}

/**
 * @brief 处理URL响应
 * @param reply URL的响应对象
 */
void VersionDownloader::processUrlResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8());
        if (!jsonDoc.isNull() && jsonDoc.isObject()) {
            QJsonObject jsonObject = jsonDoc.object();
            if (jsonObject.contains("id")) {
                // URL 存在，发射 urlCheckResult 信号并继续处理版本号和下载链接
                emit urlCheckResult(true, response);
                processVersionAndDownloadLink(response);
                return;
            }
        }
    }
    // URL 不存在，发射 urlCheckResult 信号
    emit urlCheckResult(false, "");
}

/**
 * @brief 处理版本号和下载链接
 * @param response URL返回的响应内容
 */
void VersionDownloader::processVersionAndDownloadLink(const QString& response)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8());
    if (!jsonDoc.isNull() && jsonDoc.isObject()) {
        QJsonObject jsonObject = jsonDoc.object();
        if (jsonObject.contains("tag_name")) {
            // 获取版本号并发射 versionReceived 信号
            version = jsonObject.value("tag_name").toString();
            emit versionReceived(version);
        }
        if (jsonObject.contains("assets")) {
            QJsonArray assetsArray = jsonObject.value("assets").toArray();
            if (!assetsArray.isEmpty()) {
                QJsonObject firstAsset = assetsArray.at(0).toObject();
                if (firstAsset.contains("browser_download_url")) {
                    // 获取下载链接并发射 downloadLinkReceived 信号
                    downloadLink = firstAsset.value("browser_download_url").toString();
                    emit downloadLinkReceived(downloadLink);
                }
            }
        }
    }
}

/**
 * @brief URL检查结果
 * @param exists URL是否存在的标志
 * @param url 原始的URL
 */
void VersionDownloader::urlCheckResult(bool exists, const QString& url)
{
    if (exists) {
        // URL 存在，继续处理版本号和下载链接
        processVersionAndDownloadLink(url);
    }
}

/**
 * @brief 接收到版本号
 * @param version 接收到的版本号
 */
void VersionDownloader::versionReceived(const QString& version)
{
    // 将版本号存储到类成员变量中
    this->version = version;
}

/**
 * @brief 接收到下载链接
 * @param downloadLink 接收到的下载链接
 */
void VersionDownloader::downloadLinkReceived(const QString& downloadLink)
{
    // 将下载链接存储到类成员变量中
    this->downloadLink = downloadLink;
}


/**
 * @brief 解压文件
 * @param zipFilePath 被解压的文件路径
 * @param destinationDir 解压后存放的路径
 */
bool VersionDownloader::unzipFile(const QString& zipFilePath, const QString& destinationDir)
{
    QDir tempDir;
    if(!tempDir.exists(destinationDir)) tempDir.mkpath(destinationDir);
    QZipReader reader(zipFilePath);
    return reader.extractAll(destinationDir);
}

/**
 * @brief 非静默安装
 * @param app 安装程序的路径
 */
void VersionDownloader::NonSilentInstallation(const QString& installerPath)
{
    QFile file(installerPath);
    if (file.exists()) {
        QProcess installerProcess;
        QStringList arguments;

        // 创建事件循环对象
        QEventLoop loop;

        // 将事件循环与 reply 的 finished() 信号关联
        QObject::connect(&installerProcess,
                         &QProcess::stateChanged,
                         &loop, &QEventLoop::quit);

        //启动程序
        installerProcess.start(installerPath, arguments);

        // 阻塞线程，等待 reply 完成响应
        loop.exec();
    }
}

/**
 * @brief 静默安装软件
 * @param 安装程序的路径
 */
void VersionDownloader::SilentInstallation(const QString& installerPath)
{
    QFile file(installerPath);
    if (file.exists()) {
        QProcess installerProcess;
        QStringList arguments;
        arguments << "/SILENT" << "/SP-" << "/SUPPRESSMSGBOXES";

        // 创建事件循环对象
        QEventLoop loop;

        // 将事件循环与 reply 的 finished() 信号关联
        QObject::connect(&installerProcess,
                         &QProcess::stateChanged,
                         &loop, &QEventLoop::quit);


        installerProcess.start(installerPath, arguments);


        // 阻塞线程，等待 reply 完成响应
        loop.exec();
    }
}

/**
 * @brief 删除文件夹
 * @param 文件夹路径
 */
bool VersionDownloader::deleteFolder(const QString& folderPath)
{
    if (folderPath.isEmpty() || !QDir().exists(folderPath))//是否传入了空的路径||路径是否存在
        return false;

    QFileInfo FileInfo(folderPath);

    if (FileInfo.isFile())//如果是文件
        QFile::remove(folderPath);
    else if (FileInfo.isDir())//如果是文件夹
    {
        QDir qDir(folderPath);
        qDir.removeRecursively();
    }
    return true;
}
