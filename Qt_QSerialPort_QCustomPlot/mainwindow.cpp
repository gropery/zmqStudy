#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , flatBuilder(1024)
{
    ui->setupUi(this);
    setWindowTitle("Qt Serial Plot");

    connect(ui->pushButtonMultiSend_1,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_2,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_3,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_4,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_5,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_6,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_7,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_8,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_9,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));
    connect(ui->pushButtonMultiSend_10,SIGNAL(clicked(bool)),this,SLOT(slot_pushButtonMultiSend_n_clicked()));

    // 单次发送选项卡-定时发送-定时器
    timerSingleSend = new QTimer;
    timerSingleSend->setInterval(1000);// 设置默认定时时长1000ms
    connect(timerSingleSend, &QTimer::timeout, this, [=](){
        on_pushButtonSingleSend_clicked();
    });

    // 多次发送选项卡-定时发送-定时器
    timerMultiSend = new QTimer;
    timerMultiSend->setInterval(1000);// 设置默认定时时长1000ms
    connect(timerMultiSend, &QTimer::timeout, this, [=](){
        slot_timerMultiSend_timeout();
    });

    // Label数据更新-定时器
    timerUpdateLabel = new QTimer;
    timerUpdateLabel->start(1000);
    connect(timerUpdateLabel, &QTimer::timeout, this, [=](){
        slot_timerUpdateLabel_timeout();
    });

    // 模拟波形串口输出定时器
    timerWaveGene = new QTimer;
    timerWaveGene->setInterval(20);
    connect(timerWaveGene, &QTimer::timeout, this, [=](){
        slot_timerWaveGene_timeout();
    });

    // 新建串口对象
    serialPort = new QSerialPort(this);
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(slot_serialPort_readyRead()));

    // 新建波形显示界面
    plot = new Plot;
    plot->show();

    // 为绘图程序申请数据暂存缓存
    samplesForQCustomPlot = (float*) malloc(MAX_NUM_CHANNELS*MAX_NUM_SAMPLES);
    samplesForOpenEphysGUI = (float*) malloc(MAX_NUM_CHANNELS*MAX_NUM_SAMPLES);


    // ZMQ 初始化
    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    qDebug()<<"ZMQ VERSION:"<< major<<minor<<patch;
    context = zmq_ctx_new();

    useQCustomPlot = ui->checkBoxUseQCustomPlot->isChecked();
    useOpenEphysGUI = ui->checkBoxUseOpenEphysGUI->isChecked();
}

MainWindow::~MainWindow()
{
    free(samplesForQCustomPlot);
    free(samplesForOpenEphysGUI);

    closeSocket();
    if (context)
    {
        zmq_ctx_destroy(context);
        context = 0;
    }

    closeCsvFile();
    delete plot;
    delete ui;
}


void MainWindow::createSocket()
{
    if (!socket)
    {
        socket = zmq_socket(context, ZMQ_PUB);

        if (!socket)
        {
            LOGD("Couldn't create a socket");
            LOGE(zmq_strerror(zmq_errno()));
            jassert(false);
        }

        auto urlstring = "tcp://*:" + std::to_string(port);

        if (zmq_bind(socket, urlstring.c_str()))
        {
            LOGD("Couldn't open data socket");
            LOGE(zmq_strerror(zmq_errno()));
            jassert(false);
        }

        qDebug()<<"Create ZMQ socket and bind port success"<<Qt::endl;
    }
}


void MainWindow::closeSocket()
{
    if (socket)
    {
        LOGD("Closing ZMQ socket");
        zmq_close(socket);
        socket = 0;
    }
}

