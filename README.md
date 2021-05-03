# Consumer-Producer-Threads
Operating Systems Multi-Threading exercise, using Threads, Mutex, Condition-Variables written with C on Linux. 
The Main thread creates N_PROD producers and N_CONS consumers. Each producer creates an "item" and adding it to a list, each consumer search for an item with "undone" status, doing a bit of processing on it until the status of the item changes to "DONE" and prints it to a log file.
Try it yourself: Just write "make" to the shell.
