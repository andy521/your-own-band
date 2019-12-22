/*
   采用外接电源单独供电，2 3口作为软串口接PC机作为调试端
   1 0为串口，连接WIFI模块
*/
#include <SoftwareSerial.h>
#include "edp.c"

#include<Servo.h>
Servo string1, string2, string3, string4;
int angle1, angle2;

#define KEY  "Bc1rkda2O1j6Ji=a4ltI7XHhEmQ="    //APIkey 
#define ID   "577848525"                          //设备ID
//#define PUSH_ID "680788"
#define PUSH_ID NULL

// 串口
#define _baudrate   9600
#define _rxpin      2
#define _txpin      3
#define WIFI_UART   dbgSerial
#define DBG_UART    Serial   //调试打印串口

void(* resetFunc) (void) = 0;

SoftwareSerial dbgSerial( _rxpin, _txpin ); // 软串口，调试打印
edp_pkt *pkt;

/*
* doCmdOk
* 发送命令至模块，从回复中获取期待的关键字
* keyword: 所期待的关键字
* 成功找到关键字返回true，否则返回false
*/
bool doCmdOk(String data, char *keyword)
{
  bool result = false;
  if (data != "")   //对于tcp连接命令，直接等待第二次回复
  {
    WIFI_UART.println(data);  //发送AT指令
    DBG_UART.print("SEND: ");
    DBG_UART.println(data);
  }
  if (data == "AT")   //检查模块存在
    delay(2000);
  else
    while (!WIFI_UART.available());  // 等待模块回复

  delay(200);
  if (WIFI_UART.find(keyword))   //返回值判断
  {
    DBG_UART.println("do cmd OK");
    result = true;
  }
  else
  {
    DBG_UART.println("do cmd ERROR");
    result = false;
  }
  while (WIFI_UART.available()) WIFI_UART.read();   //清空串口接收缓存
  delay(500); //指令时间间隔
  return result;
}


void setup()
{
  char buf[100] = {0};
  int tmp;

string1.attach(13);
string2.attach(12);
string3.attach(11);
string4.attach(10);                

  WIFI_UART.begin( _baudrate );
  DBG_UART.begin( _baudrate );
  WIFI_UART.setTimeout(3000);    //设置find超时时间
  delay(3000);
  DBG_UART.println("hello world!");

  delay(2000);
  digitalWrite(13, HIGH);   // 使Led亮

  while (!doCmdOk("AT+CWMODE=3", "OK"));            //工作模式
  while (!doCmdOk("AT+CWJAP=\"fxy\",\"fxylalala\"", "OK"));
  while (!doCmdOk("AT+CIPSTART=\"TCP\",\"183.230.40.39\",876", "CONNECT"));
  while (!doCmdOk("AT+CIPMODE=1", "OK"));           //透传模式
  while (!doCmdOk("AT+CIPSEND", ">"));              //开始发送
}

