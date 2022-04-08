#ifndef _TEST_CPP_H
#define _TEST_CPP_H

//C++和C互调用时需要
//extern "C" 为 C++特性, extern “C”是由Ｃ＋＋提供的一个连接交换指定符号，
//用于告诉Ｃ＋＋这段代码是Ｃ函数。extern “C”后面的函数不使用的C++的名字修饰,而是用C。
//这是因为C++编译后库中函数名会变得很长，与C生成的不一致，也就是命名规则不一样, 造成不能调用
//extern "C" 是一种链接约定，通过它可以实现兼容C与C++之间的相互调用，即对调用函数能够达成一致的意见，
//使用统一的命名规则，使得实现方提供的接口和调用方需要的接口经按照指定规则编译后，得到的都是一致的函数符号命名。
#ifdef __cplusplus
extern "C" {
#endif

int test_cpp(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif
