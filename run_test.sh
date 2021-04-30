###
 # @Author: your name
 # @Date: 2021-04-30 16:27:19
 # @LastEditTime: 2021-04-30 16:31:33
 # @LastEditors: Please set LastEditors
 # @Description: In User Settings Edit
 # @FilePath: /pmdk-demo/run_test.sh
### 


./libpmem_demo --rand=1 --thread=1 --bs=64  >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=128 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=256 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=512 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=1024 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=2048 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=4096 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=8192 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=16384 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=32768 >> rand_1.test
./libpmem_demo --rand=1 --thread=1 --bs=65536 >> rand_1.test

./libpmem_demo --rand=1 --thread=2 --bs=64  >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=128 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=256 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=512 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=1024 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=2048 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=4096 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=8192 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=16384 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=32768 >> rand_2.test
./libpmem_demo --rand=1 --thread=2 --bs=65536 >> rand_2.test

./libpmem_demo --rand=1 --thread=4 --bs=64  >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=128 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=256 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=512 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=1024 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=2048 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=4096 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=8192 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=16384 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=32768 >> rand_4.test
./libpmem_demo --rand=1 --thread=4 --bs=65536 >> rand_4.test

./libpmem_demo --rand=1 --thread=6 --bs=64  >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=128 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=256 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=512 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=1024 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=2048 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=4096 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=8192 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=16384 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=32768 >> rand_6.test
./libpmem_demo --rand=1 --thread=6 --bs=65536 >> rand_6.test

./libpmem_demo --rand=1 --thread=8 --bs=64  >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=128 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=256 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=512 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=1024 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=2048 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=4096 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=8192 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=16384 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=32768 >> rand_8.test
./libpmem_demo --rand=1 --thread=8 --bs=65536 >> rand_8.test