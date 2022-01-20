# 文件水印应用场景详细描述

文件隐形水印xxx主要解决跨主机文件流转追踪，富文本文件在安装了**xxx**系统内流转时。用户可以**xxx**实时在系统内查看文件的来源及流转信息。当文件外泄也可通过嵌入文件内的水印进行溯源。


## 问题

### 不同主机存在相同文档如何匹配并在系统内快速聚合？

通过解析文档获取的文本信息算出hash, 或通过近似度匹配hash算法simhash匹配相似度在一定范围内的文档。

### 文件以什么进行标记和追踪？

文件存在一个唯一标识GUID，意为当前主机当前文件的唯一标识（相同文件在不同主机的GUID不一致），此ID不能做为聚合使用。只能用作文件流转追踪，每次文件操作都会有包含GUID的日志信息上传至服务端。
当文件被通过拷贝创建新文件时，会产生新的GUID2，文件流转追踪实际关联的数据也是以GUID2为基础。此时我们在水印信息中应存储的信息为GUID2，当文件实际发生流转时。读取水印信息并上传日志，即可在关联文件跨主机流转信息。

### 二维码存储信息最多1500字节, 如果记录信息

```

#pragram pack(1)

typedef struct  {
    char Guid[32];          // 文件GUID
    char SID[10];           // 探针ID
    char Domain[64];        // 域
    char UserName[64];      // 用户名
    char Timestamp[10];     // 时间戳
    char PKIHash[64];       // PKI签名
} WATERMARKER_INFOMATION, *PWATERMARKER_INFORMATION;

typedef struct{
    int magic;          // 0xbeefbeef
    int crc;            // crc value
    int length;         // signature length
    char watermark_info;
}
```


实际存储结构体大约120字节，理论最多可存储10级流转信息。如果采取zip压缩的话预计能存储3000字节以上，但是目前DEMO版本并未采用，后续会实现。

目前采取如果生成的水印信息超过1500字节，则删除水印信息中时间戳最古老的水印信息直到满足水印信息小于1500字节的条件。


### 水印信息是否应该包含用户名信息

水印信息是否应该加入域/用户信息？ 必须加。

## 调用方式

调用方式目前约定为DLL方式进行调用, 提供pdf和docx文件格式的文件水印接口和编译好的DLL文件. 目前不考虑维护.复杂度和内存泄露的情况下, 尽快实现功能。

### 读取水印

int DocxMarkReader(char *FilePath, char*TempPath, char &Signature, int &length, int &State);

### 写入水印

int DocxMarkWriter(char *FilePath, char*TempPath, char *Signature, int length, int &State);

## 目前库存在的问题

### DOCX

### PDF