//配置、打开串口
void MainWindow::on_pushButtonOpenPort_clicked()
{
    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity   checkBits;

    // 获取串口波特率
    baudRate = (QSerialPort::BaudRate)ui->comboBoxBaudRate->currentText().toUInt();
    // 获取串口数据位
    dataBits = (QSerialPort::DataBits)ui->comboBoxDataBits->currentText().toUInt();
    // 获取串口停止位
    if(ui->comboBoxStopBits->currentText() == "1"){
        stopBits = QSerialPort::OneStop;
    }else if(ui->comboBoxStopBits->currentText() == "1.5"){
        stopBits = QSerialPort::OneAndHalfStop;
    }else if(ui->comboBoxStopBits->currentText() == "2"){
        stopBits = QSerialPort::TwoStop;
    }else{
        stopBits = QSerialPort::OneStop;
    }
    // 获取串口奇偶校验位
    if(ui->comboBoxParity->currentText() == "无"){
        checkBits = QSerialPort::NoParity;
    }else if(ui->comboBoxParity->currentText() == "奇校验"){
        checkBits = QSerialPort::OddParity;
    }else if(ui->comboBoxParity->currentText() == "偶校验"){
        checkBits = QSerialPort::EvenParity;
    }else{
        checkBits = QSerialPort::NoParity;
    }
    // 初始化串口属性，设置 端口号、波特率、数据位、停止位、奇偶校验位数
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(dataBits);
    serialPort->setStopBits(stopBits);
    serialPort->setParity(checkBits);
    // 匹配带有串口设备信息的文本
    QString serialPortName = ui->comboBoxSerialPort->currentText();
    serialPortName = serialPortName.section(':', 0, 0);//spTxt.mid(0, spTxt.indexOf(":"));
    serialPort->setPortName(serialPortName);

    // 根据初始化好的串口属性，打开串口
    // 如果打开成功，反转打开按钮显示和功能。打开失败，无变化，并且弹出错误对话框。
    if(ui->pushButtonOpenPort->text() == "打开串口"){
        if(serialPort->open(QIODevice::ReadWrite) == true){
            ui->pushButtonOpenPort->setText("关闭串口");
            // 让端口号下拉框不可选，避免误操作（选择功能不可用，控件背景为灰色）
            ui->comboBoxSerialPort->setEnabled(false);
            ui->comboBoxBaudRate->setEnabled(false);
            ui->comboBoxStopBits->setEnabled(false);
            ui->comboBoxDataBits->setEnabled(false);
            ui->comboBoxParity->setEnabled(false);

            if(ui->actionSaveCsv->isChecked()){
                //以当前日期时间戳创建CSV文件
                openCsvFile();
            }
            //串口使用过程中锁定保存按钮不可用
            ui->actionSaveCsv->setEnabled(false);
        }else{
            QMessageBox::critical(this, "错误提示", "串口打开失败！！！\r\n\r\n该串口可能被占用，请选择正确的串口\r\n或者波特率过高，超出硬件支持范围");
        }
    }else{
        serialPort->close();
        ui->pushButtonOpenPort->setText("打开串口");
        // 端口号下拉框恢复可选
        ui->comboBoxSerialPort->setEnabled(true);
        ui->comboBoxBaudRate->setEnabled(true);
        ui->comboBoxStopBits->setEnabled(true);
        ui->comboBoxDataBits->setEnabled(true);
        ui->comboBoxParity->setEnabled(true);
        //关闭文件
        closeCsvFile();
        ui->actionSaveCsv->setEnabled(true);
    }
}

//清除接收窗口
void MainWindow::on_pushButtonClearRec_clicked()
{
    //(1)清空plainTextEditRec内容
    ui->plainTextEditRec->clear();
    //(2)清空字节计数
    curSendNum = 0;
    curRecvNum = 0;
    lastSendNum = 0;
    lastRecvNum = 0;
    curRecvFrameNum = 0;
    lastRecvFrameNum = 0;
    recvFrameCrcErrNum = 0;
    recvFrameMissNum = 0;
}

//清除发送窗口
void MainWindow::on_pushButtonClearSend_clicked()
{
    //(1)清空plainTextEditSend内容
    ui->plainTextEditSend->clear();
    //(2)清空发送字节计数
    curSendNum = 0;
    lastSendNum = 0;
}

//定时器槽函数timeout，1s 数据更新
void MainWindow::slot_timerUpdateLabel_timeout(void)
{
    //当前总计数-上次总结存暂存
    sendRate = curSendNum - lastSendNum;
    recvRate = curRecvNum - lastRecvNum;
    recvFrameRate = curRecvFrameNum - lastRecvFrameNum;
    //设置label
    ui->labelSendRate->setText(tr("Byte/s: %1").arg(sendRate));
    ui->labelRecvRate->setText(tr("Byte/s: %1").arg(recvRate));
    ui->labelRecvFrameRate->setText(tr("FPS: %1").arg(recvFrameRate));
    //暂存当前总计数
    lastSendNum = curSendNum;
    lastRecvNum = curRecvNum;
    lastRecvFrameNum = curRecvFrameNum;
    //更新label
    ui->labelSendNum->setText(tr("S: %1").arg(curSendNum));
    ui->labelRecvNum->setText(tr("R: %1").arg(curRecvNum));
    ui->labelRecvFrameNum->setText(tr("FNum: %1").arg(curRecvFrameNum));

    ui->labelRecvFrameErrNum->setText(tr("FErr: %1").arg(recvFrameCrcErrNum));
    ui->labelRecvFrameMissNum->setText(tr("FMiss: %1").arg(recvFrameMissNum));
}

