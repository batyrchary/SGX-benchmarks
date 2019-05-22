# SGX-benchmarks



  ##### 1. Dependency
  Download and install the latest SGX SDK from https://01.org/intel-software-guard-extensions/downloads.
  
  
   ##### 2. Benchmarks
   1-Cost of provisioning multiple enclaves simultaneously
   2-Cost of entering and exiting from enclave (i.e. cost of copying data into enclave)
   3-Cost of running task inside enclave
   
   ##### 3. Notes
   - Tested on Linux
   - Heapsize of enclave should be enough to hold the data. You can configure heapsize by modifying values of <HeapMaxSize> inside ./Enclave/Enclave.config.xml file. By default it is set to 128MB. 
   
   
   ##### 4. Running
    ```sh
   $ make
   $ ./app
   ```
