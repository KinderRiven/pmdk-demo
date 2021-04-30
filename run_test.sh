###
 # @Author: your name
 # @Date: 2021-04-30 16:27:19
 # @LastEditTime: 2021-04-30 18:39:25
 # @LastEditors: Please set LastEditors
 # @Description: In User Settings Edit
 # @FilePath: /pmdk-demo/run_test.sh
### 

s_type=(rw)
s_thread=(1 2 4 6 8 10 12)
s_bs=(64 128 256 512 1024 2048 4096 8192 16384 32768 65536)

for ((i=0; i<${#s_type[*]}; i+=1))
do
    for ((j=0; j<${#s_thread[*]}; j+=1))
    do
        OUTPUT=${s_type[$i]}_${s_thread[$j]}.result
        for ((k=0; k<${#s_bs[*]}; k+=1))
        do
        echo "./libpmem_demo --type=${s_type[$i]} --thread=${s_thread[$j]} --bs=${s_bs[$k]} >> $OUTPUT"
        ./libpmem_demo --type=${s_type[$i]} --thread=${s_thread[$j]} --bs=${s_bs[$k]} >> $OUTPUT
        done
    done
done