//16进制发送选框，切换发送框内容为编码字符或16进制字节
void MainWindow::on_checkBoxHexSend_stateChanged(int arg1)
{
    changeEncodeStrAndHex(ui->plainTextEditSend, arg1);
}

//16进制接收选框，切换接收框内容为编码字符或16进制字节
void MainWindow::on_checkBoxHexRec_stateChanged(int arg1)
{
    changeEncodeStrAndHex(ui->plainTextEditRec, arg1);
}

//切换内容为编码字符或16进制字节
void MainWindow::changeEncodeStrAndHex(QPlainTextEdit *plainTextEdit, int arg1)
{
    QString strRecvData = plainTextEdit->toPlainText();
    QByteArray ba;
    QString str;

    // 获取多选框状态，未选为0，选中为2
    if(arg1 == Qt::Unchecked){
        // 多选框未勾选，接收区先前接收的16进制数据转换为asc2字符串格式
        //ba = QByteArray::fromHex(strRecvData1.toUtf8());               //Unicode(UTF8)编码，
        //str = QString(ba);//QString::fromUtf8(ba);                     //这里bytearray转为string时也可直接赋值，或者QString构造，因为QString存储的UTF-16编码
        ba = QByteArray::fromHex(strRecvData.toLocal8Bit());             //ANSI(GB2132)编码
        str =QString::fromLocal8Bit(ba);                                 //这里bytearray转为string一定要指定编码
    }
    else{
        // 多选框勾选，接收区先前接收asc2字符串转换为16进制显示
        //QByteArray ba = strRecvData1.toUtf8().toHex(' ').toUpper();    // Unicode(UTF8)编码(中国：E4 B8 AD E5 9B BD)
        ba = strRecvData.toLocal8Bit().toHex(' ').toUpper();             // ANSI(GB2132)编码(中国:D6 D0 B9 FA)
        str = QString(ba).append(' ');                                   //这里由16进制的bytearray转为string所有编码统一，所以可以直接构造，并没有歧义
    }

    // 文本控件清屏，显示新文本
    plainTextEdit->clear();
    plainTextEdit->insertPlainText(str);
    // 移动光标到文本结尾
    plainTextEdit->moveCursor(QTextCursor::End);
}

//tab1 单次发送按钮槽函数
void MainWindow::on_pushButtonSingleSend_clicked()
{
    QString strSendData = ui->plainTextEditSend->toPlainText();
    QByteArray ba;

    if(ui->checkBoxHexSend->checkState() == Qt::Unchecked){
        // 字符串形式发送
        //ba = strSendData.toUtf8();                            // Unicode编码输出
        ba = strSendData.toLocal8Bit();                         // GB2312编码输出,用以兼容大多数单片机
    }else{
        // 16进制发送
        //ba = QByteArray::fromHex(strSendData.toUtf8());       // Unicode编码输出
        ba = QByteArray::fromHex(strSendData.toLocal8Bit());    // GB2312编码输出
    }

    // 如发送成功，会返回发送的字节长度。失败，返回-1。
    int ret = serialPort->write(ba);

    if(ret > 0)
        curSendNum += ret;
}

//tab1 定时发送选框槽函数(开启单发定时器)
void MainWindow::on_checkBoxSingleSend_stateChanged(int arg1)
{
    int TimerInterval = ui->lineEditSingleSend->text().toInt();

    if(arg1 == Qt::Unchecked){
        timerSingleSend->stop();
        ui->lineEditSingleSend->setEnabled(true);
    }else{
        // 对输入的值做限幅，小于20ms会弹出对话框提示
        if(TimerInterval >= 20){
            timerSingleSend->start(TimerInterval);    // 设置定时时长
            ui->lineEditSingleSend->setEnabled(false);
        }else{
            ui->checkBoxSingleSend->setCheckState(Qt::Unchecked);
            QMessageBox::critical(this, "错误提示", "定时发送的最小间隔为 20ms\r\n请确保输入的值 >=20");
        }
    }
}

