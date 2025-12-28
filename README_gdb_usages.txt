Four usages :
1. Debug process, starting it with binary                    <--- need binary
2. Debug process, starting it with launch script             <--- need launch script
3. Debug running process                                     <--- need binary and pid
4. Debug dead process                                        <--- need binary and coredump




Suppose the binary is called "qfi_idq"
> cd ~/dev/ffwk/build/bin
> ./qfi_idq -i qfi_idq_mts_de_uat_prim -d 20250808 -t 123    <--- suppose it can be started by binary directly 
> python3 start.py -i qfi_idq_mts_de_uat_prim -a 11 -b 22    <--- suppose it can be started by launch script




[1. Debug process, starting it with binary]
> gdb --args ~/dev/ffwk/build/bin/qfi_idq -i qfi_idq_mts_de_uat_prim -d 20250808 -t 123
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(gdb) run
(gdb) break file.cpp:123
(gdb) break file.cpp:124 if x > 10
(gdb) break file.cpp:125 if x > 10 && ptr != nullptr
(gdb) c




[2. Debug process, starting it with launch script]           <--- CHANGE IN PROGRESS ........................
> gdb --args python3 start.py -i qfi_idq_mts_de_uat_prim -a 11 -b 22
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(gdb) set follow-fork-mode child                             <--- instruct gdb to debug the fork process (i.e. qfi_idq), instead of debug start.py
(gdb) catch exec                                             <--- it is a special break point when system call exec() is called
(gdb) run      
(gdb) info inferiors
(gdb) inferior <num>       
(gdb) break FRisk.h:27
(gdb) c




[3. Debug running process]
> ps -aux | grep chowdic | grep qfi_idq                      <--- suppose we get process id = 12345
> gdb ~/dev/ffwk/build/bin/qfi_idq 12345     
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(gdb) break file.cpp:123
(gdb) break file.cpp:124 if x > 10
(gdb) break file.cpp:125 if x > 10 && ptr != nullptr
(gdb) c




[4. Debug dead process]
> gdb ~/dev/ffwk/build/bin/qfi_idq /export/data/cores/core_pid_12345
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(gdb) bt





In our case
~/dev/replay/replay.sh            invokes python3           ~/dev/ffwk/build/script/chain.py           <--- this involves fork() & exec()
~/dev/ffwk/build/script/chain.py  invokes subprocess.Popen  ~/dev/ffwk/build/bin/qfi_idq               <--- this involves fork() & exec()

