#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QPainter>
#include <QPlainTextEdit>
#include <QDebug>


#include "plot.h"

#include <libs/windows/include/zmq.h>
#include "flatbuffers/flatbuffers.h"
#include "channel_generated.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define LOGC(strInfo) qDebug()<<"C:"<<strInfo<<Qt::endl
#define LOGD(strInfo) qDebug()<<"D:"<<strInfo<<Qt::endl
#define LOGE(strInfo) qDebug()<<"E:"<<strInfo<<Qt::endl
#define jassert assert

#define MAX_NUM_CHANNELS 100
#define MAX_NUM_SAMPLES  10000

enum ParseState
{
    S_FIND_HEADER = 0,
    S_CHECK_COUNTER,
    S_CHECK_DATATYPE,
    S_CHECK_DATASAMPLEBYTE,
    S_CHECK_DATACHANNEL,
    S_PROCESS_DATA,
    S_CHECK_TAIL
};

enum ProtocolOffset
{
    OFFSET_HEADER1 = 0,
    OFFSET_HEADER2,
    OFFSET_COUNTER,
    OFFSET_DATATYPE,
    OFFSET_DATASAMPLEBYTE,
    OFFSET_DATACHANNEL,
    OFFSET_DATA
};

struct DataProtocol
{
    const uint8_t header1 = 0x5A;
    const uint8_t header2 = 0x5A;
    const uint8_t tail1 = 0xA5;
    const uint8_t tail2 = 0xA5;
    uint8_t counter;
    uint8_t dataType;
    uint8_t dataSampleByte;
    uint8_t dataChannel;
};

struct ZmqData
{
    uint32_t numChannels=0;
    uint32_t numSamples=0;
    uint64_t sampleNum=0;
    double timestamp=0;
    uint32_t sampleRate=0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonOpenPort_clicked();           //打开串口
    void on_pushButtonClearRec_clicked();           //清除接收
    void on_pushButtonClearSend_clicked();          //清除发送
    void slot_timerUpdateLabel_timeout();           //统计数据更新定时器槽函数

    void on_checkBoxHexRec_stateChanged(int arg1);      //16进制接收
    void on_checkBoxHexSend_stateChanged(int arg1);     //16进制发送

    void on_pushButtonSingleSend_clicked();                 //tab1 单次发送按钮槽函数
    void on_checkBoxSingleSend_stateChanged(int arg1);      //tab1 定时发送选框槽函数(开启单发定时器)
    void slot_pushButtonMultiSend_n_clicked();              //tab2 数字标号单次发送按钮槽函数
    void on_checkBoxMultiSend_stateChanged(int arg1);       //tab2 定时发送选框槽函数(开启多发定时器)
    void slot_timerMultiSend_timeout();                     //tab2 多发定时器槽函数

    void slot_serialPort_readyRead();                       //串口接收槽函数
    void on_checkBoxFrameData_stateChanged(int arg1);       //tab3 显示Frame数据
    void on_checkBoxPlotData_stateChanged(int arg1);        //tab3 显示Plot数据
    void on_pushButtonClearFramePlotData_clicked();         //清除Frame和Plot窗口内容

    void slot_timerWaveGene_timeout();                      //tab4 仿真波形定时器
    void on_checkBoxWaveGeneStart_stateChanged(int arg1);   //tab4 仿真波形开始

    void on_actionPlotShow_triggered();     //显示Plot窗口

    void on_checkBoxUseQCustomPlot_stateChanged(int arg1);

    void on_checkBoxUseOpenEphysGUI_stateChanged(int arg1);

    void on_lineEditZmqTcpPort_textChanged(const QString &arg1);

private:
    void changeEncodeStrAndHex(QPlainTextEdit *plainTextEdit,int arg1); //切换字符编码与16进制
    void processRecvProtocol(QByteArray *baRecvData);                   //对串口接收数据进行协议处理

private:
    Ui::MainWindow *ui;

    //-----------------------
    QSerialPort *serialPort;
    //发送接收数量，速率计算
    long curSendNum=0;
    long curRecvNum=0;
    long lastSendNum=0;
    long lastRecvNum=0;
    long sendRate=0;
    long recvRate=0;
    //接收帧数量、速率、CRC错误
    long curRecvFrameNum=0;
    long lastRecvFrameNum=0;
    long recvFrameRate=0;
    long recvFrameCrcErrNum=0;
    long recvFrameMissNum=0;

    //-----------------------
    // 定时发送-定时器
    QTimer *timerSingleSend;    //定时调用on_pushButtonSingleSend_clicked()
    QTimer *timerMultiSend;     //定时轮训发送选中的数据
    // Label数据更新-定时器
    QTimer *timerUpdateLabel;
    // 仿真波形生成-定时器
    QTimer *timerWaveGene;

    //-----------------------
    QByteArray baRecvDataBuf;   //接收数据流暂存
    bool needPushDataFront = false;
    QByteArray baRecvDataBufTmp;

    ParseState STATE=S_FIND_HEADER;     //接受数据处理状态机
    uint64_t dataSizeForQCustomPlot=0;
    float *samplesForQCustomPlot;
    uint64_t dataSizeForOpenEphysGUI=0;
    float *samplesForOpenEphysGUI;
    uint16_t lastEventCode=0;
    std::vector<uint16_t> eventCodesForOpenEphysGUI;
    struct DataProtocol dataProtocol;

    //------------------------
    void *context;
    void *socket;
    uint32_t port=3335;
    int messageNumber=0;
    struct ZmqData zmqData;
    void zmqSendData(const float *bufferChanPtrs,
                                uint32_t nChannels, uint32_t nSamples,
                                uint64_t sampleNumber, double timestamp, uint32_t sampleRate);
    void createSocket();
    void closeSocket();
    flatbuffers::FlatBufferBuilder flatBuilder;


    bool showFramData=false;    //是否显示帧数据
    bool showPlotData=false;    //是否显示有效数据
    bool useQCustomPlot=false;
    bool useOpenEphysGUI=false;
    //-----------------------
    // 波形绘图窗口
    Plot *plot = NULL;// 必须初始化为空，否则后面NEW判断的时候会异常结束

    //-----------------------
    //CSV file to save data
    QFile *m_csvFile = nullptr;
    QTextStream *m_csvFileTextStream = nullptr;
    void openCsvFile(void);
    void closeCsvFile(void);
    void saveCsvFile(QByteArray baRecvData);
};
#endif // MAINWINDOW_H
