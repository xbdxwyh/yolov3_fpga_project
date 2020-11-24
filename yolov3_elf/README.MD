those code are used to complied to generate .elf file to run on fpga.
这块我没有使用SDK，而是使用命令直接编译的，在使vivado的环境变量生效以后，`arm-linux-gnueabihf-g++ -static -O3  * -o myfile.elf`,运行此命令会生成elf文件
