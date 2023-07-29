/**

@file VersionDownloader.h
@brief 用于下载和安装应用程序版本的类
该类提供了下载和安装应用程序版本的功能。它使用网络访问管理器来下载文件，并提供了解压缩文件、安装应用程序和删除文件夹等辅助方法。
@author zhoulin
*/
#ifndef VERSIONDOWNLOADER_H
#define VERSIONDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QString>

#include "VersionDownloader_global.h"

class VERSIONDOWNLOADER_EXPORT VersionDownloader : public QObject
{
public:
    explicit VersionDownloader(QObject *parent = nullptr);

public:
    /**
     * @brief 开始下载操作
     * @param url 下载地址
     * @param savePath 保存路径
     * @param app 下载程序的路径
     */
    QString Download(QString url, QString savePath, QString app);

    /**
     * @brief 解压文件
     * @param zipFilePath 被解压的文件路径
     * @param destinationDir 解压后存放的路径
     */
    bool unzipFile(const QString& zipFilePath, const QString& destinationDir);

    /**
     * @brief 非静默安装
     * @param app 安装程序的路径
     */
    void NonSilentInstallation(const QString& installerPath);

    /**
     * @brief 安装软件
     * @param 安装程序的路径
     */
    void SilentInstallation(const QString& installerPath);

    /**
     * @brief 删除文件夹
     * @param 文件夹路径
     */
    bool deleteFolder(const QString& folderPath);
private:
    /**
     * @brief URL检查结果
     * @param exists URL是否存在的标志
     * @param url 原始的URL
     */
    void urlCheckResult(bool exists, const QString& url);

    /**
     * @brief 接收到版本号
     * @param version 接收到的版本号
     */
    void versionReceived(const QString& version);

    /**
     * @brief 接收到下载链接
     * @param downloadLink 接收到的下载链接
     */
    void downloadLinkReceived(const QString& downloadLink);

public slots:
    /**
     * @brief 检查给定的URL是否存在
     * @param url 要检查的URL
     */
    void checkUrl(const QString& url);

private slots:
    /**
     * @brief 处理URL响应
     * @param reply URL的响应对象
     */
    void processUrlResponse(QNetworkReply *reply);

    /**
     * @brief 处理版本号和下载链接
     * @param response URL返回的响应内容
     */
    void processVersionAndDownloadLink(const QString& response);

private:
    QNetworkAccessManager *manager;

public:
    /**
     * @brief version 版本号
     */
    QString version;

    /**
     * @brief downloadLink 下载链接
     */
    QString downloadLink;
};

#endif // VERSIONDOWNLOADER_H
