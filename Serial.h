#ifndef _WZSERIALPORT_H
#define _WZSERIALPORT_H

#ifdef __cplusplus

#include <iostream>
using namespace std;
/*
���ߣ�ŷ��ΰ
���ڣ�2017-12-14
������WZSerialPort
��;�����ڶ�д
ʾ����
WZSerialPort w;
if(w.open("COM1",9600��0��8��1))
{
w.send("helloworld",10);
char buf[1024];
w.receive(buf,1024);
}
*/
 
class WZSerialPort
{
public:
	WZSerialPort();
	~WZSerialPort();
 
	// �򿪴���,�ɹ�����true��ʧ�ܷ���false
	// portname(������): ��Windows����"COM1""COM2"�ȣ���Linux����"/dev/ttyS1"��
	// baudrate(������): 9600��19200��38400��43000��56000��57600��115200 
	// parity(У��λ): 0Ϊ��У�飬1Ϊ��У�飬2ΪżУ�飬3Ϊ���У��
	// databit(����λ): 4-8��ͨ��Ϊ8λ
	// stopbit(ֹͣλ): 1Ϊ1λֹͣλ��2Ϊ2λֹͣλ,3Ϊ1.5λֹͣλ
	// synchronizable(ͬ�����첽): 0Ϊ�첽��1Ϊͬ��
	bool open(const char* portname, int baudrate = 115200, char parity = 0, char databit = 8, char stopbit = 1, char synchronizeflag = 0);
 
	//�رմ��ڣ���������
	void close();
 
	//�������ݻ�д���ݣ��ɹ����ط������ݳ��ȣ�ʧ�ܷ���0
	int send(string dat);
 
	//�������ݻ�����ݣ��ɹ����ض�ȡʵ�����ݵĳ��ȣ�ʧ�ܷ���0
	string receive();
 
private:
	int pHandle[16];
	char synchronizeflag;
};

#endif

//C++��C������ʱ��Ҫ
//extern "C" Ϊ C++����, extern ��C�����ɣã����ṩ��һ�����ӽ���ָ�����ţ�
//���ڸ��ߣã�����δ����ǣú�����extern ��C������ĺ�����ʹ�õ�C++����������,������C��
//������ΪC++�������к��������úܳ�����C���ɵĲ�һ�£�Ҳ������������һ��, ��ɲ��ܵ���
//extern "C" ��һ������Լ����ͨ��������ʵ�ּ���C��C++֮����໥���ã����Ե��ú����ܹ����һ�µ������
//ʹ��ͳһ����������ʹ��ʵ�ַ��ṩ�Ľӿں͵��÷���Ҫ�Ľӿھ�����ָ���������󣬵õ��Ķ���һ�µĺ�������������
#ifdef __cplusplus
extern "C" {
#endif

unsigned char serialOpen(const char *portname, int baudrate, char parity, char databit, char stopbit, char synchronizeflag);
unsigned char serialClose(void);
unsigned char serialRead(unsigned char *d, size_t order_size, size_t *result_size);
unsigned char serialWrite(unsigned char *d, size_t order_size, size_t *result_size);
int test_serial(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif
 
#endif
