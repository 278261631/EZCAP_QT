#include "downloadCapThread.h"
#include "include/dllqhyccd.h"
//#include "qhyccdStatus.h"
#include "myStruct.h"

#include <QDebug>
#include <QTime>

extern qhyccd_handle *camhandle;

DownloadCapThread::DownloadCapThread(QObject *parent) :
    QThread(parent)
{
}

void DownloadCapThread::run()
{
    unsigned int ret = QHYCCD_ERROR;

    QTime t0 = QTime::currentTime();
    t0.start();

    //获取图像数据
    ret = libqhyccd->GetQHYCCDSingleFrame(camhandle,&ix.imageX,&ix.imageY,&ix.imgBpp,&ix.imgChannels,ix.ImgData);
    //ret = GetQHYCCDSingleFrame(camhandle,&ix.imageX,&ix.imageY,&ix.imgBpp,&ix.imgChannels,ix.ImgData);
    if(ret != QHYCCD_SUCCESS)
    {
        qCritical("GetQHYCCDSingleFrame: failed");
        ix.imageReady = GetSingleFrame_Failed;
    }
    else
    {
        qDebug() << "GetQHYCCDSingleFrame: success, Time elapsed:" << t0.elapsed();
        if(ix.imgBpp == 8)
        {
            qDebug("GetQHYCCDSingleFrame the readout image depth is 8bit");
            unsigned char *temparray = (unsigned char *)malloc(ix.imageX * ix.imageY * 2);

            int i = 0,j = 1;
            for(;i < (int)(ix.imageX * ix.imageY);i++)
            {
                temparray[j] = ix.ImgData[i];
                j += 2;
            }

            memcpy(ix.ImgData,temparray,ix.imageX * ix.imageY * 2);
            delete(temparray);
        }

        memset(ix.lastImgData, 0, ix.ccdMaxImgW * ix.ccdMaxImgH * 2);
        memcpy(ix.lastImgData, ix.ImgData, ix.imageX * ix.imageY * 2);        
        ix.lastImageX = ix.imageX;
        ix.lastImageY = ix.imageY;

        ix.imageReady = GetSingleFrame_Success;
    }
}