//tab2 数字标号单次发送按钮槽函数
void MainWindow::slot_pushButtonMultiSend_n_clicked()
{
    //获取信号发送者
    QPushButton *pushButton = qobject_cast<QPushButton *>(sender());
    QString strSendData;
    QByteArray ba;

    if(pushButton == ui->pushButtonMultiSend_1)
        strSendData = ui->lineEditMultiSend_1->text();
    if(pushButton == ui->pushButtonMultiSend_2)
        strSendData = ui->lineEditMultiSend_2->text();
    if(pushButton == ui->pushButtonMultiSend_3)
        strSendData = ui->lineEditMultiSend_3->text();
    if(pushButton == ui->pushButtonMultiSend_4)
        strSendData = ui->lineEditMultiSend_4->text();
    if(pushButton == ui->pushButtonMultiSend_5)
        strSendData = ui->lineEditMultiSend_5->text();
    if(pushButton == ui->pushButtonMultiSend_6)
        strSendData = ui->lineEditMultiSend_6->text();
    if(pushButton == ui->pushButtonMultiSend_7)
        strSendData = ui->lineEditMultiSend_7->text();
    if(pushButton == ui->pushButtonMultiSend_8)
        strSendData = ui->lineEditMultiSend_8->text();
    if(pushButton == ui->pushButtonMultiSend_9)
        strSendData = ui->lineEditMultiSend_9->text();
    if(pushButton == ui->pushButtonMultiSend_10)
        strSendData = ui->lineEditMultiSend_10->text();

    // 只16进制发送
    ba = QByteArray::fromHex(strSendData.toLocal8Bit());    // GB2312编码输出

    // 如发送成功，会返回发送的字节长度。失败，返回-1。
    int ret = serialPort->write(ba);

    if(ret > 0)
        curSendNum += ret;
}

//tab2 定时发送选框槽函数(开启多发定时器)
void MainWindow::on_checkBoxMultiSend_stateChanged(int arg1)
{
    int TimerInterval = ui->lineEditMultiSend->text().toInt();

    if(arg1 == Qt::Unchecked){
        timerMultiSend->stop();
        ui->lineEditMultiSend->setEnabled(true);
    }else{
        // 对输入的值做限幅，小于20ms会弹出对话框提示
        if(TimerInterval >= 20){
            timerMultiSend->start(TimerInterval);    // 设置定时时长
            ui->lineEditMultiSend->setEnabled(false);
        }else{
            ui->checkBoxSingleSend->setCheckState(Qt::Unchecked);
            QMessageBox::critical(this, "错误提示", "定时发送的最小间隔为 20ms\r\n请确保输入的值 >=20");
        }
    }
}