void loop()
{
  static int edp_connect = 0;
  bool trigger = false;
  edp_pkt rcv_pkt;
  unsigned char pkt_type;
  int i, tmp;
  char num[10];

  /* EDP 连接 */
  if (!edp_connect)
  {
    while (WIFI_UART.available()) WIFI_UART.read(); //清空串口接收缓存
    packetSend(packetConnect(ID, KEY));             //发送EPD连接包
    while (!WIFI_UART.available());                 //等待EDP连接应答
    if ((tmp = WIFI_UART.readBytes(rcv_pkt.data, sizeof(rcv_pkt.data))) > 0 )
    {
      rcvDebug(rcv_pkt.data, tmp);
      if (rcv_pkt.data[0] == 0x20 && rcv_pkt.data[2] == 0x00 && rcv_pkt.data[3] == 0x00)
      {
        edp_connect = 1;
        DBG_UART.println("EDP connected.");
      }
      else
        DBG_UART.println("EDP connect error.");
    }
    packetClear(&rcv_pkt);
  }

  while (WIFI_UART.available())
  {
    readEdpPkt(&rcv_pkt);
    if (isEdpPkt(&rcv_pkt))
    {
      pkt_type = rcv_pkt.data[0];
      switch (pkt_type)
      {
        case CMDREQ:
          char edp_command[50];
          char edp_cmd_id[40];
          long id_len, cmd_len, rm_len;
          char datastr[20];
          char val[10];
          memset(edp_command, 0, sizeof(edp_command));
          memset(edp_cmd_id, 0, sizeof(edp_cmd_id));
          edpCommandReqParse(&rcv_pkt, edp_cmd_id, edp_command, &rm_len, &id_len, &cmd_len);
          DBG_UART.print("rm_len: ");
          DBG_UART.println(rm_len, DEC);
          delay(10);
          DBG_UART.print("id_len: ");
          DBG_UART.println(id_len, DEC);
          delay(10);
          DBG_UART.print("cmd_len: ");
          DBG_UART.println(cmd_len, DEC);
          delay(10);
          DBG_UART.print("id: ");
          DBG_UART.println(edp_cmd_id);
          delay(10);
          DBG_UART.print("cmd: ");
          DBG_UART.println(edp_command);

          //数据处理与应用中EDP命令内容对应
          //本例中格式为  datastream:[1/0] 
          angle1 = 100;
          angle2 = 80;
          string1.write(90);
          string2.write(90);
          string3.write(90);
          string4.write(90);
          delay(1000);
          sscanf(edp_command, "%[^:]:%s", datastr, val);
          if (atoi(val) == 1)
            {
            //洋娃娃和小熊跳舞
            //1行
            string3.write(angle1);//1
            delay(500);
            string3.write(angle2);//2
            delay(500);
            string2.write(angle2);//3
            delay(500);
            string2.write(angle1);//4
            delay(500);
            string4.write(angle2);//5
            delay(500);
            string4.write(angle1);//5
            delay(500);
            string4.write(angle2);//5
            delay(250);
            string2.write(angle2);//4
            delay(250);
            string2.write(angle1);//3
            delay(500);
            string2.write(angle1);//4
            delay(500);
            string2.write(angle2);//4
            delay(500);
            string2.write(angle1);//4
            delay(250);
            string2.write(angle2);//3
            delay(250);
            string3.write(angle1);//2
            delay(500);
            string3.write(angle2);//1
            delay(500);
            string2.write(angle1);//3
            delay(500);
            string4.write(angle1);//5
            delay(1000);
            
            //2行
            string3.write(angle1);//1
            delay(500);
            string3.write(angle2);//2
            delay(500);
            string2.write(angle2);//3
            delay(500);
            string2.write(angle1);//4
            delay(500);
            string4.write(angle2);//5
            delay(500);
            string4.write(angle1);//5
            delay(500);
            string4.write(angle2);//5
            delay(250);
            string2.write(angle2);//4
            delay(250);
            string2.write(angle1);//3
            delay(500);
            string2.write(angle2);//4
            delay(500);
            string2.write(angle1);//4
            delay(500);
            string2.write(angle2);//4
            delay(250);
            string2.write(angle1);//3
            delay(250);
            string3.write(angle1);//2
            delay(500);
            string3.write(angle2);//1
            delay(500);
            string2.write(angle2);//3
            delay(500);
            string3.write(angle1);//1
            delay(1000);
            
            //3行
            string1.write(angle1);//6
            delay(500);
            string1.write(angle2);//6
            delay(500);
            string1.write(angle1);//6
            delay(250);
            string4.write(angle1);//5
            delay(250);
            string2.write(angle1);//4
            delay(500);
            string4.write(angle2);//5
            delay(500);
            string4.write(angle1);//5
            delay(500);
            string4.write(angle2);//5
            delay(250);
            string2.write(angle2);//4
            delay(250);
            string2.write(angle1);//3
            delay(500);
            string2.write(angle2);//4
            delay(500);
            string2.write(angle1);//4
            delay(500);
            string2.write(angle2);//4
            delay(250);
            string2.write(angle1);//3
            delay(250);
            string3.write(angle2);//2
            delay(500);
            string3.write(angle1);//1
            delay(500);
            string2.write(angle2);//3
            delay(500);
            string4.write(angle1);//5
            delay(1000);

            //4
            string1.write(angle2);//6
            delay(500);
            string1.write(angle1);//6
            delay(500);
            string1.write(angle2);//6
            delay(250);
            string4.write(angle2);//5
            delay(250);
            string2.write(angle1);//4
            delay(500);
            string4.write(angle1);//5
            delay(500);
            string4.write(angle2);//5
            delay(500);
            string4.write(angle1);//5
            delay(250);
            string2.write(angle2);//4
            delay(250);
            string2.write(angle1);//3
            delay(500);
            string2.write(angle2);//4
            delay(500);
            string2.write(angle1);//4
            delay(500);
            string2.write(angle2);//4
            delay(250);
            string2.write(angle1);//3
            delay(250);
            string3.write(angle2);//2
            delay(500);
            string3.write(angle1);//1
            delay(500);
            string2.write(angle1);//3
            delay(500);
            string3.write(angle1);//5
            delay(1000);
            }
          else if (atoi(val) == 2)
          {
            //晴天
            string1.write(angle1);//故
            delay(250);
            string1.write(angle2);//事
            delay(250);
            string2.write(angle1);//的
            delay(250);
            string2.write(angle2);//小
            delay(500);
            string4.write(angle1);//黄
            delay(250);
            string1.write(angle1);//花
            delay(500);
            string1.write(angle2);//从
            delay(250);
            string1.write(angle1);//出
            delay(250);
            string2.write(angle1);//生
            delay(250);
            string2.write(angle2);//那
            delay(250);
            string4.write(angle2);//年
            delay(125);
            string1.write(angle1);//就
            delay(125);
            string4.write(angle1);//飘
            delay(125);
            string2.write(angle1);//
            delay(125);
            string3.write(angle1);//着
            delay(500);
            string1.write(angle2);//童
            delay(250);
            string1.write(angle1);//年
            delay(250);
            string2.write(angle2);//的
            delay(250);
            string2.write(angle1);//荡
            delay(500);
            string4.write(angle2);//秋
            delay(250);
            string1.write(angle2);//千
            delay(500);
            string1.write(angle2);//随
            delay(500);
            string4.write(angle1);//记
            delay(125);
            string1.write(angle1);//忆
            delay(125);
            string1.write(angle2);//一
            delay(125);
            string1.write(angle1);//直
            delay(125);
            string4.write(angle2);//晃
            delay(125);
            string1.write(angle2);//到
            delay(125);
            string1.write(angle1);//现
            delay(125);
            string2.write(angle2);//在
            delay(125);
            string3.write(angle2);//re
            delay(250);
            string2.write(angle1);//so
            delay(250);
            string2.write(angle2);//so
            delay(250);
            string1.write(angle2);//si
            delay(250);
            string1.write(angle1);//do
            delay(250);
            string1.write(angle2);//si
            delay(250);
            string4.write(angle1);//la
            delay(250);
            string2.write(angle1);//so
            delay(125);
            string4.write(angle2);//la
            delay(125);
            string1.write(angle2);//si
            delay(250);
            string1.write(angle1);//si
            delay(250);
            string1.write(angle2);//si
            delay(250);
            string1.write(angle1);//si
            delay(250);
            string4.write(angle1);//la
            delay(125);
            string1.write(angle2);//si
            delay(125);
            string4.write(angle2);//la
            delay(250);
            string2.write(angle2);//so
            delay(500);
            string3.write(angle1);//吹
            delay(250);
            string2.write(angle1);//着
            delay(250);
            string2.write(angle2);//前
            delay(250);
            string1.write(angle1);//奏
            delay(250);
            string1.write(angle2);//望
            delay(250);
            string1.write(angle1);//着
            delay(250);
            string4.write(angle2);//天
            delay(250);
            string2.write(angle1);//空
            delay(125);
            string4.write(angle1);//我
            delay(125);
            string1.write(angle2);//想
            delay(250);
            string1.write(angle1);//起
            delay(250);
            string1.write(angle2);//花
            delay(250);
            string1.write(angle1);//瓣
            delay(250);
            string4.write(angle2);//试
            delay(125);
            string1.write(angle2);//着
            delay(125);
            string4.write(angle2);//掉
            delay(250);
            string2.write(angle2);//落
            delay(375);
            string2.write(angle1);//为
            delay(125);
            string2.write(angle2);//你
            delay(125);
            string2.write(angle1);//翘
            delay(125);
            string2.write(angle2);//课
            delay(125);
            string2.write(angle1);//的
            delay(125);
            string2.write(angle2);//那
            delay(125);
            string2.write(angle1);//一
            delay(250);
            string2.write(angle2);//天
            delay(250);
            string2.write(angle1);//花
            delay(125);
            string2.write(angle2);//落
            delay(125);
            string2.write(angle1);//的
            delay(125);
            string2.write(angle2);//那
            delay(125);
            string2.write(angle1);//一
            delay(250);
            string2.write(angle2);//天
            delay(250);
            string2.write(angle1);//教
            delay(125);
            string2.write(angle2);//室
            delay(125);
            string2.write(angle1);//的
            delay(125);
            string2.write(angle2);//哪
            delay(125);
            string2.write(angle1);//一
            delay(250);
            string2.write(angle2);//间
            delay(250);
            string2.write(angle1);//我
            delay(125);
            string2.write(angle2);//怎
            delay(125);
            string2.write(angle1);//么
            delay(125);
            string1.write(angle1);//看
            delay(125);
            string1.write(angle2);//不
            delay(250);
            string1.write(angle1);//见
            delay(250);
            string1.write(angle2);//消
            delay(125);
            string1.write(angle1);//失
            delay(125);
            string1.write(angle2);//的
            delay(125);
            string1.write(angle1);//下
            delay(125);
            string1.write(angle2);//雨
            delay(250);
            string1.write(angle1);//天
            delay(250);
            string1.write(angle2);//我
            delay(125);
            string1.write(angle1);//好
            delay(125);
            string1.write(angle2);//想
            delay(125);
            string1.write(angle1);//再
            delay(125);
            string1.write(angle2);//淋
            delay(125);
            string1.write(angle1);//一
            delay(125);
            string1.write(angle2);//遍
            delay(1000);
            string2.write(angle2);//没
            delay(125);
            string2.write(angle1);//想
            delay(125);
            string2.write(angle2);//到
            delay(125);
            string2.write(angle1);//失
            delay(125);
            string3.write(angle2);//去
            delay(250);
            string2.write(angle2);//的
            delay(250);
            string2.write(angle1);//勇
            delay(250);
            string1.write(angle1);//气
            delay(250);
            string1.write(angle2);//我
            delay(250);
            string1.write(angle1);//还
            delay(250);
            string2.write(angle2);//留
            delay(250);
            string2.write(angle1);//着
            delay(1000);
            string3.write(angle1);//好
            delay(125);
            string3.write(angle2);//想
            delay(125);
            string2.write(angle2);//再
            delay(125);
            string2.write(angle1);//问
            delay(125);
            string1.write(angle2);//一
            delay(250);
            string2.write(angle2);//遍
            delay(250);
            string3.write(angle1);//你
            delay(250);
            string2.write(angle1);//会
            delay(250);
            string2.write(angle2);//等
            delay(250);
            string1.write(angle1);//待
            delay(250);
            string1.write(angle2);//还
            delay(250);
            string1.write(angle1);//是
            delay(250);
            string2.write(angle1);//离
            delay(250);
            string4.write(angle1);//开
            delay(1000);
            }
            
          
          else if(atoi(val) == 3)
          {//安静
          //1行
          string1.write(angle1);//只
          delay(250);
          string1.write(angle2);//剩
          delay(250);
          string1.write(angle1);//下
          delay(250);
          string1.write(angle2);//钢
          delay(250);
          string4.write(angle1);//琴
          delay(250);
          string2.write(angle1);//陪
          delay(250);
          string2.write(angle2);//我
          delay(250);
          string4.write(angle2);//谈
          delay(250);
          string4.write(angle1);//了
          delay(250);
          string4.write(angle2);//一
          delay(250);
          string2.write(angle1);//天
          delay(750);
          string3.write(angle1);//睡
          delay(250);
          string1.write(angle1);//着
          delay(250);
          string4.write(angle1);//的
          delay(250);
          string2.write(angle2);//大 
          delay(250);
          string2.write(angle1);//提
          delay(250); 
          string2.write(angle2);//琴
          delay(750);
          string3.write(angle2);//安
          delay(250);
          string1.write(angle2);//静
          delay(250);
          string4.write(angle2);//的
          delay(250);
          string2.write(angle1);//旧
          delay(250);
          string2.write(angle2);//旧
          delay(250);
          string4.write(angle1);//的
          delay(500);
           
           //2行
          string1.write(angle1);//我
          delay(250);
          string1.write(angle2);//想
          delay(250);
          string1.write(angle1);//你
          delay(250);
          string1.write(angle2);//已
          delay(250);
          string4.write(angle2);//表
          delay(250); 
          string2.write(angle1);//现
          delay(250); 
          string2.write(angle2);//得
          delay(250); 
          string4.write(angle1);//非
          delay(250);
          string4.write(angle2);//常
          delay(250);
          string4.write(angle1);//明
          delay(250);
          string2.write(angle1);//白
          delay(750);
          string3.write(angle1);//我
          delay(250);
          string1.write(angle1);//懂
          delay(250);
          string4.write(angle2);//我
          delay(250);
           string2.write(angle2);//也
          delay(250);
          string2.write(angle1);//知
          delay(250);
          string2.write(angle2);//道
          delay(750);
          string3.write(angle2);//你
          delay(250);
          string1.write(angle2);//没
          delay(250);
          string4.write(angle1);//有
          delay(250);
          string2.write(angle1);//舍
          delay(250);
          string2.write(angle2);//不
          delay(250);
          string4.write(angle2);//得
          delay(500);
          string1.write(angle1);//-
          delay(250);
          string1.write(angle2);//-
          delay(750);
          
          //3行
          string1.write(angle1);//你
         delay(250);
          string1.write(angle2);//说
          delay(250);
          string1.write(angle1);//你
          delay(250);
          string1.write(angle2);//也
          delay(250);
          string1.write(angle1);//会
          delay(250);
          string4.write(angle1);//难
          delay(250);
          string2.write(angle1);//过
          delay(250);
          string2.write(angle2);//我
          delay(250);
          string2.write(angle1);//不
          delay(250);
          string4.write(angle2);//相
          delay(250);
          string4.write(angle1);//信
          delay(500);
          string3.write(angle1);//牵
          delay(250);
          string1.write(angle1);//着
          delay(250);
          string1.write(angle2);//你
          delay(250);
          string1.write(angle1);//陪
          delay(250);
          string1.write(angle2);//着
          delay(250);
          string1.write(angle1);//我
          delay(250);
          string4.write(angle2);//也
          delay(250);
          string4.write(angle1);//只
          delay(250);
          string4.write(angle2);//是
          delay(250); 
          string1.write(angle2);//曾
          delay(750);
          string1.write(angle1);//经
          delay(875);
          string3.write(angle2);//希
          delay(250);
          
          //4行
          string1.write(angle2);//望
          delay(250);
          string1.write(angle1);//他
          delay(250);
          string1.write(angle2);//是
          delay(250);
          string1.write(angle1);//真
          delay(250);
          string4.write(angle1);//的
          delay(250); 
          string2.write(angle2);//比
          delay(250);
          string2.write(angle1);//我
          delay(250);
          string2.write(angle2);//还
          delay(250);
          string2.write(angle1);//要
          delay(250);
          string2.write(angle2);//爱
          delay(250); 
          string3.write(angle2);//你
          delay(500);
          string1.write(angle2);//-
          delay(500);
          string1.write(angle1);//我
          delay(250);
          string1.write(angle2);//才
          delay(250);
          string1.write(angle1);//会
          delay(250);
          string1.write(angle2);//逼
          delay(250);
          string1.write(angle1);//自
          delay(500);
          string4.write(angle2);//己
          delay(250); 
          string2.write(angle1);//离
          delay(250); 
          string4.write(angle1);//开
          delay(500); 
          string3.write(angle1);//你
          delay(250);
          string1.write(angle2);//要
          delay(250);
          string1.write(angle1);//我
          delay(250);
          string1.write(angle2);//说
          delay(250);
          string1.write(angle1);//多
          delay(250);
          string1.write(angle2);//难
          delay(250);
          string1.write(angle1);//堪
          delay(500);

          //5行
          string3.write(angle2);//我
          delay(250);
          string1.write(angle2);//根
          delay(250);
          string1.write(angle1);//本
          delay(250);
          string1.write(angle2);//不
          delay(250);
          string1.write(angle1);//想
          delay(250);
          string1.write(angle2);//分
          delay(250);
          string1.write(angle1);//开
          delay(500);
           string3.write(angle1);//为
          delay(250);
          string1.write(angle2);//什
          delay(250);
          string1.write(angle1);//么
          delay(250);
          string1.write(angle2);//还
          delay(250);
          string1.write(angle1);//要
          delay(250);
          string1.write(angle2);//我
          delay(250);
          string2.write(angle2);//用
          delay(250); 
          string4.write(angle2);//微
          delay(250); 
          string4.write(angle1);//笑
          delay(250); 
          string4.write(angle2);//来
          delay(250); 
          string1.write(angle1);//带
          delay(500);
          string2.write(angle1);//过
          delay(1000); 
          string1.write(angle2);//我
          delay(250);
          string1.write(angle1);//没
          delay(250);
          string2.write(angle2);//有
          delay(250); 
           string2.write(angle1);//这
          delay(250); 
           string2.write(angle2);//种
          delay(250); 
           string2.write(angle1);//天
          delay(250); 
           string2.write(angle2);//分
          delay(500); 
         
          //6行
          string1.write(angle2);//包
          delay(250);
          string1.write(angle1);//容
          delay(250);
          string2.write(angle1);//你
          delay(250); 
           string2.write(angle2);//也
          delay(250); 
           string2.write(angle1);//接
          delay(250); 
           string2.write(angle2);//受
          delay(250); 
           string2.write(angle1);//他
          delay(500); 
          string1.write(angle2);//不
          delay(250);
          string1.write(angle1);//用
          delay(250);
          string1.write(angle2);//担
          delay(250);
          string1.write(angle1);//心
          delay(250);
          string4.write(angle1);//得
          delay(250); 
          string4.write(angle2);//太
          delay(250); 
          string2.write(angle2);//多
          delay(500); 
          string1.write(angle2);//我
          delay(250);
          string1.write(angle1);//会
          delay(250);
          string1.write(angle2);//一
          delay(250);
          string1.write(angle1);//直
          delay(250);
          string4.write(angle1);//好
          delay(250); 
          string4.write(angle2);//好
          delay(250); 
          string2.write(angle1);//过
          delay(500); 
          string3.write(angle2);//你
          delay(500); 
          string1.write(angle2);//已
          delay(250);
          string1.write(angle1);//经
          delay(250);
          string1.write(angle2);//远
          delay(250);
          string1.write(angle1);//远
          delay(250);
          string1.write(angle2);//离
          delay(250);
          string1.write(angle1);//开
          delay(500);
          
          //7行
          string3.write(angle1);//我
          delay(250); 
           string1.write(angle2);//也
          delay(250);
          string1.write(angle1);//会
          delay(250);
          string1.write(angle2);//慢
          delay(250);
          string1.write(angle1);//慢
          delay(250);
          string1.write(angle2);//走
          delay(250);
          string1.write(angle1);//开
          delay(500);
          string3.write(angle2);//为
          delay(250); 
           string1.write(angle2);//什
          delay(250);
          string1.write(angle1);//么
          delay(250);
          string1.write(angle2);//我
          delay(250);
          string1.write(angle1);//连
          delay(250);
          string1.write(angle2);//分
          delay(250);
          string2.write(angle2);//开
          delay(250); 
          string4.write(angle1);//都
          delay(250); 
          string4.write(angle2);//迁
          delay(250); 
          string4.write(angle1);//就
          delay(250); 
          string1.write(angle1);//着
          delay(500);
          string2.write(angle1);//你
          delay(500);
          string1.write(angle2);//我
          delay(250);
          string1.write(angle1);//真
          delay(250);
          string2.write(angle2);//的
          delay(250); 
           string2.write(angle1);//没
          delay(250); 
           string2.write(angle2);//有
          delay(250); 
           string2.write(angle1);//天
          delay(250); 
           string2.write(angle2);//分
          delay(500); 
          
          //8行
          string1.write(angle2);//安
          delay(250);
          string1.write(angle1);//静
          delay(250);
          string2.write(angle1);//的
          delay(250); 
           string2.write(angle2);//没
          delay(250); 
           string2.write(angle1);//这
          delay(250); 
           string2.write(angle2);//么
          delay(250); 
          string2.write(angle1);//快
          delay(500); 
          string1.write(angle2);//我
          delay(250);
          string1.write(angle1);//会
          delay(250);
          string1.write(angle2);//学
          delay(250);
          string1.write(angle1);//着
          delay(250);
          string4.write(angle1);//放
          delay(250); 
          string4.write(angle2);//弃
          delay(250); 
          string2.write(angle2);//你
          delay(500); 
          string1.write(angle2);//是
          delay(250);
          string1.write(angle1);//因
          delay(250);
          string4.write(angle1);//为
          delay(250); 
          string2.write(angle1);//我
          delay(250); 
          string3.write(angle1);//太
          delay(250); 
          string2.write(angle2);//爱
          delay(250); 
          string2.write(angle1);//你
          delay(1000); 
            }
            

          packetSend(packetDataSaveTrans(NULL, datastr, val)); //将新数据值上传至数据流
          break;
        default:
          DBG_UART.print("unknown type: ");
          DBG_UART.println(pkt_type, HEX);
          break;
      }
    }
    //delay(4);
  }
  if (rcv_pkt.len > 0)
    packetClear(&rcv_pkt);
  delay(150);
}

/*
* readEdpPkt
* 从串口缓存中读数据到接收缓存
*/
bool readEdpPkt(edp_pkt *p)
{
  int tmp;
  if ((tmp = WIFI_UART.readBytes(p->data + p->len, sizeof(p->data))) > 0 )
  {
    rcvDebug(p->data + p->len, tmp);
    p->len += tmp;
  }
  return true;
}

/*
* packetSend
* 将待发数据发送至串口，并释放到动态分配的内存
*/
void packetSend(edp_pkt* pkt)
{
  if (pkt != NULL)
  {
    WIFI_UART.write(pkt->data, pkt->len);    //串口发送
    WIFI_UART.flush();
    free(pkt);              //回收内存
  }
}

void rcvDebug(unsigned char *rcv, int len)
{
  int i;

  DBG_UART.print("rcv len: ");
  DBG_UART.println(len, DEC);
  for (i = 0; i < len; i++)
  {
    DBG_UART.print(rcv[i], HEX);
    DBG_UART.print(" ");
  }
  DBG_UART.println("");
}
