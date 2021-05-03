# Consumer-Producer-Threads
Operating Systems exercise, using Threads, Mutex, Condition-Variables on Linux. 
The Main thread creates N_PROD producers and N_CONS consumers. Each producer creates an "item" and adding it to a list, each consumer search for an item with "undone" status, doing a bit of processing on it until the status of the item changes to "DONE" and prints it to a log file.