//tab2 轮训多发定时器
void MainWindow::slot_timerMultiSend_timeout()
{
    QString strSendData;
    QByteArray ba;

    if(ui->checkBoxMultiSend_1->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_1->text();
    if(ui->checkBoxMultiSend_2->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_2->text();
    if(ui->checkBoxMultiSend_3->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_3->text();
    if(ui->checkBoxMultiSend_4->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_4->text();
    if(ui->checkBoxMultiSend_5->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_5->text();
    if(ui->checkBoxMultiSend_6->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_6->text();
    if(ui->checkBoxMultiSend_7->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_7->text();
    if(ui->checkBoxMultiSend_8->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_8->text();
    if(ui->checkBoxMultiSend_9->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_9->text();
    if(ui->checkBoxMultiSend_10->checkState()==Qt::Checked)
        strSendData += ui->lineEditMultiSend_10->text();

    // 只16进制发送
    ba = QByteArray::fromHex(strSendData.toLocal8Bit());    // GB2312编码输出

    // 如发送成功，会返回发送的字节长度。失败，返回-1。
    int ret = serialPort->write(ba);

    if(ret > 0)
        curSendNum += ret;
}

//串口数据接收
void MainWindow::slot_serialPort_readyRead(void)
{
    QByteArray baRecvData = serialPort->readAll();
    QString str;
    QByteArray ba;

    //调用私有成员函数解析数据协议
    processRecvProtocol(&baRecvData);

    //已接收字节统计
    int baRecvDataSize = baRecvData.size();
    curRecvNum += baRecvDataSize;

    //是否显示接收内容
    if(ui->checkBoxStopShow->checkState() == Qt::Unchecked)
    {
        //以字符或16进制显示
        if(ui->checkBoxHexRec->checkState() == Qt::Unchecked){
            // 字符编码显示
            //str = QString::fromUtf8(baRecvData);      // Unicode编码
            str = QString::fromLocal8Bit(baRecvData);   // GB2312编码输入
        }else{
            // 16进制显示，用空格分隔，转换为大写
            ba = baRecvData.toHex(' ').toUpper();
            str = QString(ba).append(' ');
        }
        // 在当前位置插入文本，不会发生换行。(如果使用appendPlainText，则会自动换行)
        ui->plainTextEditRec->insertPlainText(str);
        // 移动光标到文本结尾.使得文本超出当前界面显示范围时，滚动轴自动向下滚动，以显示最新文本
        ui->plainTextEditRec->moveCursor(QTextCursor::End);

        // 判断多长的数据没有换行符，如果超过2000，会人为向数据接收区添加换行，来保证CPU占用率不会过高，不会导致卡顿
        // 但由于是先插入换行，后插入接收到的数据，所以每一箩数据并不是2000
        static int cnt=0;
        if(baRecvData.contains('\n')){
            cnt=0;
        }
        else{
            cnt += baRecvDataSize;
            if(cnt > 2000){
                ui->plainTextEditRec->appendPlainText(""); //添加一个回车换行
                cnt=0;
            }
        }
    }
}

//对接收数据流进行协议解析
void MainWindow::processRecvProtocol(QByteArray *baRecvData)
{
    uint32_t num = baRecvData->size();
    uint32_t num1 = num; //当前数据长度暂存
    uint32_t nextOffsetStartfindHeader = 0;
    uint32_t offset=0;

    // 在协议解析选项卡中显示单次串口接收到的数据
    if(showFramData){
        QString str1 = QString(baRecvData->toHex(' ').toUpper());
        ui->plainTextEditFrameData->appendPlainText(str1);    //自动换行
    }

    if(!num){
        qDebug()<<"recv 0 bytes"<<Qt::endl;
    }
    else{
        // 前一帧数据有余留,则需要合并余留数据到当前接收帧的前面
        if(needPushDataFront==true){
            baRecvData->push_front(baRecvDataBufTmp);
            baRecvDataBufTmp.clear();
            // 得到合并后的数据长度
            num = baRecvData->size();
        }

        // 从查找帧头开始检查帧格式是否正确
        STATE = S_FIND_HEADER;
        while(true){
            if(STATE==S_FIND_HEADER){
                //保证检查不会超出数据边界
                if(nextOffsetStartfindHeader+OFFSET_HEADER2>=num)
                    break;

                for(offset=nextOffsetStartfindHeader; offset<num; offset++){
                    if(((uint8_t)baRecvData->at(offset+OFFSET_HEADER1) == dataProtocol.header1)
                            &&((uint8_t)baRecvData->at(offset+OFFSET_HEADER2) == dataProtocol.header2)){
                        STATE = S_CHECK_COUNTER;
                        break;
                    }
                    else{
                        // 如果找不到帧头,则偏移1位继续查找
                        nextOffsetStartfindHeader = offset + 1;
                    }
                }
            }


            if(STATE==S_CHECK_COUNTER){
                //保证检查不会超出数据边界
                if(offset+OFFSET_COUNTER>=num)
                    break;

                uint8_t tmp = (uint8_t)baRecvData->at(offset+OFFSET_COUNTER);

                if(dataProtocol.counter==tmp){
                    if(!needPushDataFront){
                        // 如果累加值相等, 且当前数据未经过拼接, 则判定丢帧
                        qDebug()<<"recv miss frame: "<<dataProtocol.counter<<"->"<<tmp<<"num1:"<<num1<<"num: "<<num<<Qt::endl;
                    }
                }
                else if(dataProtocol.counter==255){
                    if(tmp!=0){
                        // counter值最大未255, 此时为边界情况
                        qDebug()<<"recv miss frame: "<<dataProtocol.counter<<"->"<<tmp<<"num1:"<<num1<<"num: "<<num<<Qt::endl;
                    }
                }else if(tmp-dataProtocol.counter!=1){
                    // 其他情况
                    qDebug()<<"recv miss frame: "<<dataProtocol.counter<<"->"<<tmp<<"num1:"<<num1<<"num: "<<num<<Qt::endl;
                }

                dataProtocol.counter = tmp;
                STATE = S_CHECK_DATATYPE;
            }

            if(STATE==S_CHECK_DATATYPE){
                //保证检查不会超出数据边界
                if(offset+OFFSET_DATATYPE>=num)
                    break;

                dataProtocol.dataType = (uint8_t)baRecvData->at(offset+OFFSET_DATATYPE);
                if((dataProtocol.dataType == 0x01)
                        ||(dataProtocol.dataType == 0x02)
                        ||(dataProtocol.dataType == 0x03)){
                    STATE = S_CHECK_DATASAMPLEBYTE;
                }
                else{
                    dataProtocol.dataType = 0;
                    nextOffsetStartfindHeader = offset + 1;
                    STATE = S_FIND_HEADER;
                }
            }

            if(STATE==S_CHECK_DATASAMPLEBYTE){
                //保证检查不会超出数据边界
                if(offset+OFFSET_DATASAMPLEBYTE>=num)
                    break;

                dataProtocol.dataSampleByte = (uint8_t)baRecvData->at(offset+OFFSET_DATASAMPLEBYTE);
                if((dataProtocol.dataSampleByte == 0x01)
                        ||(dataProtocol.dataSampleByte == 0x02)
                        ||(dataProtocol.dataSampleByte == 0x03)){
                    STATE = S_CHECK_DATACHANNEL;
                }
                else{
                    dataProtocol.dataSampleByte = 0;
                    nextOffsetStartfindHeader = offset + 1;
                    STATE = S_FIND_HEADER;
                }
            }


            if(STATE==S_CHECK_DATACHANNEL){
                //保证检查不会超出数据边界
                if(offset+OFFSET_DATACHANNEL>=num)
                    break;

                dataProtocol.dataChannel = (uint8_t)baRecvData->at(offset+OFFSET_DATACHANNEL);
                if(dataProtocol.dataChannel <= MAX_NUM_CHANNELS){
                    STATE = S_CHECK_TAIL;
                }
                else{
                    dataProtocol.dataChannel = 0;
                    nextOffsetStartfindHeader = offset + 1;
                    STATE = S_FIND_HEADER;
                }
            }

            if(STATE==S_CHECK_TAIL){
                uint32_t offsetTail1 = offset+OFFSET_DATACHANNEL+(dataProtocol.dataSampleByte*dataProtocol.dataChannel)+1;
                uint32_t offsetTail2 = offsetTail1 + 1;
                //保证检查不会超出数据边界
                if(offsetTail2>=num)
                    break;

                if(((uint8_t)baRecvData->at(offsetTail1) == dataProtocol.tail1)
                        &&((uint8_t)baRecvData->at(offsetTail2) == dataProtocol.tail2)){
                    nextOffsetStartfindHeader = offsetTail2 + 1;
                    curRecvFrameNum++; //有效帧数量计数
                    STATE = S_PROCESS_DATA;
                }
                else{
                    recvFrameCrcErrNum++; //误码帧数量计数
                    nextOffsetStartfindHeader = offset + 1;
                    STATE = S_FIND_HEADER;
                }
            }

            if(STATE==S_PROCESS_DATA){
                // 暂存当前帧有效数据首处位置
                uint32_t firstDataOffset = offset + OFFSET_DATA;

                // 在协议解析选显卡中打印帧格式正确的数据
                if(showPlotData){
                    for(int32_t i=0; i<dataProtocol.dataChannel*dataProtocol.dataSampleByte; i++){
                        baRecvDataBuf.append(baRecvData->at(firstDataOffset+i));
                    }
                    QString str2 = QString(baRecvDataBuf.toHex(' ').toUpper());
                    ui->plainTextEditPlotData->appendPlainText(str2);    //自动换行
                    baRecvDataBuf.clear();
                }

                // ADC数据
                if(dataProtocol.dataType==0x01){
                    uint32_t adcData;
                    float vol;

                    for(uint32_t i=0; i<dataProtocol.dataChannel*dataProtocol.dataSampleByte; i=i+dataProtocol.dataSampleByte){
                        if(dataProtocol.dataSampleByte==0x01){
                            adcData = (uint32_t)((uint8_t)baRecvData->at(firstDataOffset+i));     // byteArray->at() 返回char型, 不能直接转为uint32_t, 负数会出错
                            vol = (float)adcData/256 *3.3;      //电压转换公式
                        }
                        else if(dataProtocol.dataSampleByte==0x02){
                            adcData = ((uint32_t)((uint8_t)baRecvData->at(firstDataOffset+i))<<8) + (uint32_t)((uint8_t)baRecvData->at(firstDataOffset+i+1));
                            vol = (float)adcData/65535 *3.3;
                        }
                        else if(dataProtocol.dataSampleByte==0x03){
                            adcData = ((uint32_t)((uint8_t)baRecvData->at(firstDataOffset+i))<<16) + ((uint32_t)((uint8_t)baRecvData->at(firstDataOffset+i+1))<<8) + (uint32_t)((uint8_t)baRecvData->at(firstDataOffset+i+2));
                            vol = (float)adcData/16777216 *3.3;
                        }
                        // adc float数据存入缓存
                        samplesForQCustomPlot[dataSizeForQCustomPlot++] = vol;
                        samplesForOpenEphysGUI[dataSizeForOpenEphysGUI++] = vol;
                    }

                    // trigger数据存入缓存
                    eventCodesForOpenEphysGUI.push_back(lastEventCode);
                    // 当前块中adc采样点数
                    zmqData.numSamples++;
                }
                // 6轴数据
                else if(dataProtocol.dataType==0x02){
                    qDebug()<<"recv 6 axis data"<<Qt::endl;
                }
                // event数据
                else if(dataProtocol.dataType==0x03){
                    lastEventCode = (uint16_t)(uint8_t)baRecvData->at(firstDataOffset);
                }


                // 使用QCustomPlot绘制波形, 每次串口送入1个sample的数据, 即刻送入QCustomPlot
                if(useQCustomPlot){
                    dataSizeForQCustomPlot = 0;
                    plot->onNewDataArrived(samplesForQCustomPlot, dataProtocol.dataChannel);
                }
                else{
                    dataSizeForQCustomPlot = 0;
                }

                // 使用OpenEphysGUI绘制波形, 缓存串口送入数据, 直到有300个采样点, 再通过ZMQ发送出去
                if(useOpenEphysGUI){
                    if(zmqData.numSamples>300){
                        zmqData.numChannels = dataProtocol.dataChannel;
                        zmqData.timestamp = (double)GetTickCount()/1000;
                        zmqData.sampleRate = 0;

                        zmqSendData(samplesForOpenEphysGUI, zmqData.numChannels, zmqData.numSamples,
                                    zmqData.sampleNum, zmqData.timestamp, zmqData.sampleRate);

                        // 从采样开始记录的采样点位置
                        zmqData.sampleNum += zmqData.numSamples;
                        // 当前block采样点数清零
                        zmqData.numSamples = 0;
                        dataSizeForOpenEphysGUI = 0;
                        eventCodesForOpenEphysGUI.clear();
                    }
                }
                else{
                    zmqData.numSamples = 0;
                    dataSizeForOpenEphysGUI = 0;
                    eventCodesForOpenEphysGUI.clear();
                }

                STATE = S_FIND_HEADER;
            }
        }

        // 当前帧剩余数据无法解析成一个完整的帧, 则暂存预留数据
        if(nextOffsetStartfindHeader<num){
            needPushDataFront = true;

            for(uint32_t i=nextOffsetStartfindHeader; i<num; i++)
                baRecvDataBufTmp.push_back((uint8_t)baRecvData->at(i));

        }
        else{
            needPushDataFront = false;
        }

    }

}

void MainWindow::zmqSendData(const float *bufferChanPtrs,
                            uint32_t nChannels, uint32_t nSamples,
                            uint64_t sampleNumber, double timestamp, uint32_t sampleRate)
{
    messageNumber++;

    // Create message
    std::vector<float> flatsamples;
    flatsamples.reserve(nChannels * nSamples);

    for (uint32_t ch = 0; ch < nChannels; ch++)
    {
        for (uint32_t i = 0; i < nSamples; i++)
            flatsamples.push_back(bufferChanPtrs[ch + nChannels * i]);
    }

    auto samples = flatBuilder.CreateVector(flatsamples);
    auto event_codes = flatBuilder.CreateVector(eventCodesForOpenEphysGUI);

    auto streamName = flatBuilder.CreateString("Qt stream");
    auto zmqBuffer = openephysflatbuffer::CreateContinuousData(flatBuilder, samples, event_codes, streamName,
                                                               nChannels, nSamples, sampleNumber, timestamp,
                                                               messageNumber, sampleRate);
    flatBuilder.Finish(zmqBuffer);

    uint8_t *buf = flatBuilder.GetBufferPointer();
    int size = flatBuilder.GetSize();

    // Send packet
    zmq_msg_t request;
    zmq_msg_init_size(&request, size);
    memcpy(zmq_msg_data(&request), (void *)buf, size);
    int size_m = zmq_msg_send(&request, socket, 0);
    assert(size_m);
    zmq_msg_close(&request);

    //std::cout << "Sending packet " << messageNumber << " at " << Time::getHighResolutionTicks() << std::endl;

    flatBuilder.Clear();
}

//-----------------------

void MainWindow::on_checkBoxFrameData_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    showFramData = ui->checkBoxFrameData->isChecked();
}

void MainWindow::on_checkBoxPlotData_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    showPlotData = ui->checkBoxPlotData->isChecked();
}

void MainWindow::on_pushButtonClearFramePlotData_clicked()
{
    curRecvFrameNum = 0;
    lastRecvFrameNum=0;
    recvFrameCrcErrNum = 0;
    recvFrameMissNum = 0;
    ui->plainTextEditFrameData->clear();
    ui->plainTextEditPlotData->clear();
}

// 显示绘图窗口
void MainWindow::on_actionPlotShow_triggered()
{
    plot->show();
}

void MainWindow::slot_timerWaveGene_timeout()
{
//    DWORD time_start = GetTickCount(); //从操作系统启动经过的毫秒数
//    qDebug() << "t：" << time_start << Qt::endl;

    static float x;
    uint8_t y1,y2;
    static uint8_t counter=0;

    x += 0.01;
    y1 = 127 + sin(x)*50;
    y2 = 127 + cos(x)*100;


    QByteArray ba;
    ba.append(0x5A).append(0x5A).append(counter++).append(0x01).append(0x01).append(0x02);
    ba.append(y1).append(y2).append(0xA5).append(0xA5);
    // 如发送成功，会返回发送的字节长度。失败，返回-1。
    int ret = serialPort->write(ba);

    if(ret > 0)
        curSendNum += ret;
}


void MainWindow::on_checkBoxWaveGeneStart_stateChanged(int arg1)
{
    int TimerInterval = ui->lineEditWaveGeneInterval->text().toInt();

    if(arg1 == Qt::Checked){
        ui->lineEditWaveGeneInterval->setEnabled(false);
        timerWaveGene->start(TimerInterval);
    }

    else if(arg1 == Qt::Unchecked){
        timerWaveGene->stop();
        ui->lineEditWaveGeneInterval->setEnabled(true);
    }
}

//打开文件
void MainWindow::openCsvFile(void)
{
    m_csvFile = new QFile(QDateTime::currentDateTime().toString("yyyy-MM-d-HH-mm-ss-")+"data-out.csv");
    if(!m_csvFile)
      return;
    if (!m_csvFile->open(QIODevice::ReadWrite | QIODevice::Text))
      return;
    m_csvFileTextStream = new QTextStream(m_csvFile);
}

//关闭文件
void MainWindow::closeCsvFile(void)
{
    if(!m_csvFile)
        return;

    m_csvFile->close();

    if(m_csvFile)
        delete m_csvFile;
    if(m_csvFileTextStream)
        delete m_csvFileTextStream;
    m_csvFile = nullptr;
    m_csvFileTextStream = nullptr;
}

//保存CRC正确数据至文件
void MainWindow::saveCsvFile(QByteArray baRecvData)
{
    if(!m_csvFile)
        return;
    if(ui->actionSaveCsv->isChecked())
    {
        QByteArray ba = baRecvData.toHex(' ').toUpper();
        QString str(ba);
        *m_csvFileTextStream << str;
        *m_csvFileTextStream << "\n";
    }
}

void MainWindow::on_checkBoxUseQCustomPlot_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    useQCustomPlot = ui->checkBoxUseQCustomPlot->isChecked();
}


void MainWindow::on_checkBoxUseOpenEphysGUI_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    useOpenEphysGUI = ui->checkBoxUseOpenEphysGUI->isChecked();

    if(useOpenEphysGUI){
        createSocket();
    }
    else {
        closeSocket();
    }
}


void MainWindow::on_lineEditZmqTcpPort_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    port = ui->lineEditZmqTcpPort->text().toInt();
    closeSocket();
    createSocket();
}

