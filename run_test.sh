###
 # @Author: your name
 # @Date: 2021-04-30 16:27:19
 # @LastEditTime: 2021-04-30 18:38:03
 # @LastEditors: Please set LastEditors
 # @Description: In User Settings Edit
 # @FilePath: /pmdk-demo/run_test.sh
### 

s_bs=(64 128 256)
s_type=(rr rw sr sw)
s_thread=(1 2 4)

for ((i=0; i<${#s_type[*]}; i+=1))
do
    for ((j=0; j<${#s_thread[*]}; j+=1))
    do
        OUTPUT=${s_type[$i]}_${s_thread[$j]}.result
        for ((k=0; k<${#s_bs[*]}; k+=1))
        do
        ./libpmem_demo --type=${s_type[$i]} --thread=${s_thread[$j]} --bs=${s_bs[$k]} >> $OUTPUT
        done
    done
done